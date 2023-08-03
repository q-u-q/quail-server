#ifndef SRC_QUIT_PROOF_PROVIDERS_IMPL_H_
#define SRC_QUIT_PROOF_PROVIDERS_IMPL_H_


#include <memory>

#include "quiche/quic/core/crypto/proof_source.h"
#include "quiche/quic/core/crypto/proof_verifier.h"

namespace quit {


std::unique_ptr<quic::ProofSource> CreateProofSource(std::string &certificate_file,std::string &key_file);

}  // namespace quiche

#endif /* SRC_QUIT_PROOF_PROVIDERS_IMPL_H_ */
