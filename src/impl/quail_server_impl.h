#ifndef SRC_IMPL_QUAIL_SERVER_IMPL_H_
#define SRC_IMPL_QUAIL_SERVER_IMPL_H_

#include <functional>
#include <memory>

#include "third_party/sigslot/sigslot.h"

#include "quit_server_backend.h"

#include "api/quail_transport.h"


namespace quail {

class QuailServerImpl {
 public:
  QuailServerImpl();

  int Start(std::string& cert, std::string& key);
  void On(const std::string& path,
          std::function<void(quit::QuailTransport*)> callback);

  std::unique_ptr<quit::QuitServerBackend> backend_;

  sigslot::signal<quit::QuailTransport*> signal_transport_;
};

}  // namespace quail

#endif /* SRC_IMPL_QUAIL_SERVER_IMPL_H_ */
