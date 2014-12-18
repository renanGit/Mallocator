Mallocator
==========

The Mighty Mallocator

The Invincible Mallocator!

The purpose of this assignment is to build a truly invincible memory allocator and win the performance invincibility contest! In your all-out attempt at winning this competition, you will build a hybrid memory allocator with at least 7 slab caches that serve small allocation requests and a best-effort large-region memory allocator, each dynamically requesting memory from the OS as needed and returning back to the OS when no longer needed. Figuring out how many caches provide optimal performance/space utilization is part of the challenge.

Memory allocators allow dynamically growing the heap memory of a process by requesting additional page table mappings (via mmap or similar syscalls), in effect making more of the address space of the process usable by the process. When the application requests memory allocation, the memory allocator gives out portions of this space. Similarly, when memory gets deallocated, the library should also return any excess memory back to the OS via munmap!

You will build such a memory allocator as a shared library that can be linked to and dynamically loaded into a running process. The basic routines your allocator should support and export to the application are:

void *allocate(unsigned int size): Allocates memory of size bytes and returns a pointer to it. Other details are set up so that it functions similar to malloc (see the man page).
void deallocate(void * addr): Frees the allocated memory, checking for appropriate error conditions. Other details are set up so that it functions similarly to free (see the man page)
You will need to implement an initialization and finalization routines for your memory allocator shared library. These will be invoked when the shared library is loaded/unloaded.

The memory allocator should consume physical memory in proportion to the needs of the program. In other words, it should request memory dynamically from the operating system. Requesting all memory from the OS ahead of time will incur substantial penalty. To request memory, the allocator should use mmap, mapping the "/dev/zero" virtual device file into the process address space of the calling process. (Refer to the sample memory allocator we built in class to work this into your solution.)

You are required to build your memory allocator as a shared library.

gcc -m32 -c -fpic memory.c
gcc -m32 -shared -o libmemory.so memory.o

The above command compiles with -m32 gcc option to produce a 32-bit binary. This is what we will be testing with. To link an arbitrary program with this library, specify the base name of the library with "-lmemory" and the path "-L." so that the linker can find the library.

gcc -m32 -lmemory -L. -o main main.c

Create a test sub-directory in your root directory that you will be turning in. Put your own test programs in this directory. Be sure to include the ones that prove your allocator is superior! Create a README for how to use your test programs and what input/output each test program expects and should generate. Include a Makefile to compile your test programs.

Here are some guidelines for the test suite:

Test 1: Make an allocation with a size of 32 bytes for a pointer of char* type.Then assign some value through the pointer and output them. After that deallocate it once.

Test 2: Make an allocation with a size of 32 bytes and deallocate it twice.(double free)

Test 3: Make allocations with sizes of 32 bytes,64 bytes,128 bytes,192 bytes, etc. as well as large requests (e.g., 16 pages) and deallocate them.

Test 4: Loop 20 times and in each loop make an allocation with a size of 32 bytes. When the loop is over, deallocate them.

Test 5: Create 10 threads and in each thread loop 100,000 times and in each loop make an allocation with a size of 256 bytes.Then deallocate.You should ensure that no two threads get the same address returned.
For example, you could use a global array of length 10 to keep track of the returned address:when thread n makes an allocation, addr[n] is assigned with the returned address. When thread n makes a deallocation, addr[n]=0. If after an allocation, addr[i]==addr[j] and i!=j and addr[i]!=0 and addr[j]!=0, there are two threads get the same address returned. 

Test 6: Continuously make allocations of a random size from 1 byte to 256 bytes and count the total number of bytes allocated until an allocation fails.Then output the result.

A link to the test suite is here.

WINNING THE INVINCIBILITY COMPETITION!

We will pit your allocator against all the allocators received from the class using a battery of tests that we write. Our goal is to evaluate efficiency of implementation (pre-allocation loses to on-demand), allocator/deallocator performance, and memory utilization. The winning team will get an additional 20% points towards their final grade.


WE WON ... 