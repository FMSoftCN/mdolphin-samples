
#ifndef __FT_MALLOC_H__
#define __FT_MALLOC_H__

void* ft_malloc(size_t size);
void* ft_calloc(size_t num, size_t size);
void* ft_realloc(void* ptr, size_t size);
void ft_free(void* ptr);

#define SAFE_MALLOC_RETURNABLE(result, type, fail_return)\
do { \
    result = (type *)ft_malloc(sizeof(type)); \
    if(!(result)) { \
        errno = ENOMEM; \
        return fail_return; \
    } \
}while(0)

#endif // __FT_MALLOC_H__
