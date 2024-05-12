#ifndef __JSON_SCANNER__
#define __JSON_SCANNER__

#include <stdio.h>
#include "error.h"

typedef enum {
    TOKEN_RIGHT_BRACKET, TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACE,   TOKEN_LEFT_BRACE,
    TOKEN_COLON, TOKEN_COMMA,

    TOKEN_TRUE, TOKEN_FALSE,
    TOKEN_STRING, TOKEN_NUMBER, 
    TOKEN_ERROR, TOKEN_EOF
} token_type_t;

typedef struct {
    token_type_t type;
    const char * lexeme;
    size_t  line_nr;
    union {
        size_t  lenght;       // used for token
        error_t errcode;
    } error_or_lenth;
} token_t;

typedef struct {
    const char * current;
    const char * start;
    size_t curr_line;
} scanner_t;

// TODO: add support for numbers that starts with e and \uxxxx chars c:
void scanner_init(scanner_t * scan, const char * text);
token_t scanner_scan_next(scanner_t * scan);
const char * ttype2str(token_type_t type);

#endif
