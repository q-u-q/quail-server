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

  sigslot::signal<QuailTransport*> signal_transport_;

 private:
  std::string GetKey(absl::string_view host, absl::string_view path) const;

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
