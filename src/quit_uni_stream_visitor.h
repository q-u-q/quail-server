#ifndef SRC_QUIT_UNI_STREAM_VISITOR_H_
#define SRC_QUIT_UNI_STREAM_VISITOR_H_

#include "quiche/quic/core/web_transport_interface.h"

using namespace quic;

namespace quit {
class QuitUniStreamVisitor : public WebTransportStreamVisitor {
 public:
  QuitUniStreamVisitor(WebTransportStream* stream) : stream_(stream) {}

  void OnCanRead() override {
    WebTransportStream::ReadResult result = stream_->Read(&buffer_);
    QUIC_LOG(WARNING)
        << "Attempted reading on WebTransport unidirectional stream "
        << stream_->GetStreamId() 
        << ", bytes read: " << result.bytes_read
        << ", data: " << buffer_;
    buffer_ = "";
    if (result.fin) {
      QUIC_LOG(WARNING) << "Finished receiving data on a WebTransport stream "
                        << stream_->GetStreamId();
    }
  }

  void OnCanWrite() override { QUICHE_NOTREACHED(); }

  void OnResetStreamReceived(WebTransportStreamError /*error*/) override {}
  void OnStopSendingReceived(WebTransportStreamError /*error*/) override {}
  void OnWriteSideInDataRecvdState() override {}

 private:
  WebTransportStream* stream_;
  std::string buffer_;
};
}  // namespace quit

#endif /* SRC_QUIT_UNI_STREAM_VISITOR_H_ */
