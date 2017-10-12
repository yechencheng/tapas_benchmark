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

struct mem_region{
    char id[128];
    unsigned long start, end;
    int node;
};

struct mem_place{
    struct mem_region data[8192];
    size_t size;
}regions;

void open_place_file() {
    char *dp = getenv("DP");
    if(!dp) dp = "place";
    place_fp = fopen(dp, "r");
    if ( place_fp == NULL ) {
        fprintf(stderr, "cannot open place file\n");
    }else{
        fprintf(stderr, "place file opened\n");
    }
    PAGE_SIZE = sysconf(_SC_PAGESIZE);

    char buf[2048];
    char id[2048];
    while(fgets(buf, 2048, place_fp)) {
        unsigned long start_idx, end_idx;
        int node;
        sscanf(buf, "%s%lu%lu%d", id, &start_idx, &end_idx, &node);
        int idx = regions.size;
        strncpy(regions.data[idx].id, id, 128);
        regions.data[idx].start = start_idx;
        regions.data[idx].end = end_idx;
        regions.data[idx].node = node;
        regions.size++;
    }
    if(place_fp)
        fclose(place_fp);
}

void register_named_address(const char* id, void* base, size_t unit) {
    for(int i = 0; i < regions.size; i++){
        if(strcmp(regions.data[i].id, id)) continue;
        unsigned long long base = PAGE_BASE(base + regions.data[i].start * unit);
        unsigned long long end = PAGE_BASE(base + regions.data[i].end * unit);
        unsigned long size = ((unsigned long)(end - base)) / PAGE_SIZE;
        struct bitmask *nodes;
        nodes = numa_allocate_nodemask();
        numa_bitmask_setbit(nodes, regions.data[i].node);
        if (mbind((void*)base, size * PAGE_SIZE, MPOL_BIND, nodes->maskp, nodes->size + 1, MPOL_MF_MOVE|MPOL_MF_STRICT)){
            perror("err: ");
        }
        numa_bitmask_free(nodes);
    }
}



FILE* roi_fp;

void open_address_range_file() {
    roi_fp = fopen("address", "w+");
}

void close_address_range_file() {
    fclose(roi_fp);
}

void register_address_range(const char* name, void* base, size_t size, unsigned long units, size_t unit) {
    unsigned long i;
    //printf("in entry, %p, %u, %u\n", base, size, units);
    for(i = 0; i < units; ++i) {
        fprintf(roi_fp, "%s\t%p\t%p\t%lu\t%lu\n", name,
                                base + size / units * i, 
                                base + size / units * (i+1),
                                (size / unit) / units * i,
                                (size / unit) / units * (i + 1)
               );
    }
}

void __attribute__ ((noinline)) roi_begin() {
   printf("ROI begins\n");
}

void __attribute__ ((noinline)) roi_end() {
   printf("ROI ends\n");
}
