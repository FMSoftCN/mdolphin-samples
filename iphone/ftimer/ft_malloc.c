
#include <malloc.h>

#ifdef __cplusplus
extern "C"{
#endif

void* ft_malloc(size_t size)
{
    return malloc(size);
}

void* ft_calloc(size_t num, size_t size)
{
    return calloc(num, size);
}

void* ft_realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

void ft_free(void* ptr)
{
    free(ptr);
}

#ifdef __cplusplus
}
#endif

