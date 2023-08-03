#ifndef SRC_QUIT_DISPATCHER_H_
#define SRC_QUIT_DISPATCHER_H_

#include "absl/strings/string_view.h"
#include "quiche/quic/core/http/quic_server_session_base.h"
#include "quiche/quic/core/quic_dispatcher.h"
#include "quiche/quic/tools/quic_simple_server_backend.h"

using namespace quic;

namespace quit {

class QuitDispatcher : public QuicDispatcher {
 public:
  QuitDispatcher(
      const QuicConfig* config,
      const QuicCryptoServerConfig* crypto_config,
      QuicVersionManager* version_manager,
      std::unique_ptr<QuicConnectionHelperInterface> helper,
      std::unique_ptr<QuicCryptoServerStreamBase::Helper> session_helper,
      std::unique_ptr<QuicAlarmFactory> alarm_factory,
      QuicSimpleServerBackend* quic_simple_server_backend,
      uint8_t expected_server_connection_id_length,
      ConnectionIdGeneratorInterface& generator);

  ~QuitDispatcher() override;

  int GetRstErrorCount(QuicRstStreamErrorCode rst_error_code) const;

  void OnRstStreamReceived(const QuicRstStreamFrame& frame) override;

 protected:
  std::unique_ptr<QuicSession> CreateQuicSession(
      QuicConnectionId connection_id,
      const QuicSocketAddress& self_address,
      const QuicSocketAddress& peer_address,
      absl::string_view alpn,
      const ParsedQuicVersion& version,
      const ParsedClientHello& parsed_chlo) override;

  QuicSimpleServerBackend* server_backend() {
    return quic_simple_server_backend_;
  }

 private:
  QuicSimpleServerBackend* quic_simple_server_backend_;  // Unowned.

  // The map of the reset error code with its counter.
  std::map<QuicRstStreamErrorCode, int> rst_error_map_;
};

}  // namespace quit

#endif /* SRC_QUIT_DISPATCHER_H_ */
