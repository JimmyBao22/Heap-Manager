#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "panic.h"

volatile long mCount = 0;
volatile long fCount = 0;

bool initial_heap_called = false;

long get_actual_heap_size() {
    return heap_size / sizeof(long);
}

// Since a long is 8 bytes, need to align accordingly
size_t align_by_16(size_t bytes) {
    bytes = ((bytes - 1) | 15) + 1;
    bytes /= 8;

    return bytes;
}

void print_my_heap() {
    printf("total heap size: %ld\n", get_actual_heap_size());
    for (size_t i = 0; i < get_actual_heap_size(); i++) {
        printf("%ld ", the_heap[i]);
    }
    printf("\n");
}

void print_heap_2(size_t best_fit_index, long best_fit_size) {
    print_my_heap();
    printf("best fit index: %ld, best fit size: %ld\n", best_fit_index, best_fit_size);
}

void print_heap_3(size_t best_fit_index, long best_fit_size, long space_left_over, size_t left_over_index) {
    print_heap_2(best_fit_index, best_fit_size);
    printf("space left over: %ld, left over index: %ld\n", space_left_over, left_over_index);
}

void initialize_heap(long actual_heap_size) {
    the_heap[0] = actual_heap_size - 2;
    the_heap[1] = -1;
}

// helper function to help split the the current space into multiple pieces: an allocated space and a space that remains free to use
void split_heap(long best_fit_size, size_t best_fit_index, size_t bytes, long actual_heap_size) {
    long total_left_over = best_fit_size - bytes;
    long space_left_over = total_left_over - 2;

    size_t left_over_index = best_fit_index + bytes + 2;

    // allocate the current space
    the_heap[best_fit_index] = -bytes;

    // set the rest of the space to still be freed
    if (left_over_index + 1 < actual_heap_size && space_left_over > -1) {
        the_heap[left_over_index] = space_left_over;
        the_heap[left_over_index + 1] = (long)(&the_heap[best_fit_index]);

        // set the pointer after this leftover space to point to this leftover space
        if (left_over_index + space_left_over + 3 < actual_heap_size) {
            the_heap[left_over_index + space_left_over + 3] = (long)(&the_heap[left_over_index]);
        }
    }
}

void* my_malloc(size_t bytes) {
    long actual_heap_size = get_actual_heap_size();

    if (!initial_heap_called) {
        // this is the first malloc call

        if (actual_heap_size < 2) {
            // this heap is too small
            return NULL;
        }

        initialize_heap(actual_heap_size);
        initial_heap_called = true;
    }

    mCount += 1;

    if (bytes == 0 || bytes >= (size_t)(heap_size) - 2) {
        return NULL;
    }

    bytes = align_by_16(bytes);

    // this implementation uses best fit, which finds the first index and size of the smallest free space
        // within the heap that can fit this malloc space
    size_t best_fit_index = -1;
    long best_fit_size = -1;
    for (size_t i = 0; i < actual_heap_size; i++) {
        long current_location_size = the_heap[i];
        if (current_location_size > 0 && current_location_size >= bytes) {
            if (best_fit_size == -1 || current_location_size < best_fit_size) {
                best_fit_size = current_location_size;
                best_fit_index = i;
            }
        }

        // small optimization: if the best fit size is the same as the bytes, don't need to keep searching
        if (best_fit_size == bytes) {
            break;
        }

        i += (abs(current_location_size) + 1);
    }

    if (best_fit_index == -1) {
        // panic("\ncannot malloc anymore, out of space\n");
        return NULL;
    }

    split_heap(best_fit_size, best_fit_index, bytes, actual_heap_size);

    return &the_heap[best_fit_index + 2];
}

void my_free(void* p) {
    fCount += 1;

    if (p == NULL) {
        return;
    }

    long actual_heap_size = get_actual_heap_size();

    // subtract the pointer with the beginning of the array to find the index it's pointed at
    long* ptr = (long*) p;
    size_t start_index = ptr - &the_heap[0] - 2;

    long current_total_space = abs(the_heap[start_index]);
    long previous_space_address = the_heap[start_index + 1];
    size_t end_index = start_index + current_total_space + 1;

    for (size_t i = start_index + 2; i <= end_index; i++) {
        the_heap[i] = 0;
    }

    if (end_index + 1 < actual_heap_size && the_heap[end_index + 1] >= 0) {
        // can combine the freed space with the right side
        long right_side_space = the_heap[end_index + 1];
        the_heap[end_index + 1] = 0;
        the_heap[end_index + 2] = 0;
        end_index += right_side_space + 2;
        current_total_space += right_side_space + 2;
    }

    if (start_index >= 2) {
        long previous_space_index = (previous_space_address - (long)(&the_heap[0])) / sizeof(long);
        long left_side_space = the_heap[previous_space_index];

        if (left_side_space >= 0) {
            // can combine the freed space with the left side;
            previous_space_address = the_heap[previous_space_index + 1];
            
            the_heap[start_index] = 0;
            the_heap[start_index + 1] = 0;

            start_index -= (left_side_space + 2);
            current_total_space += left_side_space + 2;
        }
    }

    the_heap[start_index] = current_total_space;
    the_heap[start_index + 1] = previous_space_address;

    // set the memory address of the next space in heap to this current pointer
    if (end_index + 2 < actual_heap_size) the_heap[end_index + 2] = (long)(&the_heap[start_index]);
}