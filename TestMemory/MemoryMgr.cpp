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
	_pPoolBuf = nullptr;//�ڴ�ص�λ��
	_pHeader = nullptr;//��һ�������ڴ���λ��
	size_t _nChunkSize = 0;//�ڴ�����ڴ��Ĵ�С
	size_t _nChunkNum = 0;// �ڴ�����ڴ�����Ŀ
	xPrintf("MemoryPool()\n");
}

MemoryPool::~MemoryPool()
{
	if (_pPoolBuf)
	{
		free(_pPoolBuf);
	}

	//����������ڴ治�����ͷ�
}

void MemoryPool::InitPool()
{
	xPrintf("_nChunkSize: %d,	_nChunkNum: %d\n", _nChunkSize,_nChunkNum);

	//�����ظ���ʼ��
	assert(_pPoolBuf == nullptr);
	
	//�����ڴ���ڴ�
	_pPoolBuf = (char*)malloc(_nChunkNum * _nChunkSize);

	//��ʼ���ڴ��

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
	// TODO: �ڴ˴����� return ���
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
