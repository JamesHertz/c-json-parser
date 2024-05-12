#ifndef __JSON_MEMORY__
#define __JSON_MEMORY__

#include <stdio.h>
#include <stdlib.h>

static inline void * json_mem_alloc(size_t size){
    void * ptr = malloc(size);
#ifdef JSON_MEM_FAIL_STOP
    if(ptr == NULL){
        fprintf(stderr, "[ERROR] Failed to alloc %zu bytes", size);
        exit(1);
    }
#endif 
    return ptr;
}

static inline void * json_mem_realloc(void * old, size_t new_size){
    void * ptr = realloc(old, new_size);
#ifdef JSON_MEM_FAIL_STOP
    if(ptr == NULL){
        fprintf(stderr, "[ERROR] Failed to realloc %zu bytes", new_size);
        exit(1);
    }
#endif 
    return ptr;
}

static inline void json_mem_release(void * ptr){
    free(ptr);
}

#endif
