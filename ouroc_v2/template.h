#ifndef TEMPLATE_H
#define TEMPLATE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(cond,...)                                                \
do{                                                                     \
    if(!(cond)){                                                        \
        fprintf(stderr,"%s,%d: ASSERTION FAILED: ",__FILE__,__LINE__);  \
        fprintf(stderr,__VA_ARGS__);                                    \
        fprintf(stderr,"\n");                                           \
        exit(1);                                                        \
    }                                                                   \
}while(0)

struct da_impl {
    size_t len;
    size_t elem_size;
    size_t capacity;
};

void da_init_impl(struct da_impl* impl,size_t elem_size){
    impl->len = 0;
    impl->elem_size = elem_size;
    impl->capacity = 0;
}

#define DA_TEMPLATE(t) struct {struct da_impl impl;t *data;}

#define DA_RESET(a) (a)->impl.len = 0
#define DA_LEN(a) (a)->impl.len

#define DA_INIT(a) da_init_impl(&(a)->impl,sizeof(*(a)->data))
#define DA_ALLOC(a,size)                            \
do{                                                 \
    (a)->data = malloc((size)*(a)->impl.elem_size); \
    (a)->impl.capacity = (size);                    \
}while(0)                                       

#define DA_FREE(a) free((a)->data)
#define DA_FREE_DATA(a)                         \
do{                                             \
    for(size_t i = 0 ; i < DA_LEN((a)) ; ++i){  \
        free((a)->data[i]);                     \
    }                                           \
}while(0)

#define INIT_DA_CAPACITY 10

#define DA_APPEND(a,value)                                                                  \
do{                                                                                         \
    if((a)->impl.len >= ((a)->impl.capacity)){                                              \
        (a)->impl.capacity = ((a)->impl.len == 0) ? INIT_DA_CAPACITY : (a)->impl.capacity*2;\
        (a)->data = realloc((a)->data,(a)->impl.capacity*(a)->impl.elem_size);              \
        ASSERT((a)->data != NULL,"ERROR: failed to reallocate memory for DA.\n");           \
    }                                                                                       \
    (a)->data[(a)->impl.len++] = (value);                                                   \
}while(0)

char* heap_string(const char* stack_string){
    size_t len = strlen(stack_string);
    char* str = malloc(sizeof(char)*(len+1));
    memcpy(str,stack_string,len+1);
    return str;
}

#endif
