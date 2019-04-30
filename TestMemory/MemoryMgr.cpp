#include "MemoryMgr.h"
#include<assert.h>

#ifdef _DEBUG
#include<stdio.h>
#define xPrintf(...) printf(__VA_ARGS__)
#else
#define xPrintf(...)
#endif // _DEBUG

MemoryPool::MemoryPool()
{
	_pPoolBuf = nullptr;//内存池的位置
	_pHeader = nullptr;//第一个空闲内存块的位置
	size_t _nChunkSize = 0;//内存池中内存块的大小
	size_t _nChunkNum = 0;// 内存池中内存块的数目
	xPrintf("MemoryPool()\n");
}

MemoryPool::~MemoryPool()
{
	if (_pPoolBuf)
	{
		free(_pPoolBuf);
	}

	//池外申请的内存不主动释放
}

void MemoryPool::InitPool()
{
	xPrintf("_nChunkSize: %d,	_nChunkNum: %d\n", _nChunkSize,_nChunkNum);

	//避免重复初始化
	assert(_pPoolBuf == nullptr);
	
	//申请内存池内存
	_pPoolBuf = (char*)malloc(_nChunkNum * _nChunkSize);

	//初始化内存块

}

void* MemoryPool::allocInPool(size_t size)
{
	return nullptr;
}

void MemoryPool::freeInPool(void* pMem)
{
}

MemoryMgr& MemoryMgr::Instance()
{
	// TODO: 在此处插入 return 语句
}

void* MemoryMgr::allocMem(size_t size)
{
	return nullptr;
}

void MemoryMgr::freeMem(void* pMem)
{
}

void MemoryMgr::addRef(void* pMem)
{
}

void MemoryMgr::initMapToPool(int nBegin, int nEnd, MemoryPool* pPool)
{
}
