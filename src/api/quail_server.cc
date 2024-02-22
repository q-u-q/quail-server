
#include "api/quail_server.h"


#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "quiche/common/platform/api/quiche_logging.h"
#include "quiche/common/quiche_random.h"
#include "quiche/quic/core/quic_server_id.h"
#include "quiche/quic/core/quic_versions.h"
#include "quiche/quic/platform/api/quic_socket_address.h"
#include "quiche/quic/tools/connect_server_backend.h"
#include "quiche/quic/tools/quic_memory_cache_backend.h"

#include "impl/quit_proof_providers_impl.h"
#include "quit_server.h"
#include "quit_server_backend.h"

namespace quail {

int QuailServer::Start(std::string& cert, std::string& key) {
  quic::ParsedQuicVersionVector supported_versions;
  supported_versions = AllSupportedVersions();

  for (const auto& version : supported_versions) {
    QuicEnableVersion(version);
  }
  auto proof_source = quit::CreateProofSource(cert, key);

  // backend
  auto backend = std::make_unique<quit::QuitServerBackend>();
  backend->signal_transport_.connect([this](quit::QuailTransport* t) {
    signal_transport_(t);
  });

  // server
  auto server = std::make_unique<quit::QuitServer>(std::move(proof_source),
                                             backend.get(), supported_versions);

  int port = 4433;
  if (!server->CreateUDPSocketAndListen(
          quic::QuicSocketAddress(quic::QuicIpAddress::Any6(), port))) {
    return 1;
  }

  server->HandleEventsForever();
  return 0;
}
}