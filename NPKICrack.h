#ifndef NPKICRACK_H_INCLUDED
#define NPKICRACK_H_INCLUDED

#include <stdint.h>

#define MAX_PASSWORD	48
#define MAX_PW_CHARSET	128
// Print Interval in second
#define DEFAULT_PRINT_INTERVAL	5

struct npki_private_key
{
	uint8_t *rawkey;
	long rawkey_len;
	uint8_t salt[8]; // dkey, div, buf is temporary
    uint16_t itercount;
    uint8_t *crypto;
    long crypto_len;
    uint8_t *plain;
};
typedef struct npki_private_key NPKIPrivateKey;

struct npki_brute_force
{
	char* pkey_path;		// Private Key Path
	char* pw_charset_path;	// Password Charset
	char* pw_init;			// Initial Password
	char password[MAX_PASSWORD];		// Password
    char pw_charset[MAX_PW_CHARSET];	// Password Charset
	uint32_t pw_min_len;	// Password Minimum Length
    uint32_t pw_max_len;	// Password Maximum Length
    uint64_t pw_cursor;	// Now, which n'th pw is to iterate?
    uint64_t decrypt;
    uint32_t use_opencl;
    uint32_t print_interval;
    time_t	starttime;
};
typedef struct npki_brute_force NPKIBruteForce;

void 		BruteForceIterate (NPKIPrivateKey *pkey, NPKIBruteForce *bforce);
void 		NPKIDecrypt (NPKIPrivateKey *pkey, const char* password);

char* 		PasswordGenerate (NPKIBruteForce *bforce, char* password);

// For NPKIPrivateKey
void		NPK_Init (NPKIPrivateKey *pkey);
void 		NPK_Duplicate (NPKIPrivateKey *dest, NPKIPrivateKey *src);
int 		NPK_ReadRaw (NPKIPrivateKey *pkey, const char* PrivateKeyPath);
void 		NPK_Parse (NPKIPrivateKey *pkey);
void 		NPK_Free (NPKIPrivateKey *pkey);

//
void		NBF_Init (NPKIBruteForce *bforce);
int			NBF_ReadCharset (NPKIBruteForce *bforce);
int			NBF_ValidateInitPW	(NPKIBruteForce *bforce);
uint64_t 	NBF_SetCursorFromInitPW (NPKIBruteForce *bforce);
uint32_t 	NBF_GetCharsetNumber(NPKIBruteForce *bforce, const char tofind);
void 		NBF_Ready (NPKIBruteForce *bforce);


void 		GetMaxCursor(NPKIBruteForce *bforce);
void 		SetStartTime(NPKIBruteForce *bforce);

void 		NBF_PrintEnvInfo(NPKIBruteForce *bforce);

#endif // NPKICRACK_H_INCLUDED
