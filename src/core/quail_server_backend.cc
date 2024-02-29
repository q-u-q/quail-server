
#include "core/quail_server_backend.h"

#include "absl/strings/match.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "quiche/common/platform/api/quiche_file_utils.h"
#include "quiche/common/quiche_text_utils.h"
#include "quiche/quic/core/http/spdy_utils.h"
#include "quiche/quic/platform/api/quic_bug_tracker.h"
#include "quiche/quic/platform/api/quic_logging.h"

#include "quit_converter.h"
#include "quit_web_transport_visitors.h"

using spdy::Http2HeaderBlock;
// using spdy::kV3LowestPriority;

namespace quail {

QuailServerBackend::ResourceFile::ResourceFile(const std::string& file_name)
    : file_name_(file_name) {}

QuailServerBackend::ResourceFile::~ResourceFile() = default;

void QuailServerBackend::ResourceFile::Read() {
  std::optional<std::string> maybe_file_contents =
      quiche::ReadFileContents(file_name_);
  if (!maybe_file_contents) {
    QUIC_LOG(DFATAL) << "Failed to read file for the memory cache backend: "
                     << file_name_;
    return;
  }
  file_contents_ = *maybe_file_contents;

  // First read the headers.
  for (size_t start = 0; start < file_contents_.length();) {
    size_t pos = file_contents_.find('\n', start);
    if (pos == std::string::npos) {
      QUIC_LOG(DFATAL) << "Headers invalid or empty, ignoring: " << file_name_;
      return;
    }
    size_t len = pos - start;
    // Support both dos and unix line endings for convenience.
    if (file_contents_[pos - 1] == '\r') {
      len -= 1;
    }
    absl::string_view line(file_contents_.data() + start, len);
    start = pos + 1;
    // Headers end with an empty line.
    if (line.empty()) {
      body_ = absl::string_view(file_contents_.data() + start,
                                file_contents_.size() - start);
      break;
    }
    // Extract the status from the HTTP first line.
    if (line.substr(0, 4) == "HTTP") {
      pos = line.find(' ');
      if (pos == std::string::npos) {
        QUIC_LOG(DFATAL) << "Headers invalid or empty, ignoring: "
                         << file_name_;
        return;
      }
      spdy_headers_[":status"] = line.substr(pos + 1, 3);
      continue;
    }
    // Headers are "key: value".
    pos = line.find(": ");
    if (pos == std::string::npos) {
      QUIC_LOG(DFATAL) << "Headers invalid or empty, ignoring: " << file_name_;
      return;
    }
    spdy_headers_.AppendValueOrAddHeader(
        quiche::QuicheTextUtils::ToLower(line.substr(0, pos)),
        line.substr(pos + 2));
  }

  // The connection header is prohibited in HTTP/2.
  spdy_headers_.erase("connection");

  // Override the URL with the X-Original-Url header, if present.
  if (auto it = spdy_headers_.find("x-original-url");
      it != spdy_headers_.end()) {
    x_original_url_ = it->second;
    HandleXOriginalUrl();
  }
}

void QuailServerBackend::ResourceFile::SetHostPathFromBase(
    absl::string_view base) {
  QUICHE_DCHECK(base[0] != '/') << base;
  size_t path_start = base.find_first_of('/');
  if (path_start == absl::string_view::npos) {
    host_ = std::string(base);
    path_ = "";
    return;
  }

  host_ = std::string(base.substr(0, path_start));
  size_t query_start = base.find_first_of(',');
  if (query_start > 0) {
    path_ = std::string(base.substr(path_start, query_start - 1));
  } else {
    path_ = std::string(base.substr(path_start));
  }
}

absl::string_view QuailServerBackend::ResourceFile::RemoveScheme(
    absl::string_view url) {
  if (absl::StartsWith(url, "https://")) {
    url.remove_prefix(8);
  } else if (absl::StartsWith(url, "http://")) {
    url.remove_prefix(7);
  }
  return url;
}

void QuailServerBackend::ResourceFile::HandleXOriginalUrl() {
  absl::string_view url(x_original_url_);
  SetHostPathFromBase(RemoveScheme(url));
}


//

QuailServerBackend::QuailServerBackend() {}

std::string QuailServerBackend::GetKey(absl::string_view host,
                                      absl::string_view path) const {
  std::string host_string = std::string(host);
  size_t port = host_string.find(':');
  if (port != std::string::npos)
    host_string = std::string(host_string.c_str(), port);
  return host_string + std::string(path);
}

const QuicBackendResponse* QuailServerBackend::GetResponse(
    absl::string_view host,
    absl::string_view path) const {
  QuicWriterMutexLock lock(&response_mutex_);

  auto it = responses_.find("local2/index.html");
  if (it == responses_.end()) {
    uint64_t ignored = 0;
    if (generate_bytes_response_) {
      if (absl::SimpleAtoi(absl::string_view(path.data() + 1, path.size() - 1),
                           &ignored)) {
        // The actual parsed length is ignored here and will be recomputed
        // by the caller.
        return generate_bytes_response_.get();
      }
    }
    QUIC_DVLOG(1) << "Get response for resource failed: host " << host
                  << " path " << path;
    if (default_response_) {
      return default_response_.get();
    }
    return nullptr;
  }
  return it->second.get();
}

bool QuailServerBackend::InitializeBackend(const std::string& cache_directory) {
  
  if (cache_directory.empty()) {
    QUIC_BUG(quic_bug_10932_1) << "cache_directory must not be empty.";
    return false;
  }
  QUIC_LOG(INFO)
      << "Attempting to initialize QuicMemoryCacheBackend from directory: "
      << cache_directory;
  std::vector<std::string> files;
  if (!quiche::EnumerateDirectoryRecursively(cache_directory, files)) {
    QUIC_BUG(QuicMemoryCacheBackend unreadable directory)
        << "Can't read QuicMemoryCacheBackend directory: " << cache_directory;
    return false;
  }
  for (const auto& filename : files) {
    std::unique_ptr<ResourceFile> resource_file(new ResourceFile(filename));

    // Tease apart filename into host and path.
    std::string base(resource_file->file_name());
    // Transform windows path separators to URL path separators.
    for (size_t i = 0; i < base.length(); ++i) {
      if (base[i] == '\\') {
        base[i] = '/';
      }
    }
    base.erase(0, cache_directory.length());
    if (base[0] == '/') {
      base.erase(0, 1);
    }

    resource_file->SetHostPathFromBase(base);
    resource_file->Read();

    AddResponse(resource_file->host(), resource_file->path(),
                resource_file->spdy_headers().Clone(), resource_file->body());
  }

  
  return true;
}

bool QuailServerBackend::IsBackendInitialized() const {
  return false;
};

using SpecialResponseType = QuicBackendResponse::SpecialResponseType;

void QuailServerBackend::AddResponseImpl(
    absl::string_view host, absl::string_view path,
    SpecialResponseType response_type, Http2HeaderBlock response_headers,
    absl::string_view response_body, Http2HeaderBlock response_trailers,
    const std::vector<spdy::Http2HeaderBlock>& early_hints) {
  QuicWriterMutexLock lock(&response_mutex_);

  QUICHE_DCHECK(!host.empty())
      << "Host must be populated, e.g. \"www.google.com\"";
  std::string key = GetKey(host, path);
  QUIC_LOG(INFO) << "key: " << key;
  if (responses_.contains(key)) {
    QUIC_BUG(quic_bug_10932_3)
        << "Response for '" << key << "' already exists!";
    return;
  }
  auto new_response = std::make_unique<QuicBackendResponse>();
  new_response->set_response_type(response_type);
  new_response->set_headers(std::move(response_headers));
  new_response->set_body(response_body);
  new_response->set_trailers(std::move(response_trailers));
  for (auto& headers : early_hints) {
    new_response->AddEarlyHints(headers);
  }
  QUIC_DVLOG(1) << "Add response with key " << key;
  responses_[key] = std::move(new_response);
}

void QuailServerBackend::AddResponse(absl::string_view host,
                                         absl::string_view path,
                                         Http2HeaderBlock response_headers,
                                         absl::string_view response_body) {
  AddResponseImpl(host, path, QuicBackendResponse::REGULAR_RESPONSE,
                  std::move(response_headers), response_body,
                  Http2HeaderBlock(), std::vector<spdy::Http2HeaderBlock>());
}

void QuailServerBackend::FetchResponseFromBackend(
    const Http2HeaderBlock& request_headers,
    const std::string& /*request_body*/,
    QuicSimpleServerBackend::RequestHandler* quic_stream) {
  QUIC_LOG(INFO) << "FetchResponseFromBackend";

  const QuicBackendResponse* quic_response = nullptr;
  // Find response in cache. If not found, send error response.
  auto authority = request_headers.find(":authority");
  auto path = request_headers.find(":path");
  if (authority != request_headers.end() && path != request_headers.end()) {
    // TODO(CC): response file
    quic_response = GetResponse(authority->second, path->second);
  }

  std::string request_url;
  if (authority != request_headers.end()) {
    request_url = std::string(authority->second);
  }
  if (path != request_headers.end()) {
    request_url += std::string(path->second);
  }
  QUIC_DVLOG(1)
      << "Fetching QUIC response from backend in-memory cache for url "
      << request_url;
  quic_stream->OnResponseBackendComplete(quic_response);
}

// The memory cache does not have a per-stream handler
void QuailServerBackend::CloseBackendResponseStream(
    QuicSimpleServerBackend::RequestHandler* /*quic_stream*/) {}

void QuailServerBackend::OnTransport(QuailTransport* t) {
  signal_transport_(t);
}

QuicSimpleServerBackend::WebTransportResponse
QuailServerBackend::ProcessWebTransportRequest(
    const spdy::Http2HeaderBlock& request_headers,
    WebTransportSession* session) {
  if (!SupportsWebTransport()) {
    return QuicSimpleServerBackend::ProcessWebTransportRequest(request_headers,
                                                               session);
  }

  auto path_it = request_headers.find(":path");
  if (path_it == request_headers.end()) {
    WebTransportResponse response;
    response.response_headers[":status"] = "400";
    return response;
  }
  absl::string_view path = path_it->second;

  GURL url(absl::StrCat("https://localhost", path));

  QUIC_LOG(INFO) << "ProcessWebTransportRequest path: " << url.path();
  url::Component query(0, static_cast<int>(url.query_piece().length()));
  url::Component key, value;
  while (url::ExtractQueryKeyValue(url.query_piece().data(), &query, &key,
                                   &value)) {
    auto s_key = url.query_piece().substr(key.begin, key.len);
    auto s_value = url.query_piece().substr(value.begin, value.len);
    std::string a(s_key);
    std::string b(s_value);

    QUIC_LOG(INFO) << "key " << a << " value " << b;
  }

  if (url.path() == "/echo") {
    QUIC_LOG(INFO) << "echo";

    auto converter = std::make_unique<quit::QuitConverter>(session);

    converter->signal_transport_.connect(
        [this](QuailTransport* t) { signal_transport_(t); });

    WebTransportResponse response;
    response.response_headers[":status"] = "200";
    response.visitor = std::move(converter);
    return response;
  }

  WebTransportResponse response;
  response.response_headers[":status"] = "404";
  return response;
}

}  // namespace quit
