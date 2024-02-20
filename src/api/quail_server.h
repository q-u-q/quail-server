#ifndef SRC_API_QUAIL_SERVER_H_
#define SRC_API_QUAIL_SERVER_H_

#include "third_party/sigslot/sigslot.h"

#include "api/quit_transport.h"

namespace quail {
class QuailServer {
 public:
  int Start(std::string& cert, std::string& key);

  sigslot::signal<quit::QuitTransport*> signal_transport_;
};
}  // namespace quail

#endif /* SRC_API_QUAIL_SERVER_H_ */