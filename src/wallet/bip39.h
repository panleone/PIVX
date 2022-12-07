#include <string> 
#ifndef BIP39T
#define BIP39T 

void testStuff();
std::string CreateRandomSeedPhrase();
void GenerateSeedFromSeedPhrase(unsigned char* buffer,unsigned char* seedphrase, int seedphrase_len,std::string passphrase);
#endif //BIP39T