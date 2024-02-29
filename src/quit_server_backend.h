#ifndef SRC_QUIT_SERVER_BACKEND_H_
#define SRC_QUIT_SERVER_BACKEND_H_

#include <list>
#include <map>
#include <memory>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "quiche/quic/core/http/spdy_utils.h"
#include "quiche/quic/platform/api/quic_mutex.h"
#include "quiche/quic/tools/quic_backend_response.h"
#include "quiche/quic/tools/quic_simple_server_backend.h"
#include "quiche/quic/tools/quic_url.h"
#include "quiche/spdy/core/http2_header_block.h"
#include "quiche/spdy/core/spdy_framer.h"

#include "third_party/sigslot/sigslot.h"

#include "api/quail_transport.h"

using namespace quic;

namespace quit {

class QuitServerBackend : public QuicSimpleServerBackend {
 public:
  class ResourceFile {
   public:
    explicit ResourceFile(const std::string& file_name);
    ResourceFile(const ResourceFile&) = delete;
    ResourceFile& operator=(const ResourceFile&) = delete;
    virtual ~ResourceFile();

    void Read();

    // |base| is |file_name_| with |cache_directory| prefix stripped.
    void SetHostPathFromBase(absl::string_view base);

    const std::string& file_name() { return file_name_; }

    absl::string_view host() { return host_; }

    absl::string_view path() { return path_; }

    const spdy::Http2HeaderBlock& spdy_headers() { return spdy_headers_; }

    absl::string_view body() { return body_; }

    const std::vector<absl::string_view>& push_urls() { return push_urls_; }

   private:
    void HandleXOriginalUrl();
    absl::string_view RemoveScheme(absl::string_view url);

    std::string file_name_;
    std::string file_contents_;
    absl::string_view body_;
    spdy::Http2HeaderBlock spdy_headers_;
    absl::string_view x_original_url_;
    std::vector<absl::string_view> push_urls_;
    std::string host_;
    std::string path_;
  };

  QuitServerBackend();

  // Retrieve a response from this cache for a given host and path..
  // If no appropriate response exists, nullptr is returned.
  const QuicBackendResponse* GetResponse(absl::string_view host,
                                         absl::string_view path) const;

  // Implements the functions for interface QuicSimpleServerBackend
  // |cache_cirectory| can be generated using `wget -p --save-headers <url>`.
  bool InitializeBackend(const std::string& cache_directory) override;
  bool IsBackendInitialized() const override;
  void FetchResponseFromBackend(
      const spdy::Http2HeaderBlock& request_headers,
      const std::string& request_body,
      QuicSimpleServerBackend::RequestHandler* quic_stream) override;
  void CloseBackendResponseStream(
      QuicSimpleServerBackend::RequestHandler* quic_stream) override;
  WebTransportResponse ProcessWebTransportRequest(
      const spdy::Http2HeaderBlock& request_headers,
      WebTransportSession* session) override;
  bool SupportsWebTransport() override { return true; }

  void OnTransport(QuailTransport*);

  void AddResponse(absl::string_view host,
                   absl::string_view path,
                   spdy::Http2HeaderBlock response_headers,
                   absl::string_view response_body);

  sigslot::signal<QuailTransport*> signal_transport_;

 private:
  std::string GetKey(absl::string_view host, absl::string_view path) const;

  void AddResponseImpl(absl::string_view host,
                       absl::string_view path,
                       QuicBackendResponse::SpecialResponseType response_type,
                       spdy::Http2HeaderBlock response_headers,
                       absl::string_view response_body,
                       spdy::Http2HeaderBlock response_trailers,
                       const std::vector<spdy::Http2HeaderBlock>& early_hints);

  // Cached responses.
  absl::flat_hash_map<std::string, std::unique_ptr<QuicBackendResponse>>
      responses_ QUIC_GUARDED_BY(response_mutex_);

  // The default response for cache misses, if set.
  std::unique_ptr<QuicBackendResponse> default_response_
      QUIC_GUARDED_BY(response_mutex_);

  // The generate bytes response, if set.
  std::unique_ptr<QuicBackendResponse> generate_bytes_response_
      QUIC_GUARDED_BY(response_mutex_);

  mutable QuicMutex response_mutex_;
};

}  // namespace quit

#endif /* SRC_QUIT_SERVER_BACKEND_H_ */
