
#include "quit_converter.h"
#include <absl/strings/string_view.h>

#include "quiche/quic/platform/api/quic_logging.h"

#include "quit_stream_visitor.h"
#include "quit_transport.h"
#include "quit_uni_stream_visitor.h"
#include "quit_uni_stream_write_visitor.h"
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

void QuitConverter::OnIncomingBidirectionalStreamAvailable() {
  QUIC_LOG(WARNING) << "OnIncomingBidirectionalStreamAvailable";
  WebTransportStream* stream = session_->AcceptIncomingBidirectionalStream();
  if (stream == nullptr) {
    return;
  }
  QUIC_LOG(WARNING)
      << "EchoWebTransportSessionVisitor received a bidirectional stream "
      << stream->GetStreamId();
  stream->SetVisitor(std::make_unique<QuitStreamVisitor>(stream));
  stream->visitor()->OnCanRead();
};

void QuitConverter::OnIncomingUnidirectionalStreamAvailable() {
  QUIC_LOG(WARNING) << "OnIncomingUnidirectionalStreamAvailable";
  WebTransportStream* stream = session_->AcceptIncomingUnidirectionalStream();
  if (stream == nullptr) {
    return;
  }
  QUIC_LOG(WARNING)
      << "EchoWebTransportSessionVisitor received a unidirectional stream";
  stream->SetVisitor(std::make_unique<QuitUniStreamVisitor>(stream));
  stream->visitor()->OnCanRead();

  TrySendingUnidirectionalStreams();
};

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

void QuitConverter::TrySendingUnidirectionalStreams() {
  if (session_->CanOpenNextOutgoingUnidirectionalStream()) {
    QUIC_LOG(WARNING)
        << "EchoWebTransportServer echoed a unidirectional stream back";
    WebTransportStream* stream = session_->OpenOutgoingUnidirectionalStream();
    stream->SetVisitor(std::make_unique<QuitUniStreamWriteVisitor>(stream));
    stream->visitor()->OnCanWrite();
  }
}

}  // namespace quit