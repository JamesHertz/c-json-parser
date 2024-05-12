#ifndef __JSON_MEMORY__
#define __JSON_MEMORY__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void * json_mem_alloc(size_t size){
    void * ptr = malloc(size);
#ifdef JSON_MEM_FAIL_STOP
    if(ptr == NULL){
        fprintf(stderr, "[ERROR] Failed to alloc %zu bytes\n", size);
        exit(1);
    }
#endif 
    return ptr;
}

static inline void * json_mem_realloc(void * old, size_t new_size){
    void * ptr = realloc(old, new_size);
#ifdef JSON_MEM_FAIL_STOP
    if(ptr == NULL){
        fprintf(stderr, "[ERROR] Failed to realloc %zu bytes\n", new_size);
        exit(1);
    }
#endif 
    return ptr;
}

static inline void json_mem_release(void * ptr){
    free(ptr);
}

static inline char * json_mem_strdup(const char * str){
    char * result = strdup(str);
#ifdef JSON_MEM_FAIL_STOP
    if(str == NULL){
        fprintf(stderr, "[ERROR] Failed to copy string '%s'\n", str);
        exit(1);
    }
#endif 
    return result;
}



#endif
