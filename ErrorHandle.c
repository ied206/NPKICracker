#include <stdio.h>
#include <stdlib.h>

#include "ErrorHandle.h"

void JV_ErrorHandle(int code)
{
	char *msg = NULL;
	switch (code)
	{
	case JVERR_PRIVATE_KEY_NOT_EXIST:
		msg =	"ErrorMessage : JVERR_PRIVATE_KEY_NOT_EXIST\n"
				"NPKI private key argument doesn't exist!\n";
		break;
	case JVERR_PW_MIN_LENGTH_NOT_EXIST:
		msg = 	"ErrorMessage : JVERR_PW_MIN_LENGTH_NOT_EXIST\n"
				"Please specify password's minimum length.\n";
		break;
	case JVERR_PW_MAX_LENGTH_NOT_EXIST:
		msg = 	"ErrorMessage : JVERR_PW_MAX_LENGTH_NOT_EXIST\n"
				"Please specify password's maximum length.\n";
		break;
	case JVERR_PW_CHARSET_NOT_EXIST:
		msg = 	"ErrorMessage : JVERR_PW_CHARSET_NOT_EXIST\n"
				"Please specify valid charset file's path.\n";
		break;
	case JVERR_PW_CHARSET_TOO_LONG:
		msg = 	"ErrorMessage : JVERR_PW_CHARSET_TOO_LONG\n"
				"Password Charset is too long. Cannot be over MAX_PW_CHARSET\n";
		break;
	case JVERR_PW_INITIAL_NOT_EXIST:
		msg = 	"ErrorMessage : JVERR_PW_INITIAL_NOT_EXIST\n"
				"Initial password argument doesn't exist!\n";
		break;
	case JVERR_PW_INITIAL_NOT_VALID:
		msg = 	"ErrorMessage : JVERR_PW_INITIAL_NOT_VALID\n"
				"Initial password is not valid with charset\n";
		break;
	case JVERR_PW_CHARSET_DUPLICATE:
		msg = 	"ErrorMessage : JVERR_PW_CHARSET_DUPLICATE\n"
				"There are duplicated character in charset.\n";
		break;
	case JVERR_PW_INITIAL_TOO_SHORT:
		msg = 	"ErrorMessage : JVERR_PW_INITIAL_TOO_SHORT\n"
				"Initial password is shorter than min password length.\n";
		break;
	case JVERR_PW_INITIAL_TOO_LONG:
		msg = 	"ErrorMessage : JVERR_PW_INITIAL_TOO_LONG\n"
				"Initial password is longer than max password length.\n";
		break;
	case JVERR_PRINT_INTERVAL_NOT_EXIST:
		msg = 	"ErrorMessage : JVERR_PRINT_INTERVAL_NOT_EXIST\n"
				"Print Interval arg is not after -p.\n";
		break;
	case JVERR_PRINT_INTERVAL_TOO_SHORT:
		msg = 	"ErrorMessage : JVERR_PRINT_INTERVAL_TOO_SHORT\n"
				"Print interval must be positive number.\n";
		break;
	case JVERR_PRINT_INTERVAL_TOO_LONG:
		msg = 	"ErrorMessage : JVERR_PRINT_INTERVAL_TOO_SHORT\n"
				"Print interval is longer than 1 hour.\n";
		break;
	case JVERR_PW_MIN_LENGTH_TOO_SHORT:
		msg = 	"ErrorMessage : JVERR_PW_MIN_LENGTH_TOO_SHORT\n"
				"Minimum password length must be positive number.\n";
		break;
	case JVERR_PW_MAX_LENGTH_TOO_SHORT:
		msg = 	"ErrorMessage : JVERR_PW_MAX_LENGTH_TOO_SHORT\n"
				"Maximum password length cannot be shorter than minimum password length.\n";
		break;
	case JVERR_PW_MAX_LENGTH_TOO_LONG:
		msg = 	"ErrorMessage : JVERR_PW_MAX_LENGTH_TOO_LONG\n"
				"Maximum password length cannot be longer than 48.\n";
		break;
	case JVERR_SEED_ASSERT_FAILED:
		msg = 	"ErrorMessage : JVERR_SEED_ASSERT_FAILED\n"
				"SEED assert for variables failed.\n";
		break;
	default:
		msg = 	"ErrorMessage : UNDEFINDED ERROR\n"
				"Undefined Error\n";
		break;
	}
	printf("ErrorCode : %d\n%s", code, msg);
	exit(code);
}

void JV_WarnHandle(int code)
{
	char *msg = NULL;
	switch (code)
	{
	case JVWARN_NOT_ENOUGH_ARGV:
		msg = 	"WarnMessage : JVWARN_NOT_ENOUGH_ARGV\n"
				"Not enough argv\n";
		break;
	case JVWARN_NOT_VALID_ARGV:
		msg = 	"WarnMessage : JVWARN_NOT_VALID_ARGV\n"
				"Not valid argv\n";
		break;
	case JVWARN_RESULT_NOT_OK_FREAD:
		msg = 	"WarnMessage : JVWARN_RESULT_NOT_OK_FREAD\n"
				"fread didn't returned ok sign\n";
		break;
	case JVWARN_RESULT_NOT_OK_FGETS:
		msg = 	"WarnMessage : JVWARN_RESULT_NOT_OK_FGETS\n"
				"fgets didn't returned ok sign\n";
		break;
	default:
		msg = "WarnMessage : UNDEFINDED WARNING\nUndefined Warning\n";
		break;
	}
	printf("WarnCode : %d\n%s", code, msg);
}
