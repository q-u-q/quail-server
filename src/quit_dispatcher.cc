
#include "quit_dispatcher.h"

#include "absl/strings/string_view.h"

#include "quit_server_session.h"

namespace quit {

QuitDispatcher::QuitDispatcher(
    const QuicConfig* config, const QuicCryptoServerConfig* crypto_config,
    QuicVersionManager* version_manager,
    std::unique_ptr<QuicConnectionHelperInterface> helper,
    std::unique_ptr<QuicCryptoServerStreamBase::Helper> session_helper,
    std::unique_ptr<QuicAlarmFactory> alarm_factory,
    QuicSimpleServerBackend* quic_simple_server_backend,
    uint8_t expected_server_connection_id_length,
    ConnectionIdGeneratorInterface& generator)
    : QuicDispatcher(config, crypto_config, version_manager, std::move(helper),
                     std::move(session_helper), std::move(alarm_factory),
                     expected_server_connection_id_length, generator),
      quic_simple_server_backend_(quic_simple_server_backend) {}

QuitDispatcher::~QuitDispatcher() = default;

int QuitDispatcher::GetRstErrorCount(
    QuicRstStreamErrorCode error_code) const {
  auto it = rst_error_map_.find(error_code);
  if (it == rst_error_map_.end()) {
    return 0;
  }
  return it->second;
}

void QuitDispatcher::OnRstStreamReceived(
    const QuicRstStreamFrame& frame) {
  auto it = rst_error_map_.find(frame.error_code);
  if (it == rst_error_map_.end()) {
    rst_error_map_.insert(std::make_pair(frame.error_code, 1));
  } else {
    it->second++;
  }
}

std::unique_ptr<QuicSession> QuitDispatcher::CreateQuicSession(
    QuicConnectionId connection_id, const QuicSocketAddress& self_address,
    const QuicSocketAddress& peer_address, absl::string_view /*alpn*/,
    const ParsedQuicVersion& version,
    const ParsedClientHello& /*parsed_chlo*/) {
  // The QuicServerSessionBase takes ownership of |connection| below.
  QuicConnection* connection = new QuicConnection(
      connection_id, self_address, peer_address, helper(), alarm_factory(),
      writer(),
      /* owns_writer= */ false, Perspective::IS_SERVER,
      ParsedQuicVersionVector{version}, connection_id_generator());

  auto session = std::make_unique<QuitServerSession>(
      config(), GetSupportedVersions(), connection, this, session_helper(),
      crypto_config(), compressed_certs_cache(), quic_simple_server_backend_);
  session->Initialize();
  return session;
}


}