#ifndef __JSON__
#define __JSON__

#include <stdio.h>
#include <stdbool.h>
#include "error.h"

typedef enum {
    JSON_NUMBER,
    JSON_STRING,
    JSON_BOOLEAN,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_NULL
} json_value_type_t;

typedef struct _json_object_ json_object_t;
typedef struct _json_array_  json_array_t;

typedef struct {
    json_value_type_t type;
    union {
        double number;
        bool  boolean;
        char * string;
        json_object_t * object;
        json_array_t * array;
    } as;
} json_value_t;;

typedef struct{
    const char * key;
    json_value_t value;
} json_object_entry_t;

struct _json_array_ {
    size_t length;
    size_t size;
    json_value_t  * values;
};

struct _json_object_ {
    size_t length;
    size_t size;
    json_object_entry_t * values;
};

typedef struct {
    bool is_error;
    union {
        json_value_t json_value;
        error_t error; // TODO: make this a list of errors c:
    } as;
} json_result_t;


// super useful macros you will enjoy using
#define AS_JSON_NUMBER(value) ((json_value_t)  { .type = JSON_NUMBER,  .as = { .number  = (value) } } )
#define AS_JSON_STRING(value) ((json_value_t)  { .type = JSON_STRING,  .as = { .string  = (value) } } )
#define AS_JSON_BOOL(value)   ((json_value_t)  { .type = JSON_BOOLEAN, .as = { .boolean = (value) } } )
#define JSON_NULL_VALUE       ((json_value_t)  { .type = JSON_NULL } )


// #define AS_JSON_ARRAY(value)  ((json_value_t)  { .type = JSON_ARRAY,   .as = { .array   = (value) } } )
// #define AS_JSON_OBJECT(value) ((json_value_t)  { .type = JSON_OBJECT,  .as = { .object  = (value) } } )


// #define AS_JSON_ARRAY_VALUES(lvalue,...)
/*
 
   TODO: do the following macros

   AS_JSON_ARRRAY_VALUES
   BUILD_JSON_VALUES 
 
   AS_JSON_OBJECT_VALUES
   BUILD_JSON_OBJECT_ENTRIES 
   AS_JSON_OBJECT_ENTRY 

   // so that the code below works c:

   json_value_t value = AS_JSON_ARRAY(
        AS_JSON_ARRAY_VALUES(
            AS_JSON_BOOL(true),
            AS_JSON_BOOL(false),
            AS_JSON_NUMBER(10.0)
        )
    );

    json_value_t obj = BUILD_JSON_OBJECT(
        AS_JSON_OBJECT_ENTRY("version", AS_JSON_NUMBER(10)),
        AS_JSON_OBJECT_ENTRY("authors", BUILD_JSON_ARRAY(
            AS_JSON_STRING("James Hertz"),
            AS_JSON_STRING("Jason Furtado")
        )
    );

*/

// json value pointer has ownership
void json_parse(json_result_t * result, const char * source);


const char * json_valuetype2str(json_value_type_t type);

// TODO: USE MACROS AND STUFFS to make it so it exits the progam when
//       it is unable to reserve memory
// int json_array_add_value(json_array_t * array, json_value_t value);
// int json_object_add_entry(json_object_t * obj, const char * key, json_value_t value);

// `value`     value where to store the object
// `init_size` initial size of the arrray (use 0 if you have no idea of the initial size)
// @return 0 if everything was okay and -1 if it got problems allocating memory
// int json_object_init(json_value_t value, size_t init_size);

// `value`     value where to store the array
// `init_size` initial size of the arrray (use 0 if you have no idea of the initial size)
// @return 0 if everything was okay and -1 if it got problems allocating memory
// int json_array_init(json_value_t value, size_t init_size);

// // helper functions
// json_object_entry_t * json_object_iterator_init(json_object_t * obj);
// json_object_entry_t * json_object_iteration_next(json_object_t * obj, json_object_t * last_entry);
//
// json_value_t * json_array_iterator_init(json_array_t * obj);
// json_value_t * json_array_iterator_next(json_object_t * obj, json_value_t * last_value);


#endif
