#include <unistd.h>
#include <string>
#include "quiche/quic/platform/api/quic_logging.h"
#include "quit_toy_server.h"

#include "api/quit_transport.h"

int main(int argc, char* argv[]) {
  std::string cert = "/root/quiche-node/certificates/certificate.pem";
  std::string key = "/root/quiche-node/certificates/certificate.key";

  quit::QuitToyServer server;
  server.signal_transport_.connect([](quit::QuitTransport* t) {
    // QUIC_LOG(WARNING) << "Transport";
    // std::string data("foo");
    // sleep(1);
    // t->session_->SendOrQueueDatagram(data);
    t->signal_message_.connect([t](uint32_t stream_id, std::string message) {
      QUIC_LOG(INFO) << "stream_id:" << stream_id << " message: " << message;
      std::string response = "Dont give a shit";
      t->Send(stream_id, response);
    });
  });
  return server.Start(cert, key);
}
