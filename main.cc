#include <unistd.h>
#include "quiche/quic/platform/api/quic_logging.h"
#include "quit_toy_server.h"
#include "quit_transport.h"

int main(int argc, char* argv[]) {
  std::string cert = "/root/quiche-node/certificates/certificate.pem";
  std::string key = "/root/quiche-node/certificates/certificate.key";

  quit::QuitToyServer server;
  server.signal_transport_.connect([](quit::QuitTransport* t) {
    QUIC_LOG(WARNING) << "Transport";
    std::string data("foo");
    // sleep(1);
    // t->session_->SendOrQueueDatagram(data);
  });
  return server.Start(cert, key);
}
