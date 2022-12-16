#include <boost/test/unit_test.hpp>
#include "wallet/bip39.h"

#include "test/test_pivx.h"

#include <cstdint>
#include <string>
#include <sys/types.h>
#include <vector>

std::vector<uint8_t> hex_str_to_uint8(std::string string) {

    size_t slength = string.length();// strlen(string);

    size_t dlength = slength / 2;

    uint8_t* data = (uint8_t*)malloc(dlength);

    memset(data, 0, dlength);

    size_t index = 0;
    while (index < slength) {
        char c = string[index];
        int value = 0;
        if (c >= '0' && c <= '9')
            value = (c - '0');
        else if (c >= 'A' && c <= 'F')
            value = (10 + (c - 'A'));
        else if (c >= 'a' && c <= 'f')
            value = (10 + (c - 'a'));

        data[(index / 2)] += value << (((index + 1) % 2) * 4);

        index++;
    }
    std::vector<uint8_t> output(data,data+dlength);
    return output;
}

struct TestVector1{
    public:
        std::vector<uint8_t> entropy;
        std::string seedphrase;
        std::vector<uint8_t> seed;
    TestVector1(const std::string & hex_entropy ,const std::string& seedphrase,const std::string & hex_seed){
        this->entropy = hex_str_to_uint8(hex_entropy);
        this->seedphrase=seedphrase;
        this->seed= hex_str_to_uint8(hex_seed);
    }
};

struct TestVector2{
    std::string seedphrase;
    bool isValid;
    TestVector2(const std::string& seedphrase,bool is_valid){
        this->isValid=is_valid;
        this->seedphrase=seedphrase;
    }
};

TestVector1 b39_test1("00000000000000000000000000000000", "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about","c55257c360c07c72029aebc1b53c05ed0362ada38ead3e3e9efa3708e53495531f09a6987599d18264c1e1c92f2cf141630c7a3c4ab7c81b2f001698e7463b04");
TestVector1 b39_test2("7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f", "legal winner thank year wave sausage worth useful legal winner thank yellow","2e8905819b8723fe2c1d161860e5ee1830318dbf49a83bd451cfb8440c28bd6fa457fe1296106559a3c80937a1c1069be3a3a5bd381ee6260e8d9739fce1f607");
TestVector1 b39_test3("0000000000000000000000000000000000000000000000000000000000000000", "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon art","bda85446c68413707090a52022edd26a1c9462295029f2e60cd7c4f2bbd3097170af7a4d73245cafa9c3cca8d561a7c3de6f5d4a10be8ed2a5e608d68f92fcc8");

TestVector2 b39_test1_2("legal winner thank year wave sausage worth useful legal winner thank yellow",true);
TestVector2 b39_test2_2("legal winner thank weak wave sausage worth useful legal winner thank yellow",false);
TestVector2 b39_test3_2("panda eyebrow bullet gorilla call smoke muffin taste mesh discover soft ostrich alcohol speed nation flash devote level hobby quick inner drive ghost inside",true);
static void RunTest(const TestVector1 &test) {

        //Test seedphrase
        BOOST_CHECK(EntropyToSeedPhrase(test.entropy)==test.seedphrase);

        // Test seed
        BOOST_CHECK(GenerateSeedFromMnemonic(test.seedphrase,"TREZOR") == test.seed);
}
static void RunTest2(const TestVector2 &test){
     //Test seedphrase
    BOOST_CHECK(CheckValidityOfSeedPhrase(test.seedphrase,false)==test.isValid);
}

BOOST_FIXTURE_TEST_SUITE(bip39_tests, TestingSetup)

BOOST_AUTO_TEST_CASE(bip39_test1) {
    RunTest(b39_test1);
}

BOOST_AUTO_TEST_CASE(bip39_test2) {
    RunTest(b39_test2);
}

BOOST_AUTO_TEST_CASE(bip39_test3) {
    RunTest(b39_test3);
}

BOOST_AUTO_TEST_CASE(bip39_test1_2) {
    RunTest2(b39_test1_2);
}

BOOST_AUTO_TEST_CASE(bip39_test2_2) {
    RunTest2(b39_test2_2);
}

BOOST_AUTO_TEST_CASE(bip39_test3_2) {
    RunTest2(b39_test3_2);
}

BOOST_AUTO_TEST_SUITE_END()