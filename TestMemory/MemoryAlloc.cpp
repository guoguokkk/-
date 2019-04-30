#include "MemoryAlloc.h"

void* operator new(size_t size)
{
	return nullptr;
}

void* operator new[](size_t size)
{
	return nullptr;
}

void operator delete(void* p)
{
}

void operator delete[](void* p)
{
}

void* memAlloc(size_t size)
{
	return nullptr;
}

void memFree(void* p)
{
}
