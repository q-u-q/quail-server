#ifndef SRC_QUIT_TRANSPORT_H_
#define SRC_QUIT_TRANSPORT_H_

#include "third_party/sigslot/sigslot.h"

namespace quit {

class QuitTransportImpl;
class QuitTransport {
 public:
  QuitTransport(QuitTransportImpl* impl);
  void SendDatagram(std::string& data);

  void Send(uint32_t stream_id, std::string& data);

  sigslot::signal<uint32_t, std::string> signal_message_;

  QuitTransportImpl* impl_;
};

}  // namespace quit

#endif /* SRC_QUIT_TRANSPORT_H_ */
