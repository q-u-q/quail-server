#ifndef SRC_QUIT_TRANSPORT_STREAM_H_
#define SRC_QUIT_TRANSPORT_STREAM_H_

#include "quiche/quic/core/web_transport_interface.h"

#include "third_party/sigslot/sigslot.h"

using namespace quic;

namespace quit {

class QuitTransportStream : public WebTransportStreamVisitor {
 public:
  QuitTransportStream(WebTransportStream* stream);

  void OnCanRead() override;

  void OnCanWrite() override;

  void OnResetStreamReceived(WebTransportStreamError /*error*/) override;
  void OnStopSendingReceived(WebTransportStreamError /*error*/) override;
  void OnWriteSideInDataRecvdState() override;

  void Send();


  WebTransportStream* stream_;
  // std::string buffer_;
  uint32_t stream_id_;
  sigslot::signal<uint32_t,std::string> signal_message_;


};
}  // namespace quit

#endif /* SRC_QUIT_TRANSPORT_STREAM_H_ */
