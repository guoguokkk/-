#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_
#include<stdlib.h>
#include<assert.h>
#include<mutex>
#define MAX_MEMORY_SIZE 256//����ڴ浥Ԫ�ĳߴ�

#ifdef _DEBUG
#include<stdio.h>
#define xPrintf(...) printf(__VA_ARGS__)
#else
#define xPrintf(...) 
#endif // _DEBUG

class Chunk;//�ڴ��
class MemoryPool;//�ڴ��
class MemoryMgr;//�ڴ�ع�����

//�ڴ�飬����洢�����ڴ��
class Chunk
{
public:
	int _nID;//id
	int _nRef;//���ô���
	bool _bPool;//�Ƿ����ڴ����
	MemoryPool* _pPool;//�����ڴ��
	Chunk* _pNext;//��һ����
};

//int size = sizeof(Chunk);

//�ڴ�أ��ڴ��������Ϣ+����-->�ڴ��������Ϣ+����
class MemoryPool
{
public:
	MemoryPool();
	~MemoryPool();
	void initPool();//��ʼ���ڴ��
	void* allocMemInPool(size_t size);//�����ڴ�����ڴ�
	void freeMemInPool(void* pMem);//�ͷ��ڴ�����ڴ�

protected:
	char* _pBuf;//�ڴ�ص�λ��
	Chunk* _pHeader;//��һ�����п��λ��
	size_t _nChunkSize;//�ڴ�����ݴ�С
	size_t _nChunkNum;//�ڴ����Ŀ
	std::mutex _mutex;
};

template<size_t nChunkSize, size_t nChunkNum>
class MyMemoryPool :public MemoryPool
{
public:
	MyMemoryPool()
	{
		const int n = sizeof(void*);
		_nChunkSize = (nChunkSize / n) * n + (nChunkSize % n == 0 ? 0 : n);//��ָ���С����
		_nChunkNum = nChunkNum;
	}
};

//�ڴ�ع����࣬����ģʽ��ֻ��Ҫ����һ������ȥ��������Ҫ���ɺܶ������
class MemoryMgr
{
public:
	static MemoryMgr& instance();//��ȡ����Ķ��󣬵���ģʽ
	void* allocMem(size_t size);//�����ڴ棬�ṩ���ⲿ�Ľӿ�
	void freeMem(void* pMem);//�ͷ��ڴ棬�ṩ���ⲿ�Ľӿ�	
	void addRef(void* pMem);

private:
	MemoryMgr();
	~MemoryMgr();
	void init_szAlloc(int nBegin, int nEnd, MemoryPool* pPool);//��ʼ���ڴ��ӳ������

private:
	MyMemoryPool<60, 1000000> _mem64;
	MyMemoryPool<128, 1000000> _mem128;
	MyMemoryPool<256, 1000000> _mem256;
	MemoryPool* _szAlloc[MAX_MEMORY_SIZE + 1];//�ڴ��ӳ�����飬�����û�����Ĵ�Сֱ�Ӷ�λ���ڴ��
};

#endif // !MEMORYPOOL_H_

