#include "wallet/bip39.h"

#include "crypter.h"
#include "script/standard.h"
#include "crypto/scrypt.h"
#include "crypto/sha256.h"
#include "crypto/hmac_sha512.h"
#include "random.h"

#include <fstream>
#include <string> 
#include <iostream>
#include <math.h> 
#include <bitset>
#include <vector>

constexpr int BYTE_SIZE=8;
constexpr int WORD_SIZE=11;
constexpr int ITERATIONS=2048;
constexpr int NUMBER_OF_WORDS=2048;
constexpr int SEED_LENGTH=64;
bool generated=false;
std::string WORDS[NUMBER_OF_WORDS];

void LoadWords(){
  std::ifstream infile("src/wallet/bip39_english.txt");
  std::string word;
  int i=0;
  while (infile >> word)
  {
    WORDS[i]=word;
    i++;
  }
}

int IsValidWord(std::string word){
  //Load words if you haven't yet
  if(!generated){
    LoadWords();
    generated=true;
  }
  for(int i=0;i<NUMBER_OF_WORDS;i++){
    if(word.compare(WORDS[i])==0){
      return i;
    }
  }
  return 2048;
}

void ConvertToBinary(unsigned char* x,int x_len,int* buffer)
{
    int result[BYTE_SIZE*x_len];
    for(int i=0;i<x_len;i++){
      std::bitset<WORD_SIZE> b(x[i]);
      for(int j=0;j<BYTE_SIZE;j++){
        result[j+i*BYTE_SIZE]=b[BYTE_SIZE-1-j];
      }
    }
    std::memcpy(buffer, result, BYTE_SIZE*x_len*sizeof(*buffer));
}

void ConvertToWordBinary(int* x,int x_len,int* buffer)
{
    int result[WORD_SIZE*x_len];
    for(int i=0;i<x_len;i++){
      std::bitset<WORD_SIZE> b(x[i]);

      for(int j=0;j<WORD_SIZE;j++){
        result[j+i*WORD_SIZE]=b[WORD_SIZE-1-j];
      }
    }
    std::memcpy(buffer, result, WORD_SIZE*x_len*sizeof(*buffer));
}

bool CheckValidityOfSeedPhrase(std::string seedphrase){
  std::vector<std::string> splitphrase;
  std::string word="";
  for(char c: seedphrase){
    
    if(c==32){
      splitphrase.push_back(word);
      word="";
      continue;
    }
    word+=c;
    
  }
  splitphrase.push_back(word);

  if(splitphrase.size()!= 24 && splitphrase.size() != 21 && splitphrase.size()!= 18 && splitphrase.size()!= 15 && splitphrase.size()!= 12){
    return false;
  }
  int  int_words[splitphrase.size()];

  for(int i=0;i<splitphrase.size();i++){
    int val= IsValidWord(splitphrase[i]);
    if(val==2048){
      return false;
    }
    int_words[i]=val;
  }
  int binary[splitphrase.size()*WORD_SIZE];
  ConvertToWordBinary(int_words, splitphrase.size(), binary);
  int entropy[splitphrase.size()*WORD_SIZE*32/33];

  memcpy(entropy, binary,splitphrase.size()*11*32/33*sizeof(*entropy) );
  unsigned char entropy_bytes[splitphrase.size()*WORD_SIZE*32/(8*33)];
  unsigned char sum=0;
  int i=0;
  do{
    sum+=pow(2,7-i%8)*entropy[i];
    if(i%8==0){
      entropy_bytes[i/8-1]=sum;
      sum=0;
    }
    i++;
  }while(i<splitphrase.size()*WORD_SIZE*32/33+1);
  unsigned char checksum=0;
  for(int i=splitphrase.size()*WORD_SIZE-1;i>splitphrase.size()*WORD_SIZE*32/33-1;i--){
    checksum+=pow(2,(splitphrase.size()*WORD_SIZE-1)-i)*binary[i];
  }
  unsigned char sha_buffer[32];
  CSHA256().Write(entropy_bytes, 32).Finalize(sha_buffer);
  std::bitset<BYTE_SIZE> checksum_bits(sha_buffer[0]);
  unsigned char second_checksum=0;
  for(int i=0;i<splitphrase.size()*WORD_SIZE/33;i++){
    second_checksum+=pow(2,splitphrase.size()*WORD_SIZE/33-i-1)*checksum_bits[i];
  }
  return second_checksum==checksum;

}



std::string GenerateSeedPhrase(unsigned char* data,int data_len){
  std::string seedphrase="";
  int buffer[data_len*BYTE_SIZE];
  ConvertToBinary(data,data_len,buffer);

  int i=0;
  while(i<data_len*BYTE_SIZE/WORD_SIZE){
    int sum=0;
    if(i!=0){
      seedphrase+=" ";
    }
    for(int j=0;j<WORD_SIZE;j++){
      sum+=pow(2,WORD_SIZE-1-j)*buffer[i*WORD_SIZE+j];
    }
    seedphrase+=WORDS[sum];
    i++;
  }
  return seedphrase;
}

//GENERATE SEED FROM SEEDPHRASE
void GenerateSeedFromSeedPhrase(unsigned char* buffer,unsigned char* seedphrase, int seedphrase_len,std::string passphrase=""){
  std::string pre_salt="mnemonic";
  pre_salt+= passphrase;
  unsigned char salt[pre_salt.length()];
  std::copy( pre_salt.begin(), pre_salt.end(), salt );
  PBKDF2_SHA512(seedphrase,seedphrase_len, salt,pre_salt.length(), ITERATIONS ,buffer,SEED_LENGTH);
}

//GENERATE SEEDPHRASE OF 24 WORDS
std::string CreateRandomSeedPhrase()
{  
   //Load words if you haven't yet
    if(!generated){
        LoadWords();
        generated=true;
    }
    uint8_t buffer[32];
    uint8_t sha_buffer[32];
    uint8_t total_buffer[32+1];
    
    GetStrongRandBytes(buffer, 32);
    CSHA256().Write(buffer, 32).Finalize(sha_buffer);
    
    for(int i=0; i<32;i++){
        total_buffer[i]=buffer[i];
    }
    total_buffer[32]=sha_buffer[0];
    std::string seedphrase = GenerateSeedPhrase(total_buffer,33);
    return seedphrase;
}

//temporary test function
void testStuff(){
  std::string seedphrase = CreateRandomSeedPhrase();
  unsigned char seed[SEED_LENGTH];
  unsigned char to_crypt[seedphrase.length()];
  std::copy( seedphrase.begin(), seedphrase.end(), to_crypt );
  GenerateSeedFromSeedPhrase(seed,to_crypt,seedphrase.length(),"TREZOR");
  std::cout<<seedphrase<<std::endl;
  bool result = CheckValidityOfSeedPhrase("letter advice cage absurd amount doctor acoustic avoid letter advice cage absurd amount doctor acoustic avoid letter advice cage absurd amount doctor acoustic bless");
  std::cout<<result<<std::endl;

}