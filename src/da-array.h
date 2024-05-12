#ifndef __JSON_DA_ARRAY__
#define __JSON_DA_ARRAY__
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>

// WARNING: TO USE THIS MODULE ARRAYS SHOULD FOLLOW RIGIDLY THE FORMAT BELOW
//          (the filds name can be different but their position should not change)
typedef struct{
    size_t length;
    size_t size;
    // replace void with the respective 
    // type of yours dynamic array c:
    void * values;
} da_array;

typedef bool (*find_func)(void * ctx, const void * elem);

da_array * da_init(size_t elem_size, size_t intial_size);
int da_add(size_t elem_size, da_array * array, void * elem);
void da_set(size_t elem_size, da_array * array, size_t position, void * elem);

// bool da_remove(size_t elem_size, da_array * array, eq_func func, void *elem);
void da_remove(size_t elem_size, da_array * array, size_t position);
// doesn't move elements back c:
void da_fast_remove(size_t elem_size, da_array * array, size_t position);

size_t da_find(size_t elem_size, da_array * array, find_func func, void * ctx);

// takes ownership c:
void da_destroy(da_array * array);

// the return result is a borrowed pointer and it should not be changed
void * da_iterator_init(da_array * array);
// returns the address to the next elem and NULL to determinte that is over
void * da_iterator_next(size_t elem_size, da_array * array, void * previous);

#define pda_init(type, initsize)              da_init(sizeof(type), initsize)
#define pda_add(type, array, elem)            da_add(sizeof(type), (da_array*) array, elem)
#define pda_set(type, array, pos, elem)       da_set(sizeof(type), (da_array*) array, pos, elem)
#define pda_remove(type, array, pos)          da_remove(sizeof(type), (da_array*) array, pos)
#define pda_fast_remove(type, array, pos)     da_fast_remove(sizeof(type), (da_array*) array, pos) 
#define pda_find(type, array, func, ctx)      da_find(sizeof(type), (da_array*) array, func, (void*) ctx) 
#define pda_destroy(array)                    da_destroy((da_array*) array) 
#define pda_iterator_init(array)              da_iterator_init((da_array*) array)
#define pda_iterator_next(type, array, prev)  da_iterator_next(sizeof(type), (da_array*) array, prev)

#endif
