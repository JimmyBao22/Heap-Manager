#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"


#define HEAP_SIZE 512

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

const long DEBUG = 0; // turn this to 1 if you want debugging
void print_heap() {
    if (DEBUG) {
        printf("===== THE HEAP =====\n");
        for (size_t i = 0; i < HEAP_SIZE / sizeof(long); i++) {
            printf("%p: %ld\n", &the_heap[i], the_heap[i]);
        }
    }
}

#define NUM_PTRS 5
void *last[NUM_PTRS] = {};

void test_repeat() {
    // create 5 pointers and free 5 pointers
    // should result in original malloc'd state
    for (size_t i = 0; i < NUM_PTRS; i++) {
        last[i] = malloc(1);
    }

    print_heap();

    for (size_t i = 0; i < NUM_PTRS; i++) {
        free(last[i]);
    }

    print_heap();
}

void test_null_free() {
    void* ptr = malloc(-1);
    if(ptr) {
        panic("pointer in test_null_free was not 0!\n");
    }
    free(ptr); // check that this doesn't segfault
}

int main() {
    // if this fails make sure that repeated malloc and free works
    for(size_t i = 0; i < 1000000; i++) {
        test_repeat();
    }

    test_null_free();

    printf("All Correct (AC)");
}