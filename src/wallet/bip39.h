#ifndef PIVX_WALLET_BIP39_H
#define PIVX_WALLET_BIP39_H

#include <string>
#include <vector>

size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);
std::string getCachedSeedphrase();
std::string EntropyToSeedPhrase(const std::vector<uint8_t>& entropy);
std::string CreateRandomSeedPhrase(bool wantToCache);
bool CheckValidityOfSeedPhrase(const std::string& seedphrase, bool wantToCache);
std::vector<uint8_t> GenerateSeedFromMnemonic(const std::string& mnemonic, const std::string& passphrase = "");
bool ValidateSeedPhrase(const std::string& seedphrase);

#endif // PIVX_WALLET_BIP39_H
