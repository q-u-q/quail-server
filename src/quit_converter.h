#ifndef SRC_QUIT_CONVERTER_H_
#define SRC_QUIT_CONVERTER_H_

#include "quiche/quic/core/web_transport_interface.h"

#include "quit_transport.h"
#include "third_party/sigslot/sigslot.h"

using namespace quic;

namespace quit {

class QuitConverter : public WebTransportVisitor {
 public:
  QuitConverter(WebTransportSession* session);
  void OnSessionReady() override;
  void OnSessionClosed(WebTransportSessionError /*error_code*/,
                       const std::string& /*error_message*/) override;

  void OnIncomingBidirectionalStreamAvailable() override;

  void OnIncomingUnidirectionalStreamAvailable() override;

  void OnDatagramReceived(absl::string_view datagram) override;

  void OnCanCreateNewOutgoingBidirectionalStream() override;

  void OnCanCreateNewOutgoingUnidirectionalStream() override;

  sigslot::signal<QuitTransport*> signal_transport_;


  WebTransportSession* session_;



};
}  // namespace quit

#endif /* SRC_QUIT_CONVERTER_H_ */
