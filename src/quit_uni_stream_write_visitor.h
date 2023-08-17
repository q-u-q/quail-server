#ifndef SRC_QUIT_UNI_STREAM_WRITE_VISITOR_H_
#define SRC_QUIT_UNI_STREAM_WRITE_VISITOR_H_



#include "quiche/quic/core/web_transport_interface.h"

using namespace quic;

namespace quit {
class QuitUniStreamWriteVisitor : public WebTransportStreamVisitor {
 public:
  QuitUniStreamWriteVisitor(WebTransportStream* stream) : stream_(stream) {}

  void OnCanRead() override { QUICHE_NOTREACHED(); }
  void OnCanWrite() override {
    std::string data_ = "foo you";
    absl::Status write_status = quiche::WriteIntoStream(*stream_, data_);
    if (!write_status.ok()) {
      QUICHE_DLOG_IF(WARNING, !absl::IsUnavailable(write_status))
          << "Failed to write into stream: " << write_status;
      return;
    }
    // absl::Status fin_status = quiche::SendFinOnStream(*stream_);
    // QUICHE_DVLOG(1)
    //     << "WebTransportUnidirectionalEchoWriteVisitor finished sending data.";
    // QUICHE_DCHECK(fin_status.ok());
  }

  void OnResetStreamReceived(WebTransportStreamError /*error*/) override {}
  void OnStopSendingReceived(WebTransportStreamError /*error*/) override {}
  void OnWriteSideInDataRecvdState() override {}

 private:
  WebTransportStream* stream_;
  std::string buffer_;
};
}  // namespace quit

#endif /* SRC_QUIT_UNI_STREAM_WRITE_VISITOR_H_ */
