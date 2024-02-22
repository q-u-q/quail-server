#ifndef SRC_QUIT_TOY_SERVER_H_
#define SRC_QUIT_TOY_SERVER_H_

#include "third_party/sigslot/sigslot.h"

#include "api/quail_transport.h"

namespace quit {
class QuitToyServer {
 public:
  int Start(std::string& cert, std::string& key);

  sigslot::signal<QuailTransport*> signal_transport_;
};
}  // namespace quit

#endif /* SRC_QUIT_TOY_SERVER_H_ */
