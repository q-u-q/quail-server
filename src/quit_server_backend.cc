
#include "quit_server_backend.h"

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

namespace quit {

QuitServerBackend::QuitServerBackend() {}

std::string QuitServerBackend::GetKey(absl::string_view host,
                                      absl::string_view path) const {
  std::string host_string = std::string(host);
  size_t port = host_string.find(':');
  if (port != std::string::npos)
    host_string = std::string(host_string.c_str(), port);
  return host_string + std::string(path);
}

const QuicBackendResponse* QuitServerBackend::GetResponse(
    absl::string_view host,
    absl::string_view path) const {
  QuicWriterMutexLock lock(&response_mutex_);

  auto it = responses_.find(GetKey(host, path));
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

bool QuitServerBackend::InitializeBackend(const std::string& cache_directory) {
  return true;
}

bool QuitServerBackend::IsBackendInitialized() const {
  return false;
};

void QuitServerBackend::FetchResponseFromBackend(
    const Http2HeaderBlock& request_headers,
    const std::string& /*request_body*/,
    QuicSimpleServerBackend::RequestHandler* quic_stream) {
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
void QuitServerBackend::CloseBackendResponseStream(
    QuicSimpleServerBackend::RequestHandler* /*quic_stream*/) {}

void QuitServerBackend::OnTransport(QuitTransport* t) {
  signal_transport_(t);
}

QuicSimpleServerBackend::WebTransportResponse
QuitServerBackend::ProcessWebTransportRequest(
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

    auto converter = std::make_unique<QuitConverter>(session);

    converter->signal_transport_.connect(
        [this](QuitTransport* t) { signal_transport_(t); });

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
