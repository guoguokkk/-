#ifndef MEMORYALLOC_H_
#define MEMORYALLOC_H_

#ifndef _WIN32
#include <stddef.h> //size_t
#include<new>
#endif // !_WIN32

void* operator new(size_t size);
void* operator new[](size_t size);

void operator delete(void* p) noexcept;
void operator delete[](void* p) noexcept;

void* memAlloc(size_t size);
void memFree(void* p);

#endif // !MEMORYALLOC_H_
