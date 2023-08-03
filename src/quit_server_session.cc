
#include "quit_server_session.h"

#include <utility>

#include "absl/memory/memory.h"
#include "quiche/quic/core/http/quic_server_initiated_spdy_stream.h"
#include "quiche/quic/core/http/quic_spdy_session.h"
#include "quiche/quic/core/quic_connection.h"
#include "quiche/quic/core/quic_stream_priority.h"
#include "quiche/quic/core/quic_types.h"
#include "quiche/quic/core/quic_utils.h"
#include "quiche/quic/platform/api/quic_flags.h"
#include "quiche/quic/platform/api/quic_logging.h"
#include "quiche/quic/tools/quic_simple_server_stream.h"

namespace quit {

QuitServerSession::QuitServerSession(
    const QuicConfig& config,
    const ParsedQuicVersionVector& supported_versions,
    QuicConnection* connection,
    QuicSession::Visitor* visitor,
    QuicCryptoServerStreamBase::Helper* helper,
    const QuicCryptoServerConfig* crypto_config,
    QuicCompressedCertsCache* compressed_certs_cache,
    QuicSimpleServerBackend* quic_simple_server_backend)
    : QuicServerSessionBase(config,
                            supported_versions,
                            connection,
                            visitor,
                            helper,
                            crypto_config,
                            compressed_certs_cache),
      quic_simple_server_backend_(quic_simple_server_backend) {
  QUICHE_DCHECK(quic_simple_server_backend_);
}

QuitServerSession::~QuitServerSession() {
  DeleteConnection();
}

std::unique_ptr<QuicCryptoServerStreamBase>
QuitServerSession::CreateQuicCryptoServerStream(
    const QuicCryptoServerConfig* crypto_config,
    QuicCompressedCertsCache* compressed_certs_cache) {
  return CreateCryptoServerStream(crypto_config, compressed_certs_cache, this,
                                  stream_helper());
}

void QuitServerSession::OnStreamFrame(const QuicStreamFrame& frame) {
  if (!IsIncomingStream(frame.stream_id) && !WillNegotiateWebTransport()) {
    QUIC_LOG(WARNING) << "Client shouldn't send data on server push stream";
    connection()->CloseConnection(
        QUIC_INVALID_STREAM_ID, "Client sent data on server push stream",
        ConnectionCloseBehavior::SEND_CONNECTION_CLOSE_PACKET);
    return;
  }
  QuicSpdySession::OnStreamFrame(frame);
}

QuicSpdyStream* QuitServerSession::CreateIncomingStream(QuicStreamId id) {
  if (!ShouldCreateIncomingStream(id)) {
    return nullptr;
  }

  QUIC_LOG(INFO) << "CreateIncomingStream";
  QuicSpdyStream* stream = new QuicSimpleServerStream(
      id, this, BIDIRECTIONAL, quic_simple_server_backend_);
  ActivateStream(absl::WrapUnique(stream));
  return stream;
}

QuicSpdyStream* QuitServerSession::CreateIncomingStream(
    PendingStream* pending) {
  QuicSpdyStream* stream =
      new QuicSimpleServerStream(pending, this, quic_simple_server_backend_);
  ActivateStream(absl::WrapUnique(stream));
  return stream;
}

QuicSpdyStream* QuitServerSession::CreateOutgoingBidirectionalStream() {
  if (!WillNegotiateWebTransport()) {
    QUIC_BUG(QuicSimpleServerSession CreateOutgoingBidirectionalStream without
                 WebTransport support)
        << "QuicSimpleServerSession::CreateOutgoingBidirectionalStream called "
           "in a session without WebTransport support.";
    return nullptr;
  }
  if (!ShouldCreateOutgoingBidirectionalStream()) {
    return nullptr;
  }

  QuicServerInitiatedSpdyStream* stream = new QuicServerInitiatedSpdyStream(
      GetNextOutgoingBidirectionalStreamId(), this, BIDIRECTIONAL);
  ActivateStream(absl::WrapUnique(stream));
  return stream;
}

QuicSimpleServerStream*
QuitServerSession::CreateOutgoingUnidirectionalStream() {
  if (!ShouldCreateOutgoingUnidirectionalStream()) {
    return nullptr;
  }

  QuicSimpleServerStream* stream = new QuicSimpleServerStream(
      GetNextOutgoingUnidirectionalStreamId(), this, WRITE_UNIDIRECTIONAL,
      quic_simple_server_backend_);
  ActivateStream(absl::WrapUnique(stream));
  return stream;
}

}  // namespace quit