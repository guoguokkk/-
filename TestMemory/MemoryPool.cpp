#include "MemoryPool.h"
#include<assert.h>

MemoryPool::MemoryPool()
{	
	_pBuf = nullptr;//内存池的位置
	_pHeader = nullptr;//第一个空闲块的位置
	_nChunkSize = 0;//内存块大小
	_nChunkNum = 0;//内存块数目
	xPrintf("MemoryPool...\n");
}

MemoryPool::~MemoryPool()
{
	if (_pBuf != nullptr)
	{
		free(_pBuf);
	}
}

//初始化内存池
void MemoryPool::initPool()
{
	xPrintf("initPool: _nChunkSize=%d, _nChunkNum=%d\n", _nChunkSize, _nChunkNum);

	//避免重复初始化
	assert(_pBuf == nullptr);//不满足条件，就抛出错误
	if (_pBuf != nullptr)
	{
		return;
	}

	//向系统申请池的内存
	size_t realSize = _nChunkSize + sizeof(Chunk); //内存块描述信息 + 数据
	_pBuf = (char*)malloc(_nChunkNum * realSize);

	//初始化每个块
	_pHeader = (Chunk*)_pBuf;
	_pHeader->_nID = 0;
	_pHeader->_nRef = 0;
	_pHeader->_bPool = true;
	_pHeader->_pPool = this;
	_pHeader->_pNext = nullptr;

	Chunk* pCurChunk = _pHeader;
	for (size_t i = 1; i < _nChunkNum; ++i)
	{
		Chunk* pTempChunk = (Chunk*)(_pBuf + realSize * i);

		pTempChunk->_nID = i;
		pTempChunk->_nRef = 0;
		pTempChunk->_bPool = true;
		pTempChunk->_pPool = this;
		pTempChunk->_pNext = nullptr;

		pCurChunk->_pNext = pTempChunk;
		pCurChunk = pCurChunk->_pNext;
	}
}

void* MemoryPool::allocMemInPool(size_t size)
{
	std::lock_guard<std::mutex> lg(_mutex);
	//避免重复初始化
	if (_pBuf == nullptr)
	{
		initPool();
	}

	//分配内存
	Chunk* pReturn = nullptr;
	if (_pHeader == nullptr)
	{//内存池没有空闲块，向系统申请内存，内存块描述信息+数据
		pReturn = (Chunk*)malloc(sizeof(Chunk) + size);
		pReturn->_nID = -1;
		pReturn->_nRef = 1;
		pReturn->_bPool = false;
		pReturn->_pPool = nullptr;
		pReturn->_pNext = nullptr;
	}
	else
	{//将空闲块链表的第一个块分配出去
		pReturn = _pHeader;
		_pHeader = _pHeader->_pNext;
		assert(0 == pReturn->_nRef);
		pReturn->_nRef = 1;//更新引用的次数
	}
	xPrintf("allocMem: %p, id=%d, size=%d\n", pReturn, pReturn->_nID, size);

	//内存池：内存块描述信息+数据-->内存块描述信息+数据	
	return ((char*)pReturn + sizeof(Chunk));//输出：数据的地址
}

//输入：数据的地址
void MemoryPool::freeMemInPool(void* pMem)
{
	//内存池：内存块描述信息+数据-->内存块描述信息+数据
	Chunk* pTempChunk = (Chunk*)((char*)pMem - sizeof(Chunk));
	assert(pTempChunk->_nRef == 1);

	if (pTempChunk->_bPool == false)
	{//不在池中
		if (--pTempChunk->_nRef != 0)
		{
			return;
		}
		free(pTempChunk);
	}
	else
	{//在池中
		std::lock_guard<std::mutex> lg(_mutex);
		if (--pTempChunk->_nRef != 0)
		{
			return;
		}
		pTempChunk->_pNext = _pHeader;
		_pHeader = pTempChunk;
	}
}

MemoryMgr::MemoryMgr()
{
	init_szAlloc(0, 64, &_mem64);
	init_szAlloc(65, 128, &_mem128);
	init_szAlloc(129, 256, &_mem256);
}

MemoryMgr::~MemoryMgr()
{
}

void MemoryMgr::init_szAlloc(int nBegin, int nEnd, MemoryPool * pPool)
{
	for (int i = nBegin; i <= nEnd; ++i)
	{
		_szAlloc[i] = pPool;
	}
}

MemoryMgr& MemoryMgr::instance()
{
	static MemoryMgr mgr;
	return mgr;
}

void* MemoryMgr::allocMem(size_t size)
{
	Chunk* pReturn = nullptr;

	if (size > MAX_MEMORY_SIZE)
	{//大于最大的内存池，在系统中分配一个块		
		pReturn = (Chunk*)malloc(sizeof(Chunk) + size);
		pReturn->_nID = -1;
		pReturn->_nRef = 1;
		pReturn->_bPool = false;
		pReturn->_pPool = nullptr;
		pReturn->_pNext = nullptr;
		/*xPrintf("allocMem: %p, id=%d, size=%d\n",
			pReturn, pReturn->_nID, size);*/
		return ((char*)pReturn + sizeof(Chunk));
	}
	else
	{//小于等于最大的内存池，在合适的内存池中分配内存
		return _szAlloc[size]->allocMemInPool(size);
	}
}

void MemoryMgr::freeMem(void* pMem)
{
	Chunk* pTempChunk = (Chunk*)((char*)pMem - sizeof(Chunk));
	xPrintf("freeMem: %p, id=%d\n", pTempChunk, pTempChunk->_nID);

	if (pTempChunk->_bPool == false)
	{//不在池中
		if (--pTempChunk->_nRef == 0)
		{
			free(pTempChunk);
		}
	}
	else
	{//在池中
		pTempChunk->_pPool->freeMemInPool(pMem);
	}
}

void MemoryMgr::addRef(void* pMem)
{
	Chunk* pTempChunk = (Chunk*)((char*)pMem - sizeof(Chunk));
	++pTempChunk->_nRef;
}
