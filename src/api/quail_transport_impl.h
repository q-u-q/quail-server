#ifndef SRC_API_QUAIL_TRANSPORT_IMPL_H_
#define SRC_API_QUAIL_TRANSPORT_IMPL_H_

#include "quiche/quic/core/web_transport_interface.h"

namespace quail {

class QuailTransportImpl {
 public:
  QuailTransportImpl(quic::WebTransportSession* session);

  quic::WebTransportSession* session_;
};
}  // namespace quit

#endif /* SRC_API_QUAIL_TRANSPORT_IMPL_H_ */
