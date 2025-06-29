# Heap-Manager - Custom Malloc/Free Implementation in C

This project is a simplified implementation of dynamic memory allocation functions (`malloc` and `free`) using only a fixed-size heap buffer. The allocator simulates how memory management works under the hood, helping to deepen understanding of pointers and memory in C.

## Constraints

- Only the memory defined by `the_heap` is available for use. Do not use `sbrk`, `mmap`, or system `malloc`.
- Keep track of metadata such as allocated blocks and their sizes.

## Variables Provided

- `heap_size`: Total size of the heap in bytes
- `the_heap`: The actual memory buffer to be used for allocation
- `mCount`: Number of times `malloc` is called
- `fCount`: Number of times `free` is called

## Testing

### Adding Tests
Adding testcase, create 2 files:

       <name>.c       contains the C program
       <name>.ok      contains the expected output

### To run tests:

    make test

### To make the output less noisy:

    make -s test

### To run one test

    make -s t0.test