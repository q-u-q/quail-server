#include <unistd.h>
#include "quiche/quic/platform/api/quic_logging.h"
#include "quit_toy_server.h"
#include "quit_transport.h"

int main(int argc, char* argv[]) {
  quit::QuitToyServer server;
  server.signal_transport_.connect([](quit::QuitTransport* t){
    QUIC_LOG(INFO)<< "Transport";
    std::string data("fuck you");
    // sleep(1);
    // t->session_->SendOrQueueDatagram(data);
  });
  return server.Start();
}
