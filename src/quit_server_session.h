#ifndef SRC_QUIT_SERVER_SESSION_H_
#define SRC_QUIT_SERVER_SESSION_H_

#include <stdint.h>

#include <list>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "quiche/quic/core/http/quic_server_session_base.h"
#include "quiche/quic/core/http/quic_spdy_session.h"
#include "quiche/quic/core/quic_crypto_server_stream_base.h"
#include "quiche/quic/core/quic_packets.h"
#include "quiche/quic/tools/quic_backend_response.h"
#include "quiche/quic/tools/quic_simple_server_backend.h"
#include "quiche/quic/tools/quic_simple_server_stream.h"
#include "quiche/spdy/core/http2_header_block.h"

using namespace quic;

namespace quit {

class QuitServerSession : public QuicServerSessionBase {
 public:
  // Takes ownership of |connection|.
  QuitServerSession(const QuicConfig& config,
                    const ParsedQuicVersionVector& supported_versions,
                    QuicConnection* connection,
                    QuicSession::Visitor* visitor,
                    QuicCryptoServerStreamBase::Helper* helper,
                    const QuicCryptoServerConfig* crypto_config,
                    QuicCompressedCertsCache* compressed_certs_cache,
                    QuicSimpleServerBackend* quic_simple_server_backend);
  QuitServerSession(const QuitServerSession&) = delete;
  QuitServerSession& operator=(const QuitServerSession&) = delete;

  ~QuitServerSession() override;

  // Override base class to detact client sending data on server push stream.
  void OnStreamFrame(const QuicStreamFrame& frame) override;

 protected:
  // QuicSession methods:
  QuicSpdyStream* CreateIncomingStream(QuicStreamId id) override;
  QuicSpdyStream* CreateIncomingStream(PendingStream* pending) override;
  QuicSpdyStream* CreateOutgoingBidirectionalStream() override;
  QuicSimpleServerStream* CreateOutgoingUnidirectionalStream() override;

  // QuicServerSessionBaseMethod:
  std::unique_ptr<QuicCryptoServerStreamBase> CreateQuicCryptoServerStream(
      const QuicCryptoServerConfig* crypto_config,
      QuicCompressedCertsCache* compressed_certs_cache) override;

  QuicSimpleServerBackend* server_backend() {
    return quic_simple_server_backend_;
  }

  WebTransportHttp3VersionSet LocallySupportedWebTransportVersions()
      const override {
    return quic_simple_server_backend_->SupportsWebTransport()
               ? kDefaultSupportedWebTransportVersions
               : WebTransportHttp3VersionSet();
  }
  HttpDatagramSupport LocalHttpDatagramSupport() override {
    if (ShouldNegotiateWebTransport()) {
      return HttpDatagramSupport::kRfcAndDraft04;
    }
    return QuicServerSessionBase::LocalHttpDatagramSupport();
  }

 private:
  friend class test::QuicSimpleServerSessionPeer;

  QuicSimpleServerBackend* quic_simple_server_backend_;  // Not owned.
};

}  // namespace quit

#endif /* SRC_QUIT_SERVER_SESSION_H_ */
