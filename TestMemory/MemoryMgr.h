#ifndef MEMORYMGR_H_
#define MEMORYMGR_H_
#define MAX_MEMORY_SIZE 128//支持的最大内存块的大小

#include<mutex>

class MemoryPool;

//内存块
class MemoryChunk
{
public:
	int _nID;//内存块编号
	int _nRef;//内存块引用次数
	MemoryPool* _pPool;//所属的内存池
	MemoryChunk* _pNext;//下一个内存块
	bool _pPool;//是否在内存池中

private:
	//预留
	char c1;
	char c2;
	char c3;
};

//内存池，由内存块组成，链式存储（块——>块——>块）
class MemoryPool
{
public:
	MemoryPool();
	~MemoryPool();
	void InitPool();
	void* allocInPool(size_t size);//申请内存	
	void freeInPool(void* pMem);//释放内存

private:
	char* _pPoolBuf;//内存池的位置
	MemoryChunk* _pHeader;//第一个空闲内存块的位置
	size_t _nChunkSize;//内存池中内存块的大小
	size_t _nChunkNum;// 内存池中内存块的数目
	std::mutex _mutex;

};

template<size_t nChunkSize, size_t nChunkNum>
class MyMemoryPool :public MemoryPool
{
public:
	MyMemoryPool()
	{
		//强制补齐，按指针大小对齐
		const size_t n = sizeof(void*);
		_nChunkSize = (nChunkSize / n) * n + (nChunkSize % n == 0 ? 0 : n);
		_nChunkNum = nChunkNum;
	}
};

//内存池管理类
class MemoryMgr
{
public:
	static MemoryMgr& Instance();//单例模式
	void* allocMem(size_t size);//申请内存
	void freeMem(void* pMem);//释放内存
	void addRef(void* pMem);//增加对内存块的引用

private:
	MemoryMgr();
	~MemoryMgr() = default;
	void initMapToPool(int nBegin, int nEnd, MemoryPool* pPool);//初始化内存池映射数组

private:
	MyMemoryPool<64, 100000> _mem64;//10个内存块，块大小为64字节的内存池
	MyMemoryPool<128, 100000> _mem128;//10个内存块，块大小为128字节的内存池
	MemoryPool* _mapToPool[MAX_MEMORY_SIZE + 1];//内存池映射数组，无需查找，直接映射，1-64，保存每个内存池

};

#endif // !MEMORYMGR_H_
