#include "wallet/bip39.h"

#include "crypter.h"
#include "crypto/hmac_sha512.h"
#include "crypto/scrypt.h"
#include "crypto/sha256.h"
#include "random.h"
#include "script/standard.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <boost/algorithm/string.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

#include <csignal>

// Size in bits of a byte
constexpr int BYTE_SIZE = 8;
// Sizes in bits of the index of a word (Each index can be from 0 to 2048)
constexpr int WORD_SIZE = 11;
// Number of iterations
constexpr int ITERATIONS = 2048;
constexpr int SEED_LENGTH = 64;

std::vector<std::string> _words;

static std::vector<uint8_t> BitsToBytes(std::vector<bool>::const_iterator begin, std::vector<bool>::const_iterator end)
{
    std::vector<uint8_t> result;
    size_t size = end - begin;
    result.reserve(size / BYTE_SIZE);
    for (size_t i = 0; i < size / BYTE_SIZE; ++i) {
        result.push_back(0);
        for (int j = 0; j < BYTE_SIZE; ++j) {
            result[i] |= (begin[i * BYTE_SIZE + j] << (BYTE_SIZE - 1 - j));
        }
    }
    return result;
}

static std::vector<bool> BytesToBits(const std::vector<uint8_t>& bytes)
{
    std::vector<bool> result;
    result.reserve(bytes.size() * 8);
    for (auto&& b : bytes) {
        for (int i = BYTE_SIZE - 1; i >= 0; --i) {
            result.push_back(b & (1 << i));
        }
    }
    return result;
}


static std::vector<std::string>& GetWordList()
{
    if (_words.size() == 0) {
        std::ifstream infile("src/wallet/bip39_english.txt");
        std::string word;
        for (int i = 0; infile >> word; i++) {
            _words.push_back(std::move(word));
        }
    }
    return _words;
}

static int IsValidWord(const std::string& word)
{
    auto words = GetWordList();
    return std::find(words.begin(), words.end(), word) - words.begin();
}

bool CheckValidityOfSeedPhrase(const std::string& seedphrase)
{
    std::vector<std::string> words;
    boost::split(words, seedphrase, boost::is_any_of(" "));

    if (words.size() % 3 != 0) {
        return false;
    }

    std::vector<bool> word_bits;

    for (auto&& word : words) {
        int index = IsValidWord(word);
        if (index == 2048) {
            return false;
        }
        // Convert index to 11 bit word
        for (int i = WORD_SIZE - 1; i >= 0; i--) {
            word_bits.push_back(index & (1 << i));
        }
    }
    // Divider index between entropy and checksum
    int divider = (word_bits.size() / 33) * 32;
    auto entropy_bytes = BitsToBytes(word_bits.begin(), word_bits.begin() + divider);
    auto checksum_bytes = BitsToBytes(word_bits.begin() + divider, word_bits.end());
    if (entropy_bytes.size() < 16 || entropy_bytes.size() > 32 || entropy_bytes.size() % 4 != 0) {
        return false;
    }
    std::array<uint8_t, 32> sha256;
    CSHA256().Write(&entropy_bytes[0], entropy_bytes.size()).Finalize(&sha256[0]);
    for (size_t i = 0; i < checksum_bytes.size(); i++) {
        if (checksum_bytes[i] != sha256[i]) {
            return false;
        }
    }
    return true;
}


std::string EntropyToSeedPhrase(const std::vector<uint8_t>& entropy)
{
    std::vector<bool> entropy_bits = BytesToBits(entropy);
    std::string seedphrase;
    auto words = GetWordList();
    for (size_t i = 0; i < entropy.size() * BYTE_SIZE / WORD_SIZE; ++i) {
        if (i != 0) {
            seedphrase += " ";
        }
        int sum = 0;

        for (int j = 0; j < WORD_SIZE; ++j) {
            sum += pow(2, WORD_SIZE - 1 - j) * entropy_bits[i * WORD_SIZE + j];
        }
        seedphrase += words[sum];
    }
    return seedphrase;
}

std::vector<uint8_t> GenerateSeedFromMnemonic(const std::string& mnemonic, const std::string& passphrase)
{
    std::string salt = "mnemonic";
    std::vector<uint8_t> result;
    result.resize(SEED_LENGTH);
    salt += passphrase;
    PBKDF2_SHA512((const unsigned char*)mnemonic.c_str(), mnemonic.length(), (const unsigned char*)salt.c_str(), salt.length(), ITERATIONS, &result[0], SEED_LENGTH);
    return result;
}

// GENERATE SEEDPHRASE OF 24 WORDS
std::string CreateRandomSeedPhrase()
{
    std::vector<uint8_t> random_bytes;
    random_bytes.resize(32);
    std::array<uint8_t, 32> sha256;

    // Put 32 random bytes in buffer
    GetStrongRandBytes(&random_bytes[0], random_bytes.size());

    // Sha256 the bytes
    CSHA256().Write(&random_bytes[0], random_bytes.size()).Finalize(&sha256[0]);

    random_bytes.push_back(sha256[0]);
    std::string seedphrase = EntropyToSeedPhrase(random_bytes);
    return seedphrase;
}

// temporary test function
void testStuff()
{
    std::string seedphrase = CreateRandomSeedPhrase();

    auto seed = GenerateSeedFromMnemonic(seedphrase);
    std::cout << seedphrase << std::endl;
    bool result = CheckValidityOfSeedPhrase("great light crowd glad together verify supply horror guilt walk provide connect glory scare solution cost play talent radar initial minute essay purity uphold");
    std::cout << result << std::endl;
}
