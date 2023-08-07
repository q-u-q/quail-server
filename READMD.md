# Data route
/root/chromium/src/net/tools/quic/quic_simple_server.cc
StartReading-> OnReadComplete

/root/chromium/src/net/third_party/quiche/src/quiche/quic/core/quic_dispatcher.cc
QuicDispatcher::ProcessPacket
QuicDispatcher::ProcessHeader
QuicDispatcher::TryExtractChloOrBufferEarlyPacket

/root/chromium/src/net/third_party/quiche/src/quiche/quic/core/tls_chlo_extractor.h
TlsChloExtractor::IngestPacket

/root/chromium/src/net/third_party/quiche/src/quiche/quic/core/quic_framer.cc
QuicFramer::ProcessPacket
QuicFramer::ProcessPacketInternal
ProcessIetfDataPacket
RemoveHeaderProtection
ProcessIetfFrameData

## Other

"tools/quic/quic_simple_server_bin.cc",

---

/root/chromium/src/net/tools/quic/quic_simple_server.cc
StartReading-> OnReadComplete

/root/chromium/src/net/third_party/quiche/src/quiche/quic/core/quic_dispatcher.cc
QuicDispatcher::ProcessPacket
QuicDispatcher::ProcessHeader
QuicDispatcher::TryExtractChloOrBufferEarlyPacket

/root/chromium/src/net/third_party/quiche/src/quiche/quic/core/tls_chlo_extractor.h
TlsChloExtractor::IngestPacket

/root/chromium/src/net/third_party/quiche/src/quiche/quic/core/quic_framer.cc
QuicFramer::ProcessPacket
QuicFramer::ProcessPacketInternal
ProcessIetfDataPacket
RemoveHeaderProtection
ProcessIetfFrameData

DecryptPayload