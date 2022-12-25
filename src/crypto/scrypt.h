#ifndef SCRYPT_H
#define SCRYPT_H
#include <stdlib.h>
#include <stdint.h>
#include <string>

void scrypt(const char* pass, unsigned int pLen, const char* salt, unsigned int sLen, char *output, unsigned int N, unsigned int r, unsigned int p, unsigned int dkLen);
void PBKDF2_SHA512(const uint8_t* passwd, size_t passwdlen, const uint8_t* salt, size_t saltlen, uint64_t c, uint8_t* buf, size_t dkLen);

#endif
