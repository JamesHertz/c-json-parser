#ifndef __JSON_ERROR__
#define __JSON_ERROR__
#include <err.h>
#include <stdio.h>

typedef enum {
    SCANNER_UNEXPECTED_SYMBOL,
    SCANNER_UNKNOWN_ESCAPE,
    SCANNER_UNFINISH_STRING,
    SCANNER_BAD_NUMBER_FORMAT,
    SCANNER_INVALID_INDENTIFIER
} error_t;

const char * error2str(error_t error);
#endif
