#ifndef SRC_API_QUIT_TRANSPORT_IMPL_H_
#define SRC_API_QUIT_TRANSPORT_IMPL_H_

#include "quiche/quic/core/web_transport_interface.h"

namespace quit {

class QuitTransportImpl {
 public:
  QuitTransportImpl(quic::WebTransportSession* session);

  quic::WebTransportSession* session_;
};
}  // namespace quit

#endif /* SRC_API_QUIT_TRANSPORT_IMPL_H_ */
