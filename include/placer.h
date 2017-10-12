#pragma once

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <numaif.h>
#include <numa.h>
#include <stdint.h>

FILE* place_fp;

#define BIND_DATA 1

unsigned long PAGE_SIZE;
#define PAGE_BASE(x) (((x) / PAGE_SIZE) * PAGE_SIZE)

struct address_info {
    void* ptr;
    char id[128];
    size_t unit;
};

struct address_name_map {
    struct address_info data[64];
    unsigned long size;
} address_names;

void open_place_file() {
    char *dp = getenv("DP");
    if(!dp) dp = "place";
    place_fp = fopen(dp, "r");
    if ( place_fp == NULL ) {
        fprintf(stderr, "cannot open place file\n");
    }else{
        fprintf(stderr, "place file opened\n");
    }
}

void close_place_file() {
    if(place_fp)
    fclose(place_fp);
}

void register_named_address(const char* id, void* base, size_t unit) {
    address_names.data[address_names.size].ptr = base;
    strncpy(address_names.data[address_names.size].id, id, 128);
    address_names.data[address_names.size].unit = unit;
    address_names.size++;
}

void place_address_range() {
    unsigned long i;
    char buf[2048];
    char id[2048];

#ifdef BIND_DATA
    printf("Data binding enabled\n");
#endif
    if(!place_fp){
        fprintf(stderr, "No data place file found\n");
        return;
    }

    int lines = 0;
    PAGE_SIZE = sysconf(_SC_PAGESIZE);
    while(fgets(buf, 2048, place_fp)) {
        unsigned long start_idx, end_idx;
        int node;
        sscanf(buf, "%s%lu%lu%d", id, &start_idx, &end_idx, &node);
        for(i = 0; i < address_names.size; ++i) {
            if(0 == strcmp(address_names.data[i].id, id)) {
                unsigned long long base = PAGE_BASE((unsigned long long)address_names.data[i].ptr + start_idx * address_names.data[i].unit);
                unsigned long long end  = PAGE_BASE((unsigned long long)address_names.data[i].ptr + end_idx * address_names.data[i].unit);
                unsigned long size = ((unsigned long)(end - base)) / PAGE_SIZE;
                if ( size != 0 ) {
                    node = node>>1<<1;

                    struct bitmask *nodes;
                    nodes = numa_allocate_nodemask();
                    numa_bitmask_setbit(nodes, node);
                    //assert((((size_t)base)%PAGE_SIZE) == 0);
#ifdef BIND_DATA
                    //printf("bind %s [%p, %p] to node %d\n", id, (void*)base, (void*)(base + size * PAGE_SIZE), node);
                    if (mbind((void*)base, size * PAGE_SIZE, 
                                MPOL_BIND, 
                                nodes->maskp, nodes->size + 1, 
                                MPOL_MF_MOVE|MPOL_MF_STRICT)) {
                        perror("err: ");
                    }
                    numa_bitmask_free(nodes);
#endif
                }
            }
        }
    }
}
