
#include "api/quit_transport_stream.h"
#include "quiche/quic/platform/api/quic_logging.h"

namespace quit {
QuitTransportStream::QuitTransportStream(WebTransportStream* stream)
    : stream_(stream) {
  stream_id_ = stream_->GetStreamId();
}

void QuitTransportStream::OnCanRead() {
  QUIC_LOG(INFO) << "OnCanRead";

  std::string buffer;
  WebTransportStream::ReadResult result = stream_->Read(&buffer);

  QUIC_LOG(WARNING) << "Attempted reading on WebTransport bidirectional stream "
                    << stream_->GetStreamId()
                    << ", bytes read: " << result.bytes_read
                    << ", data: " << buffer;
  // upup
  signal_message_(stream_id_, buffer);

  if (result.fin) {
    QUIC_LOG(INFO) << "send_fin_";
    //   send_fin_ = true;
  }
}

void QuitTransportStream::OnCanWrite() {
  QUIC_LOG(INFO) << "OnCanWrite";
}

void QuitTransportStream::OnResetStreamReceived(
    WebTransportStreamError /*error*/) {
  QUIC_LOG(INFO) << "OnResetStreamReceived";
}

void QuitTransportStream::OnStopSendingReceived(
    WebTransportStreamError /*error*/) {
  QUIC_LOG(INFO) << "OnStopSendingReceived";
}

void QuitTransportStream::OnWriteSideInDataRecvdState() {
  QUIC_LOG(INFO) << "OnWriteSideInDataRecvdState";
}

}  // namespace quit