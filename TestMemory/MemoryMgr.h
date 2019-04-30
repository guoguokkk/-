#ifndef MEMORYMGR_H_
#define MEMORYMGR_H_
#define MAX_MEMORY_SIZE 128//֧�ֵ�����ڴ��Ĵ�С

#include<mutex>

class MemoryPool;

//�ڴ��
class MemoryChunk
{
public:
	int _nID;//�ڴ����
	int _nRef;//�ڴ�����ô���
	MemoryPool* _pPool;//�������ڴ��
	MemoryChunk* _pNext;//��һ���ڴ��
	bool _pPool;//�Ƿ����ڴ����

private:
	//Ԥ��
	char c1;
	char c2;
	char c3;
};

//�ڴ�أ����ڴ����ɣ���ʽ�洢���顪��>�顪��>�飩
class MemoryPool
{
public:
	MemoryPool();
	~MemoryPool();
	void InitPool();
	void* allocInPool(size_t size);//�����ڴ�	
	void freeInPool(void* pMem);//�ͷ��ڴ�

private:
	char* _pPoolBuf;//�ڴ�ص�λ��
	MemoryChunk* _pHeader;//��һ�������ڴ���λ��
	size_t _nChunkSize;//�ڴ�����ڴ��Ĵ�С
	size_t _nChunkNum;// �ڴ�����ڴ�����Ŀ
	std::mutex _mutex;

};

template<size_t nChunkSize, size_t nChunkNum>
class MyMemoryPool :public MemoryPool
{
public:
	MyMemoryPool()
	{
		//ǿ�Ʋ��룬��ָ���С����
		const size_t n = sizeof(void*);
		_nChunkSize = (nChunkSize / n) * n + (nChunkSize % n == 0 ? 0 : n);
		_nChunkNum = nChunkNum;
	}
};

//�ڴ�ع�����
class MemoryMgr
{
public:
	static MemoryMgr& Instance();//����ģʽ
	void* allocMem(size_t size);//�����ڴ�
	void freeMem(void* pMem);//�ͷ��ڴ�
	void addRef(void* pMem);//���Ӷ��ڴ�������

private:
	MemoryMgr();
	~MemoryMgr() = default;
	void initMapToPool(int nBegin, int nEnd, MemoryPool* pPool);//��ʼ���ڴ��ӳ������

private:
	MyMemoryPool<64, 100000> _mem64;//10���ڴ�飬���СΪ64�ֽڵ��ڴ��
	MyMemoryPool<128, 100000> _mem128;//10���ڴ�飬���СΪ128�ֽڵ��ڴ��
	MemoryPool* _mapToPool[MAX_MEMORY_SIZE + 1];//�ڴ��ӳ�����飬������ң�ֱ��ӳ�䣬1-64������ÿ���ڴ��

};

#endif // !MEMORYMGR_H_
