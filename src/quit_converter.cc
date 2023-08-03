
#include "quit_converter.h"

#include "quiche/quic/platform/api/quic_logging.h"

#include "quit_transport.h"
#include "quit_web_transport_visitors.h"

namespace quit {

QuitConverter::QuitConverter(WebTransportSession* session)
    : session_(session) {}

void QuitConverter::OnSessionReady(const spdy::Http2HeaderBlock& headers) {
  if (session_->CanOpenNextOutgoingBidirectionalStream()) {
    OnCanCreateNewOutgoingBidirectionalStream();
  }

  auto transport = new QuitTransport(session_);
  signal_transport_(transport);
};

void QuitConverter::OnSessionClosed(WebTransportSessionError /*error_code*/,
                                    const std::string& /*error_message*/){};

void QuitConverter::OnIncomingBidirectionalStreamAvailable(){};

void QuitConverter::OnIncomingUnidirectionalStreamAvailable(){};

void QuitConverter::OnDatagramReceived(absl::string_view datagram){};

void QuitConverter::OnCanCreateNewOutgoingBidirectionalStream() {
    QUIC_LOG(INFO) << "OnCanCreateNewOutgoingBidirectionalStream";
//   WebTransportStream* stream = session_->OpenOutgoingBidirectionalStream();
//   stream->SetVisitor(
//       std::make_unique<WebTransportBidirectionalEchoVisitor>(stream));
};

void QuitConverter::OnCanCreateNewOutgoingUnidirectionalStream(){};
}  // namespace quit