// Copyright (c) 2023 The PIVX Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "wallet/bip39.h"

#include "crypto/scrypt.h"
#include "crypto/sha256.h"
#include "random.h"
#include "wallet/bip39_word_lists.json.h"

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

// Size in bits of a byte
constexpr int BYTE_SIZE = 8;
// Sizes in bits of the index of a word (Each index can be from 0 to 2048)
constexpr int WORD_SIZE = 11;
// Number of iterations
constexpr int ITERATIONS = 2048;
constexpr int SEED_LENGTH = 64;

std::vector<std::string> _words;

std::string cached_seedphrase = "";
std::vector<std::string> langs = {"en", "it"};
std::string getCachedSeedphrase()
{
    return cached_seedphrase;
}
void resetCachedSeedphrase()
{
    cached_seedphrase = "";
}
// split a string into words
void loadWords(const std::string& lang)
{
    split(loadWordList(lang), _words, ' ');
}

size_t split(const std::string& txt, std::vector<std::string>& strs, char ch)
{
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

    return strs.size();
}

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


static int IsValidWord(const std::string& word, const std::string& lang)
{
    loadWords(lang);
    return std::find(_words.begin(), _words.end(), word) - _words.begin();
}


int CheckValidityOfSeedPhrase(const std::string& seedphrase, bool wantToCache)
{
    std::vector<std::string> words;
    boost::split(words, seedphrase, boost::is_any_of(" "));

    if (words.size() % 3 != 0) {
        return BIP39_ERRORS::WRONG_ENTROPY;
    }

    std::string lang;
    bool lang_found = false;
    for (std::string l : langs) {
        if (IsValidWord(words[0], l) != 2048) {
            lang = l;
            lang_found = true;
            break;
        }
    }
    if (!lang_found) {
        return BIP39_ERRORS::LANGUAGE_NOT_FOUND;
    }
    std::vector<bool> word_bits;

    for (size_t i = 0; i < words.size(); i++) {
        const std::string& word = words[i];
        int index = IsValidWord(word, lang);
        if (index == 2048) {
            return i; // index of the wrong word
        }
        // Convert index to 11 bit word
        for (int i = WORD_SIZE - 1; i >= 0; i--) {
            word_bits.push_back(index & (1 << i));
        }
    }
    // Divider index between entropy and checksum
    int divider = (word_bits.size() / 33) * 32;
    auto entropy_bytes = BitsToBytes(word_bits.begin(), word_bits.begin() + divider);
    std::vector<bool> checksum_bits(word_bits.begin() + divider, word_bits.end());

    if (entropy_bytes.size() < 16 || entropy_bytes.size() > 32 || entropy_bytes.size() % 4 != 0) {
        return BIP39_ERRORS::WRONG_ENTROPY;
    }
    std::array<uint8_t, 32> sha256;
    CSHA256().Write(&entropy_bytes[0], entropy_bytes.size()).Finalize(&sha256[0]);
    std::vector<bool> sha256_bits;
    // we only need to convert in bits the first byte
    for (int i = 7; i > -1; i--) {
        sha256_bits.push_back(((sha256[0] >> i) & 0x01));
    }
    for (size_t i = 0; i < checksum_bits.size(); i++) {
        if (checksum_bits[i] != sha256_bits[i]) {
            return BIP39_ERRORS::WRONG_CHECKSUM;
        }
    }
    if (wantToCache) {
        cached_seedphrase = seedphrase;
    }
    return BIP39_ERRORS::BIP39_OK;
}


std::string EntropyToSeedPhrase(const std::vector<uint8_t>& entropy, const std::string& lang)
{
    std::vector<uint8_t> data_vector(entropy.begin(), entropy.end());
    std::array<uint8_t, 32> sha256;
    // Sha256 the bytes
    CSHA256().Write(&entropy[0], entropy.size()).Finalize(&sha256[0]);
    data_vector.push_back(sha256[0]);
    std::vector<bool> entropy_bits = BytesToBits(data_vector);
    std::string seedphrase;

    loadWords(lang);
    for (size_t i = 0; i < data_vector.size() * BYTE_SIZE / WORD_SIZE; ++i) {
        if (i != 0) {
            seedphrase += " ";
        }
        int sum = 0;

        for (int j = 0; j < WORD_SIZE; ++j) {
            sum += pow(2, WORD_SIZE - 1 - j) * entropy_bits[i * WORD_SIZE + j];
        }
        seedphrase += _words[sum];
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
std::string CreateRandomSeedPhrase(bool wantToCache, const std::string& lang)
{
    std::vector<uint8_t> random_bytes;
    random_bytes.resize(32);

    // Put 32 random bytes in buffer
    GetStrongRandBytes(&random_bytes[0], random_bytes.size());
    std::string seedphrase = EntropyToSeedPhrase(random_bytes, lang);
    if (wantToCache) {
        cached_seedphrase = seedphrase;
    }
    return seedphrase;
}
