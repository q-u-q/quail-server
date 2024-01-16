#ifndef SRC_QUIT_TRANSPORT_H_
#define SRC_QUIT_TRANSPORT_H_

#include "quiche/quic/core/web_transport_interface.h"

#include "third_party/sigslot/sigslot.h"

using namespace quic;

namespace quit {

class QuitTransport {
 public:
  QuitTransport(WebTransportSession* session_);
  void SendDatagram(std::string& data);

  void Send(uint32_t stream_id, std::string& data);

  sigslot::signal<uint32_t, std::string> signal_message_;

  WebTransportSession* session_;
};

}  // namespace quit

#endif /* SRC_QUIT_TRANSPORT_H_ */
