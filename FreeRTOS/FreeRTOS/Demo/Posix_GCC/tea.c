#include "tea.h"

// void tea_encrypt(uint32_t* v, uint32_t key) {
//     printf("\nOriginal Values: ");
//     printf("[ %X %X ]", v[0], v[1]);
//     uint32_t v0=v[0], v1=v[1], sum=0, i;             // set up
//     uint32_t delta=0x9e3779b9;                       // a key schedule constant
//     for (i=0; i < 32; i++) {                         // basic cycle start
//         sum += delta;
//         v0 += ((v1<<4) + key) ^ (v1 + sum) ^ ((v1>>5) + key);
//         v1 += ((v0<<4) + key) ^ (v0 + sum) ^ ((v0>>5) + key);  
//     }                                                // end cycle 
//     v[0]=v0; v[1]=v1;
//     printf("\n Encrypted:       ");
//     printf("[ %X %X ]", v[0], v[1]);
// }

#define SUM     0x2468af130eca8640
#define DELTA   0x123457898765432


void tea_encrypt(uint64_t* v, uint64_t key, int whichMsg) {
    // printf("\n\nOriginal Values:");
    // printf("[ %X %X ]", v[0], v[1]);
    // printf("\nkey = %lx", key);
    uint64_t v0=v[0], v1=v[1], sum=0, i;             // set up
    uint64_t delta=0x123457898765432;                // a key schedule constant
    for (i=0; i < 32; i++) {                         // basic cycle start
        sum += delta;
        v0 += ((v1<<4) + key) ^ (v1 + sum) ^ ((v1>>5) + key);
        v1 += ((v0<<4) + key) ^ (v0 + sum) ^ ((v0>>5) + key);
    }                                                // end cycle
    v[0]=v0; v[1]=v1;
    if(whichMsg == 0) {
        printf("\nEncrypted key for Planet White:       ");
        printf("[ %lX %lX ]\nA fresh short term key will be exchaged again after 1 min....", v[0], v[1]);
    } else if (whichMsg == 1) {
        printf("\nEncrypted message for Planet White:       ");
        printf("[ %lX %lX ]\nAfter 1 second....", v[0], v[1]);
    }
    tea_decrypt(v, key);
}

void tea_decrypt (uint64_t* v, uint64_t key) {
    uint64_t v0=v[0], v1=v[1], sum = SUM, i;  // set up 
    uint64_t delta = DELTA;                      // a key schedule constant 
    for (i=0; i<32; i++) {                                 // basic cycle start 
        v1 -= ((v0<<4) + key) ^ (v0 + sum) ^ ((v0>>5) + key);
        v0 -= ((v1<<4) + key) ^ (v1 + sum) ^ ((v1>>5) + key);
        sum -= delta;
    }                                                      // end cycle 
    v[0]=v0; v[1]=v1;
}