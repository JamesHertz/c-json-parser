#ifndef __JSON_ERROR__
#define __JSON_ERROR__
#include <stdio.h>

typedef enum {
    // scanner errors
    SCANNER_UNEXPECTED_SYMBOL,
    SCANNER_UNKNOWN_ESCAPE,
    SCANNER_UNFINISH_STRING,
    SCANNER_BAD_NUMBER_FORMAT,
    SCANNER_INVALID_INDENTIFIER


    // parser erros
} error_code_t;

typedef struct {
    error_code_t errcode;
    const char * position;
    size_t line_nr;
} error_t;

const char * error2str(error_code_t error);
#endif
