#ifndef SRC_QUAIL_TRANSPORT_H_
#define SRC_QUAIL_TRANSPORT_H_

#include "third_party/sigslot/sigslot.h"

namespace quail {

class QuailTransportImpl;
class QuailTransport {
 public:
  QuailTransport(QuailTransportImpl* impl);
  void SendDatagram(std::string& data);

  void Send(uint32_t stream_id, std::string& data);

  sigslot::signal<uint32_t, std::string> signal_message_;

  QuailTransportImpl* impl_;
};

}  // namespace quit

#endif /* SRC_QUAIL_TRANSPORT_H_ */
