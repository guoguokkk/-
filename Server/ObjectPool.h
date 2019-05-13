#ifndef OBJECTPOOL_H_
#define OBJECTPOOL_H_
#include"MemoryAlloc.h"
#include<assert.h>
#include<mutex>

//������ͺͶ�����ж���ڵ����Ŀ
template<typename Type, size_t nNodeNum>
class ObjectPool
{
private:
	struct objectNode
	{
		int nID;//����ڵ�ı��
		char nRef;//����ڵ�����ô���
		bool bPool;//�Ƿ��ڶ������
		objectNode* pNext;//��һ���ڵ�
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

		//�ж��Ƿ��п��ж���ڵ�
		if (_pHeader != nullptr)
		{
			pReturn = _pHeader;
			_pHeader = _pHeader->pNext;
			assert(pReturn->nRef == 0);
			pReturn->nRef = 1;
		}
		else
		{
			size_t nNodeSize = sizeof(Type) + sizeof(objectNode);//һ���ڵ�Ĵ�С������+ͷ��������Ϣ
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

		//�ж��Ƿ��ڶ������
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
		//�����ظ���ʼ��
		assert(_pBuf == nullptr);
		if (_pBuf != nullptr) { return; }

		size_t nNodeSize = sizeof(Type) + sizeof(objectNode);//һ���ڵ�Ĵ�С������+ͷ��������Ϣ
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
	char* _pBuf;//����ص�ַ
	objectNode* _pHeader;//���ж���ڵ��ͷ��
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
		static ObjectPool <Type, nNodeNum> objectPool;//һ��Ҫ��static����Ȼ����ʳ�ͻ
		return objectPool;
	}
};

#endif // !OBJECTPOOL_H_

