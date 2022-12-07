#ifndef PIVX_WALLET_BIP39_H
#define PIVX_WALLET_BIP39_H

#include <string>
#include <vector>

std::string CreateRandomSeedPhrase();
std::vector<uint8_t> GenerateSeedFromMnemonic(const std::string& mnemonic, const std::string& passphrase = "");
bool ValidateSeedPhrase(const std::string& seedphrase);
void testStuff();

#endif // PIVX_WALLET_BIP39_H
