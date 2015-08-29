#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

/* header */

#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

#ifdef __BIG_ENDIAN__
# define SHA_BIG_ENDIAN
#endif

typedef struct sha1nfo {

	uint32_t buffer[BLOCK_LENGTH/4];
	uint32_t state[HASH_LENGTH/4];
	uint32_t byteCount;
	uint8_t bufferOffset;
	uint8_t keyBuffer[BLOCK_LENGTH];
	uint8_t innerHash[HASH_LENGTH];
} sha1nfo;

void sha1_init(sha1nfo *s);
void sha1_writebyte(sha1nfo *s, uint8_t data);
void sha1_write(sha1nfo *s, const uint8_t *data, size_t len);
uint8_t* sha1_result(sha1nfo *s);

uint8_t* JV_PBKDF1(uint8_t* dkey, const uint8_t password[], const size_t pwlen, const uint8_t salt[], const size_t saltlen, const uint32_t itercount);
uint8_t* JV_SHA1(uint8_t* out, const uint8_t* input, const size_t inputlen);

#endif // HASH_H_INCLUDED
