// Copyright (c) 2023 The PIVX Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef PIVX_WALLET_BIP39_H
#define PIVX_WALLET_BIP39_H

#include <string>
#include <vector>

enum BIP39_ERRORS {
    LANGUAGE_NOT_FOUND = -4,
    WRONG_CHECKSUM = -3,
    WRONG_ENTROPY = -2,
    BIP39_OK = -1,
};
size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);
std::string getCachedSeedphrase();
void resetCachedSeedphrase();
std::string EntropyToSeedPhrase(const std::vector<uint8_t>& entropy, const std::string& lang);
std::string CreateRandomSeedPhrase(bool wantToCache, const std::string& lang);
int CheckValidityOfSeedPhrase(const std::string& seedphrase, bool wantToCache);
std::vector<uint8_t> GenerateSeedFromMnemonic(const std::string& mnemonic, const std::string& passphrase = "");
bool ValidateSeedPhrase(const std::string& seedphrase);

#endif // PIVX_WALLET_BIP39_H
