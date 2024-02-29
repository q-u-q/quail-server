
#include "api/quail_server.h"

#include "impl/quail_server_impl.h"

namespace quail {

QuailServer::QuailServer() : impl_(new QuailServerImpl()) {}

void QuailServer::On(const std::string& path,
                     std::function<void(quit::QuailTransport*)> callback) {
  impl_->On(path, callback);
}

int QuailServer::Start(std::string& cert, std::string& key) {
  return impl_->Start(cert, key);
}
}  // namespace quail