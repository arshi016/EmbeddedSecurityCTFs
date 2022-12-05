#include <stdint.h>
#include <stdio.h>

void tea_encrypt(uint64_t* v, uint64_t key, int whichMsg);
void tea_decrypt(uint64_t* v, uint64_t key);