
#include "api/quail_transport_impl.h"

namespace quail {

QuailTransportImpl::QuailTransportImpl(quic::WebTransportSession* session)
    : session_(session) {}

}  // namespace quit