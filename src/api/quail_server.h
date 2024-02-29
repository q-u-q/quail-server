#ifndef SRC_API_QUAIL_SERVER_H_
#define SRC_API_QUAIL_SERVER_H_

#include <functional>

#include "third_party/sigslot/sigslot.h"

#include "api/quail_transport.h"

namespace quail {
class QuailServer {
 public:
  QuailServer();
  int Start(std::string& cert, std::string& key);
  void On(const std::string& path,
          std::function<void(quit::QuailTransport*)> callback);

  sigslot::signal<quit::QuailTransport*> signal_transport_;
};
}  // namespace quail

#endif /* SRC_API_QUAIL_SERVER_H_ */
