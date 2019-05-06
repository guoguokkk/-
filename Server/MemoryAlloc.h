#ifndef MEMORYALLOC_H_
#define MEMORYALLOC_H_

void* operator new(size_t size);
void* operator new[](size_t size);

void operator delete(void* p);
void operator delete[](void* p);

void* memAlloc(size_t size);
void memFree(void* p);

#endif // !MEMORYALLOC_H_
