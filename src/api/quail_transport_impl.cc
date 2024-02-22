
#include "api/quail_transport_impl.h"

namespace quit {

QuailTransportImpl::QuailTransportImpl(quic::WebTransportSession* session)
    : session_(session) {}

}  // namespace quit