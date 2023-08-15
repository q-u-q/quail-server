#ifndef SRC_QUIT_STREAM_VISITOR
#define SRC_QUIT_STREAM_VISITOR

#include "quiche/quic/core/web_transport_interface.h"

using namespace quic;

namespace quit {
class QuitStreamVisitor : public WebTransportStreamVisitor {
 public:
  QuitStreamVisitor(WebTransportStream* stream) : stream_(stream) {}

  void OnCanRead() override {
    WebTransportStream::ReadResult result = stream_->Read(&buffer_);
    QUIC_LOG(WARNING) << "Attempted reading on WebTransport bidirectional stream "
                  << stream_->GetStreamId()
                  << ", bytes read: " << result.bytes_read
                  << ", data: " << buffer_;
    if (result.fin) {
      send_fin_ = true;
    }
    OnCanWrite();
  }

  void OnCanWrite() override {
    QUIC_LOG(WARNING) << "OnCanWrite";
    
    if (stop_sending_received_) {
      return;
    }

    if (!buffer_.empty()) {
      absl::Status status = quiche::WriteIntoStream(*stream_, buffer_);
      QUIC_LOG(WARNING) << "Attempted writing on WebTransport bidirectional stream "
                    << stream_->GetStreamId() << ", success: " << status;
      if (!status.ok()) {
        return;
      }

      buffer_ = "";
    } 

    // if (send_fin_ && !fin_sent_) {
    //   absl::Status status = quiche::SendFinOnStream(*stream_);
    //   if (status.ok()) {
    //     fin_sent_ = true;
    //   }
    // }
  }

  void OnResetStreamReceived(WebTransportStreamError /*error*/) override {
    // Send FIN in response to a stream reset.  We want to test that we can
    // operate one side of the stream cleanly while the other is reset, thus
    // replying with a FIN rather than a RESET_STREAM is more appropriate here.
    send_fin_ = true;
    OnCanWrite();
  }
  void OnStopSendingReceived(WebTransportStreamError /*error*/) override {
    stop_sending_received_ = true;
  }
  void OnWriteSideInDataRecvdState() override {}

 protected:
  WebTransportStream* stream() { return stream_; }

 private:
  WebTransportStream* stream_;
  std::string buffer_;
  bool send_fin_ = false;
  bool fin_sent_ = false;
  bool stop_sending_received_ = false;
};
}  // namespace quit

#endif /* SRC_QUIT_STREAM_VISITOR */
