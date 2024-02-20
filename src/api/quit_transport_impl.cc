
#include "api/quit_transport_impl.h"

namespace quit {

QuitTransportImpl::QuitTransportImpl(quic::WebTransportSession* session)
    : session_(session) {}

}  // namespace quit