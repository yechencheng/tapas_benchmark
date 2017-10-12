#include <malloc.h>
#include <stdio.h>
#include <dlfcn.h>

extern "C" void *__libc_malloc(size_t size);

static bool malloc_hook_on = true;

static int malloc_idx = 0;

extern "C" void* malloc(size_t size){
    void *rt = __libc_malloc(size);
    malloc_idx += malloc_hook_on;
    if(malloc_hook_on && size >= (1<<20)){
        malloc_hook_on=false;
        printf("%d %d\n", malloc_idx, (int)size);
        malloc_hook_on=true;
    }
    return rt; 
 }
