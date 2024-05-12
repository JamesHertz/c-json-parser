#include <string.h>
#include <stdio.h>

#include "da-array.h"
#include "memory.h"

#define DEFAULT_SIZE 8
#define CALC_POS(position)  (array->values + elem_size * (position))

da_array * da_init(size_t elem_size, size_t initial_size){
    da_array * ptr = json_mem_alloc(sizeof(da_array));
    if(ptr == NULL) return ptr;

    ptr->length = 0;
    ptr->size   = initial_size == 0 ? DEFAULT_SIZE : initial_size;

    void * values = json_mem_alloc(ptr->size * elem_size);
    if(values == NULL){
        json_mem_release(ptr);
        return NULL;
    }

    ptr->values = values;
    return ptr;
}

int da_add(size_t elem_size, da_array * array, void * elem){
    if(array->length == array->size){
        void * values = json_mem_realloc(
                array->values, array->size * 2 * elem_size
        );
        if(values == NULL) return -1;
        array->size   *= 2;
        array->values = values;
    }

    void * ptr = CALC_POS(array->length++);
    memcpy(ptr, elem, elem_size);
    return 0;
}

void da_set(size_t elem_size, da_array * array, size_t position, void * elem){
    if(position >= array->length){
        fprintf(
            stderr, "[ERROR] Trying to modify position %zu in array with length %zu.", position, array->length
        );
        exit(1);
    }

    void * ptr = CALC_POS(position);
    memcpy(ptr, elem, elem_size);
}

void da_remove(size_t elem_size, da_array * array, size_t position){
    if(position >= array->length){
        fprintf(
            stderr, "[ERROR] Trying to remove position %zu in array with length %zu.", position, array->length
        );
        exit(1);
    }

    void * target_pos = CALC_POS(position);
    void * next_pos   = CALC_POS(position+1);
    void * end_pos    = CALC_POS(array->length);
    
    memmove(target_pos, next_pos, end_pos - next_pos);
    array->length--;
}

void da_fast_remove(size_t elem_size, da_array * array, size_t position){
    if(position >= array->length){
        fprintf(
            stderr, "[ERROR] Trying to remove position %zu in array with length %zu.", position, array->length
        );
        exit(1);
    }

    if(position != array->length - 1){
        void * target_pos = CALC_POS(position);
        void * last_pos   = CALC_POS(array->length - 1);
        memcpy(target_pos, last_pos, elem_size);
    }
    array->length--;
}

size_t da_find(size_t elem_size, da_array * array, find_func func, void * ctx){
    for(size_t i = 0; i < array->length; i++){
        void * ptr = CALC_POS(i);
        if(func(ctx, ptr))
            return i;
    }
    return -1;
}

void da_destroy(da_array * array){
    json_mem_release(array->values);
    json_mem_release(array);
}
