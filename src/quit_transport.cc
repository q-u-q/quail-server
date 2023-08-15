
#include "quit_transport.h"

namespace quit {
QuitTransport::QuitTransport(WebTransportSession* session)
    : session_(session) {}

void QuitTransport::SendDatagram(std::string& data) {
  session_->SendOrQueueDatagram(data);
}

}  // namespace quit