#include "MemoryAlloc.h"
#include"MemoryPool.h"
#include<memory>

void* operator new(size_t size)
{
	return MemoryMgr::instance().allocMem(size);
}

void* operator new[](size_t size)
{
	return MemoryMgr::instance().allocMem(size);
}

void operator delete(void* p)
{
	return MemoryMgr::instance().freeMem(p);
}

void operator delete[](void* p)
{
	return MemoryMgr::instance().freeMem(p);
}

void* memAlloc(size_t size)
{
	return malloc(size);
}

void memFree(void* p)
{
	free(p);
}
