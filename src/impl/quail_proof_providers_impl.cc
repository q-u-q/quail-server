// Copyright 2022 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impl/quail_proof_providers_impl.h"

#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "quiche/quic/core/crypto/certificate_view.h"
#include "quiche/quic/core/crypto/proof_source.h"
#include "quiche/quic/core/crypto/proof_source_x509.h"
#include "quiche/quic/core/crypto/proof_verifier.h"


namespace quail {

std::unique_ptr<quic::ProofSource> CreateProofSource(std::string &certificate_file,std::string &key_file){
  
  std::ifstream cert_stream(certificate_file, std::ios::binary);
  std::vector<std::string> certs =
      quic::CertificateView::LoadPemFromStream(&cert_stream);
  if (certs.empty()) {
    // TODO(b/275440369): switch to QUICHE_LOG(FATAL) when available.
    std::cerr << "Failed to load certificate chain from --certificate_file="
              << certificate_file << std::endl;
    exit(1);
  }

  std::ifstream key_stream(key_file, std::ios::binary);
  std::unique_ptr<quic::CertificatePrivateKey> private_key =
      quic::CertificatePrivateKey::LoadPemFromStream(&key_stream);
  if (private_key == nullptr) {
    // TODO(b/275440369): switch to QUICHE_LOG(FATAL) when available.
    std::cerr << "Failed to load private key from --key_file=" << key_file
              << std::endl;
    exit(1);
  }

  quiche::QuicheReferenceCountedPointer<quic::ProofSource::Chain> chain(
      new quic::ProofSource::Chain({certs}));
  return quic::ProofSourceX509::Create(chain, std::move(*private_key));
}

}  // namespace quiche
