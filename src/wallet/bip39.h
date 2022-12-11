#ifndef PIVX_WALLET_BIP39_H
#define PIVX_WALLET_BIP39_H

#include <string>
#include <vector>

size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);
std::string CreateRandomSeedPhrase();
std::vector<uint8_t> GenerateSeedFromMnemonic(const std::string& mnemonic, const std::string& passphrase = "");
bool ValidateSeedPhrase(const std::string& seedphrase);
void testStuff();

#endif // PIVX_WALLET_BIP39_H
