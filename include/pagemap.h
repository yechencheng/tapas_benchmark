#pragma once

#include <stdio.h>

#define RECORD_PAGEMAP 1

static FILE* pagemap_fp;

// TODO
//static void check_error(int status) {
//}

void open_pagemap_file(const char* filename) {
    pagemap_fp = fopen(filename, "a");
}

void close_pagemap_file() {
    fclose(pagemap_fp);
}

void record_pagemap(const char* name, void* base, size_t size, unsigned long units, size_t unit) {

#ifdef RECORD_PAGEMAP
    unsigned long i;
    unsigned long PAGE_SIZE = sysconf(_SC_PAGESIZE);

    #define PAGE_BASE(x) (((x) / PAGE_SIZE) * PAGE_SIZE)
    unsigned long long pbase = PAGE_BASE((unsigned long long)base);
    unsigned long long pend  = PAGE_BASE((unsigned long long)(base + size));
    void *p;
    unsigned long pcount = (pend - pbase) / PAGE_SIZE + 1;
    void** ptr = (void**)malloc(pcount*sizeof(void*));
    int*   loc = (int*)malloc(pcount*sizeof(int));
    memset(loc, -1, pcount*sizeof(int));
    unsigned long idx = 0;

    for(p = base, idx = 0; p < pend; p += PAGE_SIZE) {
        ptr[idx++] = p;
    }

    if(move_pages(0, pcount, ptr, NULL, loc, 0))
        perror("error : ");

    int portion = pcount / units;
    for(idx = 0; idx < units; ++idx) {
        int si = idx * portion;
        int ei = (idx + 1) * portion;
        fprintf(pagemap_fp, "%s", name);
        if ( portion == 0 ) {
            fprintf(pagemap_fp, "\t%d", loc[0]/2);
        }
        else {
            for(i = si; i < ei; ++i) {
                fprintf(pagemap_fp, "\t%d", loc[i]/2);
                //check_error(loc[i]);
            }
        }
        fprintf(pagemap_fp, "\n");
    }

    free(ptr);
    free(loc);
#endif
}

