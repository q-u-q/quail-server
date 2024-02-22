
#include "quit_converter.h"
#include <absl/strings/string_view.h>

#include "quiche/quic/core/web_transport_interface.h"
#include "quiche/quic/platform/api/quic_logging.h"

#include "quit_stream_visitor.h"
#include "quit_uni_stream_visitor.h"
#include "quit_uni_stream_write_visitor.h"
#include "quit_web_transport_visitors.h"

#include "api/quit_transport_stream.h"
#include "api/quail_transport_impl.h"

namespace quit {

QuitConverter::QuitConverter(WebTransportSession* session)
    : session_(session) {}

void QuitConverter::OnSessionReady() {
  QUIC_LOG(WARNING) << "OnSessionReady";
  if (session_->CanOpenNextOutgoingBidirectionalStream()) {
    OnCanCreateNewOutgoingBidirectionalStream();
  }

  auto impl = new QuailTransportImpl(session_);
  transport_ = new QuailTransport(impl);
  signal_transport_(transport_);
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
  // stream->SetVisitor(std::make_unique<QuitStreamVisitor>(stream));
  //thread
  auto transport_stream_visitor = std::make_unique<QuitTransportStream>(stream);
  transport_stream_visitor->signal_message_.connect([this](uint32_t stream_id, std::string message) {
    this->transport_->signal_message_(stream_id, message);
  });
  stream->SetVisitor(std::move(transport_stream_visitor));
  // stream->visitor()->OnCanRead();
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