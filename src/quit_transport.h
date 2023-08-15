#ifndef SRC_QUIT_TRANSPORT_H_
#define SRC_QUIT_TRANSPORT_H_

#include "quiche/quic/core/web_transport_interface.h"

using namespace quic;

namespace quit {

class QuitTransport {
 public:
  QuitTransport(WebTransportSession* session_);
  void SendDatagram(std::string& data);
  
  WebTransportSession* session_;
};

}  // namespace quit

#endif /* SRC_QUIT_TRANSPORT_H_ */
