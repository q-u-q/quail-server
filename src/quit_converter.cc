
#include "quit_converter.h"
#include <absl/strings/string_view.h>

#include "quiche/quic/platform/api/quic_logging.h"

#include "quit_stream_visitor.h"
#include "quit_transport.h"
#include "quit_web_transport_visitors.h"

namespace quit {

QuitConverter::QuitConverter(WebTransportSession* session)
    : session_(session) {}

void QuitConverter::OnSessionReady() {
  QUIC_LOG(WARNING) << "OnSessionReady";
  if (session_->CanOpenNextOutgoingBidirectionalStream()) {
    OnCanCreateNewOutgoingBidirectionalStream();
  }

  auto transport = new QuitTransport(session_);
  signal_transport_(transport);
};

void QuitConverter::OnSessionClosed(WebTransportSessionError /*error_code*/,
                                    const std::string& /*error_message*/) {
  QUIC_LOG(WARNING) << "OnSessionClosed";
};

void QuitConverter::OnIncomingBidirectionalStreamAvailable(){
  QUIC_LOG(WARNING) << "OnIncomingBidirectionalStreamAvailable";
     while (true) {
      WebTransportStream* stream =
          session_->AcceptIncomingBidirectionalStream();
      if (stream == nullptr) {
        return;
      }
      QUIC_LOG(WARNING)
          << "EchoWebTransportSessionVisitor received a bidirectional stream "
          << stream->GetStreamId();
      stream->SetVisitor(
          std::make_unique<QuitStreamVisitor>(stream));
      stream->visitor()->OnCanRead();
    }
};

void QuitConverter::OnIncomingUnidirectionalStreamAvailable(){};

void QuitConverter::OnDatagramReceived(absl::string_view datagram) {
  QUIC_LOG(WARNING) << "OnDatagramReceived " << datagram;
  session_->SendOrQueueDatagram(datagram);
};

void QuitConverter::OnCanCreateNewOutgoingBidirectionalStream() {
  QUIC_LOG(INFO) << "OnCanCreateNewOutgoingBidirectionalStream";
  //   WebTransportStream* stream = session_->OpenOutgoingBidirectionalStream();
  //   stream->SetVisitor(
  //       std::make_unique<WebTransportBidirectionalEchoVisitor>(stream));
};

void QuitConverter::OnCanCreateNewOutgoingUnidirectionalStream(){};
}  // namespace quit