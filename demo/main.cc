#include <iostream>
#include <string>

// #include "quit_toy_server.h"

#include "api/quail_server.h"
#include "api/quail_transport.h"

int main(int argc, char* argv[]) {
  std::string cert = "/root/quail-server/certificates/certificate.pem";
  std::string key = "/root/quail-server/certificates/certificate.key";

  quail::QuailServer server;

  server.On("/echo", [](quit::QuailTransport* t) {
    std::cout << "Transport" << std::endl;
    // std::string data("foo");
    // sleep(1);
    // t->session_->SendOrQueueDatagram(data);
    t->signal_message_.connect([t](uint32_t stream_id, std::string message) {
      std::cout << "stream_id:" << stream_id << " message: " << message
                << std::endl;
      std::string response = "Dont give a shit";
      t->Send(stream_id, response);
    });
  });

  server.signal_transport_.connect([](quit::QuailTransport* t) {
    std::cout << "Transport" << std::endl;
    // std::string data("foo");
    // sleep(1);
    // t->session_->SendOrQueueDatagram(data);
    t->signal_message_.connect([t](uint32_t stream_id, std::string message) {
      std::cout << "stream_id:" << stream_id << " message: " << message
                << std::endl;
      std::string response = "Dont give a shit";
      t->Send(stream_id, response);
    });
  });
  return server.Start(cert, key);
}
