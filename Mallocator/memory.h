#ifndef MEMORY_H
#define MEMORY_H

#define PAGE_SIZE 4096
#define MAX_SIZE (1024*1024)
#define REGION_SIZE sizeof(Region)
#define SLAB_SIZE sizeof(Slab)

// Caches are:
// C[0]: 16, C[1]: 32, C[2]: 64, C[3]: 128, C[4]: 256, C[5]: 512, C[6]: 1024, C[7]: 2048


typedef struct __Region {
  int size;
  void *addr;
  struct __Region *next;
} Region;


typedef struct __Slab {
  int size;
  int slots;
  int slot_size;
  int bitmap_size;
  char *bitmap;
  void *addr;
  struct __Slab * next;
} Slab;



typedef struct __Cache {
  int size;
  Slab *S;
} Cache;


typedef struct __Memory {
  Region *R;
  Cache C[8];
} Memory;



#endif // MEMORY_H
