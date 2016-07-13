#ifndef SEED_INCLUDED
#define SEED_INCLUDED

// Simplified SEED Decrypt code from KISA_SEED
// Intended for conversion to OpenCL Code

#include <stdint.h>
#include <sys/types.h>

typedef void (*block128_f) (const unsigned char in[16], unsigned char out[16], const void *key);

// KISA
/**************************** Constant Definitions ****************************/

#define NoRounds         16						// the number of rounds
#define NoRoundKeys      (NoRounds*2)			// the number of round-keys
#define SeedBlockSize    16    					// block length in bytes
#define SeedBlockLen     128   					// block length in bits


/******************************** Common Macros *******************************/

// macroses for left or right rotations
#define ROTL(x, n)     (((x) << (n)) | ((x) >> (32-(n))))		// left rotation
#define ROTR(x, n)     (((x) >> (n)) | ((x) << (32-(n))))		// right rotation

// macroses for converting endianess
#define EndianChange(dwS)                       \
    ( (ROTL((dwS),  8) & (uint32_t)0x00ff00ff) |   \
      (ROTL((dwS), 24) & (uint32_t)0xff00ff00) )

/******************** Macros for Encryption and Decryption ********************/

#define GetB0(A)  ( (uint8_t)((A)    ) )
#define GetB1(A)  ( (uint8_t)((A)>> 8) )
#define GetB2(A)  ( (uint8_t)((A)>>16) )
#define GetB3(A)  ( (uint8_t)((A)>>24) )

// Round function F and adding output of F to L.
// L0, L1 : left input values at each round
// R0, R1 : right input values at each round
// K : round keys at each round
#define SeedRound(L0, L1, R0, R1, K) {             \
    T0 = R0 ^ (K)[0];                              \
    T1 = R1 ^ (K)[1];                              \
    T1 ^= T0;                                      \
    T1 = SS0[GetB0(T1)] ^ SS1[GetB1(T1)] ^         \
         SS2[GetB2(T1)] ^ SS3[GetB3(T1)];          \
    T0 += T1;                                      \
    T0 = SS0[GetB0(T0)] ^ SS1[GetB1(T0)] ^         \
         SS2[GetB2(T0)] ^ SS3[GetB3(T0)];          \
    T1 += T0;                                      \
    T1 = SS0[GetB0(T1)] ^ SS1[GetB1(T1)] ^         \
         SS2[GetB2(T1)] ^ SS3[GetB3(T1)];          \
    T0 += T1;                                      \
    L0 ^= T0; L1 ^= T1;                            \
}


/************************ Constants for Key schedule **************************/

//		KC0 = golden ratio; KCi = ROTL(KCi-1, 1)
#define KC0     0x9e3779b9UL
#define KC1     0x3c6ef373UL
#define KC2     0x78dde6e6UL
#define KC3     0xf1bbcdccUL
#define KC4     0xe3779b99UL
#define KC5     0xc6ef3733UL
#define KC6     0x8dde6e67UL
#define KC7     0x1bbcdccfUL
#define KC8     0x3779b99eUL
#define KC9     0x6ef3733cUL
#define KC10    0xdde6e678UL
#define KC11    0xbbcdccf1UL
#define KC12    0x779b99e3UL
#define KC13    0xef3733c6UL
#define KC14    0xde6e678dUL
#define KC15    0xbcdccf1bUL


/************************** Macros for Key schedule ***************************/

#define RoundKeyUpdate0(K, A, B, C, D, KC) {	\
    T0 = A + C - KC;                            \
    T1 = B + KC - D;                            \
    (K)[0] = SS0[GetB0(T0)] ^ SS1[GetB1(T0)] ^  \
             SS2[GetB2(T0)] ^ SS3[GetB3(T0)];   \
    (K)[1] = SS0[GetB0(T1)] ^ SS1[GetB1(T1)] ^  \
             SS2[GetB2(T1)] ^ SS3[GetB3(T1)];   \
    T0 = A;                                     \
    A = (A>>8) ^ (B<<24);                       \
    B = (B>>8) ^ (T0<<24);                      \
}

#define RoundKeyUpdate1(K, A, B, C, D, KC) {	\
    T0 = A + C - KC;                            \
    T1 = B + KC - D;                            \
    (K)[0] = SS0[GetB0(T0)] ^ SS1[GetB1(T0)] ^  \
             SS2[GetB2(T0)] ^ SS3[GetB3(T0)];   \
    (K)[1] = SS0[GetB0(T1)] ^ SS1[GetB1(T1)] ^  \
             SS2[GetB2(T1)] ^ SS3[GetB3(T1)];   \
    T0 = C;                                     \
    C = (C<<8) ^ (D>>24);                       \
    D = (D<<8) ^ (T0>>24);                      \
}



void JV_SeedRoundKey(uint32_t *pdwRoundKey,	uint8_t *pbUserKey);
//void JV_SeedDecrypt(uint8_t *pbData, uint8_t *pbPlain, uint32_t *pdwRoundKey);
void JV_SEED_CBC128_Decrypt_Serial(const uint8_t *in, uint8_t *out, const size_t length, const uint32_t *K, const uint8_t iv[16]);
void JV_SEED_CBC128_Decrypt_OneBlock(const uint8_t *in, uint8_t *out, const uint32_t *K, const uint8_t iv[16]);
void JV_SEED_CBC128_Decrypt_NoBranch(const uint8_t *virt_in, uint8_t *out, const size_t length, const uint32_t *K);

#endif // SEED_INCLUDED
