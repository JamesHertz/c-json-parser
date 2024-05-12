#include "json.h"
#include "da-array.h"
#include "error.h"
#include "memory.h"
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

// static void * parser_allocate(json_parser_t * ctx, size_t bytes){
//     // TODO: 
//     //      - check for null and report error
//     //      - fill the linked list of allocated memory c:
//     void * memory = json_mem_alloc(bytes);
//     return memory;
// }

static char * parser_allocate_string(json_parser_t * ctx, const char * src, size_t strlen){
    char * str = json_mem_alloc(strlen);
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

// functions to deal with json objects c:
json_array_t * json_array_new(size_t init_size){
    return (json_array_t*) pda_init(json_value_t, init_size);
}

int  json_array_add(json_array_t * array, json_value_t value){
    return pda_add(json_value_t, array, &value);
}

void json_array_remove(json_array_t * array, size_t position){
    pda_remove(json_value_t, array, position);
}

json_object_t * json_object_new(size_t init_size){
    return (json_object_t*) pda_init(json_object_entry_t, init_size);
}

bool find_entry(void * ctx, const void * elem){
    char * s1 = ctx;
    const char * s2 = ((json_object_entry_t*) elem)->key;
    return strcmp(s1, s2) == 0;
} 

// only borrows key and makes it own personal copy
int  json_object_set(json_object_t * obj, const char * key, json_value_t value){
    size_t pos = pda_find(json_object_entry_t, obj, find_entry, key);
    if(pos < 0){
        char * keycopy = json_mem_strdup(key);
        if(keycopy == NULL) return 1;
        json_object_entry_t entry = {
            .key =  keycopy,
            .value = value
        };
        return pda_add(json_object_entry_t, obj, &entry);
    } 

    json_object_entry_t * entry = &obj->values[pos];
    json_destroy_value(entry->value);
    entry->value = value;
    return 0;
}

void json_object_remove(json_object_t * obj, size_t position){
    pda_remove(json_object_entry_t, obj, position);
}

// helper functions
json_object_entry_t * json_object_iterator_init(json_object_t * obj){
    return pda_iterator_init(obj);
}

json_object_entry_t * json_object_iteration_next(json_object_t * obj, json_object_t * last_entry){
    return pda_iterator_next(json_object_entry_t, obj, last_entry);
}

json_value_t * json_array_iterator_init(json_array_t * array){
    return pda_iterator_init(array);
}

json_value_t * json_array_iterator_next(json_array_t * array, json_value_t * last_value){
    return pda_iterator_next(json_value_t, array, last_value);
}

void json_destroy_array(json_array_t * array){
    for(size_t i = 0; i < array->length; i++){
        json_destroy_value(array->values[i]);
    }
    pda_destroy(array);
}

void json_destroy_object(json_object_t * obj){
    for(size_t i = 0; i < obj->length; i++){
        json_object_entry_t * entry = &obj->values[i];
        json_destroy_value(entry->value);
        json_mem_release((void*)entry->key);
        entry->key = NULL;
    }
    pda_destroy(obj);
}

void json_destroy_value(json_value_t value){
    switch (value.type) {
        case JSON_ARRAY:
            json_destroy_array(value.as.array);
            value.as.array = NULL;
            break;
        case JSON_OBJECT:
            json_destroy_object(value.as.object);
            value.as.object = NULL;
            break;
        case JSON_STRING:
            // TODO: think about this seriously
            json_mem_release((void*) value.as.string);
            value.as.string = NULL;
            break;
        default:
            break;
    }
}
