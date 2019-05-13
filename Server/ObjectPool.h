#ifndef OBJECTPOOL_H_
#define OBJECTPOOL_H_
#include"MemoryAlloc.h"
#include<assert.h>
#include<mutex>

//类的类型和对象池中对象节点的数目
template<typename Type, size_t nNodeNum>
class ObjectPool
{
private:
	struct objectNode
	{
		int nID;//对象节点的编号
		char nRef;//对象节点的引用次数
		bool bPool;//是否在对象池中
		objectNode* pNext;//下一个节点
	};

public:
	ObjectPool()
	{
		_pBuf = nullptr;
		_pHeader = nullptr;
		initObjectPool();
	}

	~ObjectPool()
	{
		if (_pBuf)
		{
			delete[] _pBuf;
		}
	}

	void* allocInPool(size_t size)
	{
		std::lock_guard<std::mutex> lg(_mutex);
		objectNode* pReturn = nullptr;

		//判断是否有空闲对象节点
		if (_pHeader != nullptr)
		{
			pReturn = _pHeader;
			_pHeader = _pHeader->pNext;
			assert(pReturn->nRef == 0);
			pReturn->nRef = 1;
		}
		else
		{
			size_t nNodeSize = sizeof(Type) + sizeof(objectNode);//一个节点的大小：数据+头部描述信息
			pReturn = (objectNode*)new char[nNodeSize];
			pReturn->nID = -1;
			pReturn->nRef = 1;
			pReturn->bPool = false;
			pReturn->pNext = nullptr;
		}

		//printf("pReturn=%p, pReturn->nID=%d, nNodeNum=%d\n", pReturn, pReturn->nID, nNodeNum);
		return ((char*)pReturn + sizeof(objectNode));
	}

	void freeInPool(void* p)
	{
		objectNode* pNode = (objectNode*)((char*)p - sizeof(objectNode));
		assert(pNode->nRef == 1);

		//判断是否在对象池中
		if (pNode->bPool == true)
		{
			std::lock_guard<std::mutex> lg(_mutex);
			if (--pNode->nRef != 0) { return; }
			pNode->pNext = _pHeader;
			_pHeader = pNode;
		}
		else
		{
			if (--pNode->nRef != 0) { return; }
			delete[] pNode;
		}
	}

private:
	void initObjectPool()
	{
		printf("initObjectPool...\n");
		//避免重复初始化
		assert(_pBuf == nullptr);
		if (_pBuf != nullptr) { return; }

		size_t nNodeSize = sizeof(Type) + sizeof(objectNode);//一个节点的大小：数据+头部描述信息
		size_t n = nNodeSize * nNodeNum;
		_pBuf = new char[n];
		_pHeader = (objectNode*)_pBuf;
		_pHeader->nID = 0;
		_pHeader->nRef = 0;
		_pHeader->bPool = true;
		_pHeader->pNext = nullptr;

		objectNode* preNode = _pHeader;
		for (int i = 1; i < nNodeNum; ++i)
		{
			objectNode* pNode = (objectNode*)(_pBuf + i * nNodeSize);
			pNode->nID = i;
			pNode->nRef = 0;
			pNode->bPool = true;
			pNode->pNext = nullptr;

			preNode->pNext = pNode;
			preNode = pNode;
		}
	}

private:
	char* _pBuf;//对象池地址
	objectNode* _pHeader;//空闲对象节点的头部
	std::mutex _mutex;
};

template<typename Type, size_t nNodeNum>
class ObjectPoolBase
{
public:
	void* operator new(size_t size)
	{
		return getObjectPool().allocInPool(size);
	}

	void operator delete(void* p)
	{
		getObjectPool().freeInPool(p);
	}

	template<typename...Args>
	static Type* createObject(Args...args)
	{
		Type* obj = new Type(args...);
		return obj;
	}

	static void destroyObject(Type* obj)
	{
		delete obj;
	}

private:
	static ObjectPool<Type, nNodeNum>& getObjectPool()
	{
		static ObjectPool <Type, nNodeNum> objectPool;//一定要是static，不然会访问冲突
		return objectPool;
	}
};

#endif // !OBJECTPOOL_H_

