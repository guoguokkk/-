#include "MemoryPool.h"
#include<assert.h>

MemoryPool::MemoryPool()
{	
	_pBuf = nullptr;//�ڴ�ص�λ��
	_pHeader = nullptr;//��һ�����п��λ��
	_nChunkSize = 0;//�ڴ���С
	_nChunkNum = 0;//�ڴ����Ŀ
	xPrintf("MemoryPool...\n");
}

MemoryPool::~MemoryPool()
{
	if (_pBuf != nullptr)
	{
		free(_pBuf);
	}
}

//��ʼ���ڴ��
void MemoryPool::initPool()
{
	xPrintf("initPool: _nChunkSize=%d, _nChunkNum=%d\n", _nChunkSize, _nChunkNum);

	//�����ظ���ʼ��
	assert(_pBuf == nullptr);//���������������׳�����
	if (_pBuf != nullptr)
	{
		return;
	}

	//��ϵͳ����ص��ڴ�
	size_t realSize = _nChunkSize + sizeof(Chunk); //�ڴ��������Ϣ + ����
	_pBuf = (char*)malloc(_nChunkNum * realSize);

	//��ʼ��ÿ����
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
	//�����ظ���ʼ��
	if (_pBuf == nullptr)
	{
		initPool();
	}

	//�����ڴ�
	Chunk* pReturn = nullptr;
	if (_pHeader == nullptr)
	{//�ڴ��û�п��п飬��ϵͳ�����ڴ棬�ڴ��������Ϣ+����
		pReturn = (Chunk*)malloc(sizeof(Chunk) + size);
		pReturn->_nID = -1;
		pReturn->_nRef = 1;
		pReturn->_bPool = false;
		pReturn->_pPool = nullptr;
		pReturn->_pNext = nullptr;
	}
	else
	{//�����п�����ĵ�һ��������ȥ
		pReturn = _pHeader;
		_pHeader = _pHeader->_pNext;
		assert(0 == pReturn->_nRef);
		pReturn->_nRef = 1;//�������õĴ���
	}
	xPrintf("allocMem: %p, id=%d, size=%d\n", pReturn, pReturn->_nID, size);

	//�ڴ�أ��ڴ��������Ϣ+����-->�ڴ��������Ϣ+����	
	return ((char*)pReturn + sizeof(Chunk));//��������ݵĵ�ַ
}

//���룺���ݵĵ�ַ
void MemoryPool::freeMemInPool(void* pMem)
{
	//�ڴ�أ��ڴ��������Ϣ+����-->�ڴ��������Ϣ+����
	Chunk* pTempChunk = (Chunk*)((char*)pMem - sizeof(Chunk));
	assert(pTempChunk->_nRef == 1);

	if (pTempChunk->_bPool == false)
	{//���ڳ���
		if (--pTempChunk->_nRef != 0)
		{
			return;
		}
		free(pTempChunk);
	}
	else
	{//�ڳ���
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
	{//���������ڴ�أ���ϵͳ�з���һ����		
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
	{//С�ڵ��������ڴ�أ��ں��ʵ��ڴ���з����ڴ�
		return _szAlloc[size]->allocMemInPool(size);
	}
}

void MemoryMgr::freeMem(void* pMem)
{
	Chunk* pTempChunk = (Chunk*)((char*)pMem - sizeof(Chunk));
	xPrintf("freeMem: %p, id=%d\n", pTempChunk, pTempChunk->_nID);

	if (pTempChunk->_bPool == false)
	{//���ڳ���
		if (--pTempChunk->_nRef == 0)
		{
			free(pTempChunk);
		}
	}
	else
	{//�ڳ���
		pTempChunk->_pPool->freeMemInPool(pMem);
	}
}

void MemoryMgr::addRef(void* pMem)
{
	Chunk* pTempChunk = (Chunk*)((char*)pMem - sizeof(Chunk));
	++pTempChunk->_nRef;
}
