#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "scanner.h"
#include "error.h"

static inline char scanner_peek(const scanner_t * scan){
    return *scan->current;
}

static inline bool scanner_isover(const scanner_t * scan){
    return scanner_peek(scan) == '\0';
}

static inline char scanner_peek_next(const scanner_t * scan){
    if(scanner_isover(scan))
        return '\0';
    return scan->current[1];
}

static inline char scanner_advance(scanner_t * scan){
    return *scan->current++;
}

static inline char scanner_previous(scanner_t * scan){
    return scan->current[-1];
}

static void scanner_skip_spaces(scanner_t * scan){
    for(;;){
        switch (scanner_peek(scan)) {
            case '\n':
                scan->curr_line++;
            case  ' ':
            case '\t':
            case '\r':
            case '\v':
                scanner_advance(scan);
                break;
            default:
                return;
        }
    }

}

static token_t scanner_make_token(const scanner_t * scan, token_type_t type){
    token_t result =  {
        .type      = type,
        .lexeme    = scan->start,
        .line_nr   = scan->curr_line
    };
    result.error_or_lenth.lenght = (scan->current - scan->start);
    return result;
}

static token_t scanner_make_error(const scanner_t * scan, error_t errcode){
    token_t error = {
        .type     = TOKEN_ERROR,
        .lexeme   = scan->current - 1,
        .line_nr  = scan->curr_line
    };
    error.error_or_lenth.errcode= errcode;
    return error;
}

static token_t scanner_scan_string(scanner_t *scan){
    char ch;

    while(!scanner_isover(scan) && (ch = scanner_advance(scan)) != '"'){
        if(ch == '\n')
            break;

        if(ch == '\\') {
            switch(scanner_advance(scan)){
                case '"':
                case '\\':
                case 'f':
                case 'b':
                case 'n':
                case 'r':
                case 't':
                    break;
                // case 'u': TODO: fix this c:
                default:
                    return scanner_make_error(
                            scan, SCANNER_UNKNOWN_ESCAPE
                    );
            }
        }
        
    }

    return (scanner_previous(scan) == '"') ? 
          scanner_make_token(scan, TOKEN_STRING) 
        : scanner_make_error(scan, SCANNER_UNFINISH_STRING);
}

static token_t scanner_scan_number(scanner_t *scan){

    while(isdigit( scanner_peek(scan) ))
        scanner_advance(scan);


    if(scanner_peek(scan) == '.'){

        if(!isdigit( scanner_peek_next(scan) ))
            return scanner_make_error(scan, SCANNER_BAD_NUMBER_FORMAT);

        while(isdigit( scanner_peek(scan) ))
            scanner_advance(scan);

    }

    if(scanner_peek(scan) == '.')
        return scanner_make_error(scan, SCANNER_BAD_NUMBER_FORMAT);

    return scanner_make_token(scan, TOKEN_NUMBER);
}

static token_t scanner_scan_identifier(scanner_t *scan){

    // TODO: try to extend this further (for better error report)
    while(isalpha( scanner_peek(scan) ))
        scanner_advance(scan);

    size_t size = scan->current - scan->start;

    if(strncmp("true", scan->start, size) == 0){
        return scanner_make_token(scan, TOKEN_TRUE);
    } 

    if(strncmp("false", scan->start, size) == 0){
        return scanner_make_token(scan, TOKEN_FALSE);
    }

    // little hack to get better error msg c:
    token_t error = scanner_make_token(scan, TOKEN_ERROR);
    error.error_or_lenth.errcode = SCANNER_INVALID_INDENTIFIER;
    return error;
}

// public functions
void scanner_init(scanner_t *scan, const char *text){
    scan->current   = text;
    scan->start     = text;
    scan->curr_line = 1;
}

token_t scanner_scan_next(scanner_t *scan){
    scanner_skip_spaces(scan);

    if(scanner_isover(scan))
        return scanner_make_token(scan, TOKEN_EOF);
    
    scan->start = scan->current;

    char ch = scanner_advance(scan); 
    if(isdigit(ch) || ch == '-')
        return scanner_scan_number(scan);

    if(isalpha(ch))
        return scanner_scan_identifier(scan);

    switch (ch) {
        case '{':
            return scanner_make_token(scan, TOKEN_LEFT_BRACE);
        case '}':
            return scanner_make_token(scan, TOKEN_RIGHT_BRACE);
        case '[':
            return scanner_make_token(scan, TOKEN_LEFT_BRACKET);
        case ']':
            return scanner_make_token(scan, TOKEN_RIGHT_BRACKET);
        case ':':
            return scanner_make_token(scan, TOKEN_COLON);
        case ',':
            return scanner_make_token(scan, TOKEN_COMMA);
        case '"':
            return scanner_scan_string(scan);
        default:
            return scanner_make_error(scan, SCANNER_UNEXPECTED_SYMBOL);
    }

    return (token_t) { };
}



const char * ttype2str(token_type_t type){ 

#define CASE(token) case token: return #token

    switch (type) {

        CASE(TOKEN_RIGHT_BRACKET);
        CASE(TOKEN_LEFT_BRACKET);
        CASE(TOKEN_RIGHT_BRACE);
        CASE(TOKEN_LEFT_BRACE);
        CASE(TOKEN_STRING);
        CASE(TOKEN_NUMBER);
        CASE(TOKEN_COLON);
        CASE(TOKEN_COMMA);
        CASE(TOKEN_TRUE);
        CASE(TOKEN_FALSE);
        CASE(TOKEN_ERROR);
        CASE(TOKEN_EOF);

        default: return NULL; // unreachable!!
    }
#undef CASE

}


