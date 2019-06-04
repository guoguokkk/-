#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_
#include<stdlib.h>
#include<assert.h>
#include<mutex>
#define MAX_MEMORY_SIZE 256//最大内存单元的尺寸

#ifdef _DEBUG
#include<stdio.h>
#define xPrintf(...) printf(__VA_ARGS__)
#else
#define xPrintf(...) 
#endif // _DEBUG

class Chunk;//内存块
class MemoryPool;//内存池
class MemoryMgr;//内存池管理类

//内存块，链表存储空闲内存块
class Chunk
{
public:
	int _nID;//id
	int _nRef;//引用次数
	bool _bPool;//是否在内存池中
	MemoryPool* _pPool;//所属内存池
	Chunk* _pNext;//下一个块
};

//int size = sizeof(Chunk);

//内存池：内存块描述信息+数据-->内存块描述信息+数据
class MemoryPool
{
public:
	MemoryPool();
	~MemoryPool();
	void initObjectPool();//初始化内存池
	void* allocMemInPool(size_t size);//申请内存池中内存
	void freeMemInPool(void* pMem);//释放内存池中内存

protected:
	char* _pBuf;//内存池的位置
	Chunk* _pHeader;//第一个空闲块的位置
	size_t _nChunkSize;//内存块数据大小
	size_t _nChunkNum;//内存块数目
	std::mutex _mutex;
};

template<size_t nChunkSize, size_t nChunkNum>
class MyMemoryPool :public MemoryPool
{
public:
	MyMemoryPool()
	{
		const int n = sizeof(void*);
		_nChunkSize = (nChunkSize / n) * n + (nChunkSize % n == 0 ? 0 : n);//按指针大小对齐
		_nChunkNum = nChunkNum;
	}
};

//内存池管理类，单例模式，只需要生成一个对象去管理，不需要生成很多管理类
class MemoryMgr
{
public:
	static MemoryMgr& instance();//获取该类的对象，单例模式
	void* allocMem(size_t size);//申请内存，提供给外部的接口
	void freeMem(void* pMem);//释放内存，提供给外部的接口	
	void addRef(void* pMem);

private:
	MemoryMgr();
	~MemoryMgr();
	void init_szAlloc(int nBegin, int nEnd, MemoryPool* pPool);//初始化内存池映射数组

private:
	MyMemoryPool<60, 1000000> _mem64;
	MyMemoryPool<128, 1000000> _mem128;
	MyMemoryPool<256, 1000000> _mem256;
	MemoryPool* _szAlloc[MAX_MEMORY_SIZE + 1];//内存池映射数组，根据用户输入的大小直接定位到内存池
};

#endif // !MEMORYPOOL_H_

