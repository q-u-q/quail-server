
#include "api/quail_transport.h"

#include "api/quail_transport_impl.h"


namespace quit {
QuailTransport::QuailTransport(QuailTransportImpl* impl)
    : impl_(impl) {}

void QuailTransport::SendDatagram(std::string& data) {
  impl_->session_->SendOrQueueDatagram(data);
}

void QuailTransport::Send(uint32_t stream_id, std::string& data) {

  auto stream = impl_->session_->GetStreamById(stream_id);

  if(!stream){
    QUIC_LOG(INFO) << "getstream:" << stream_id;
  }

  absl::Status write_status = quiche::WriteIntoStream(*stream, data);
  if (!write_status.ok()) {
    QUICHE_DLOG_IF(WARNING, !absl::IsUnavailable(write_status))
        << "Failed to write into stream: " << write_status;
    return;
  }
}

}  // namespace quit