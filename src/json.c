#include "json.h"
#include "error.h"
#include "scanner.h"
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <assert.h> // TODO: remove this later

typedef struct {
    scanner_t scan;
    json_result_t * result;
    jmp_buf jbuf;
    // list of allocated memory
    // setjmp values
} json_parser_t;

static void parser_error(json_parser_t *, token_t, error_code_t) 
    __attribute__ ((noreturn));

static void parser_error(
        json_parser_t * ctx, 
        token_t position, 
        error_code_t errcode) {

        ctx->result->is_error = true;
        error_t * err = &ctx->result->as.error;

        err->errcode  = errcode;
        err->position = position.lexeme;
        err->line_nr  = position.line_nr;
        longjmp(ctx->jbuf, 1);
}

static token_t parser_advance(json_parser_t * ctx){
    token_t token = scanner_scan_next(&ctx->scan);
    if(token.type == TOKEN_ERROR)
        parser_error(ctx, token, token.error_or_lenth.errcode);
    return token;
}

static json_object_t * parser_json_object(json_object_t * ctx){
    return NULL;
}

static json_array_t * parser_json_array(json_object_t * ctx){
    return NULL;
}

static void * parser_allocate(json_parser_t * ctx, size_t bytes){
    void * memory = malloc(bytes);
    // TODO: report error later on c:
    assert(memory != NULL);
    return memory;
}

static char * parser_allocate_string(json_parser_t * ctx, const char * src, size_t strlen){
    char * str = parser_allocate(ctx, strlen + 1);
    memcpy(str, src, strlen);
    str[strlen] = 0;
    return str;
}

static json_value_t parser_json_value(json_parser_t * ctx){
    token_t token = parser_advance(ctx);

    switch (token.type) {
        case TOKEN_NULL:
            return JSON_NULL_VALUE;
        case TOKEN_TRUE:
            return AS_JSON_BOOL(true);
        case TOKEN_FALSE:
            return AS_JSON_BOOL(false);
        case TOKEN_NUMBER: {
            // TODO: should I report?
            char * str = parser_allocate_string(
                ctx, token.lexeme, token.error_or_lenth.lexeme_lenght
            );
            double value = strtod(str, NULL);
            free(str);
        }
        case TOKEN_STRING: 
            return AS_JSON_STRING(
                parser_allocate_string(
                    // (+1) to skip the double quote and (-2) to remove then from length
                    ctx, token.lexeme + 1, token.error_or_lenth.lexeme_lenght - 2
                )
            );
        default:
            // FIXME: add the right error code
            parser_error(ctx,token, SCANNER_UNEXPECTED_SYMBOL);
    }

}


// public functions

void json_parse(json_result_t *result, const char *source){
    json_parser_t parser_ctx = {
        .result = result
    };
    scanner_init(&parser_ctx.scan, source);
    if(setjmp(parser_ctx.jbuf) == 0){
        result->is_error      = false;
        result->as.json_value = parser_json_value(&parser_ctx);
        // TODO: consume EOF c:
    } else {
        // TODO: free all pointers c:
    }
    // do set jump c:
}


const char * json_valuetype2str(json_value_type_t type){
#define CASE(value_type) case value_type: return #value_type
    switch (type) {
        CASE(JSON_NUMBER);
        CASE(JSON_STRING);
        CASE(JSON_BOOLEAN);
        CASE(JSON_OBJECT);
        CASE(JSON_ARRAY);
        CASE(JSON_NULL);
        default: return NULL; // unreachable!!
    }
#undef CASE
}
