#include"MemoryAlloc.h"
#include<stdlib.h>
#include<iostream>
#include<thread>
#include<mutex>//��
#include<memory>
#include"../Test/TimeStamp.h"
#include"ObjectPool.h"
#include<string>
using namespace std;

class ClassA : public ObjectPoolBase<ClassA, 20>
{
public:
	ClassA(int n)
	{
		num = n;
		printf("ClassA\n");
	}

	~ClassA()
	{
		printf("~ClassA\n");
	}
public:
	int num = 0;
};

class ClassB : public ObjectPoolBase<ClassB, 20>
{
public:
	ClassB(int n, int m)
	{
		num = n * m;
		printf("ClassB\n");
	}

	~ClassB()
	{
		printf("~ClassB\n");
	}
public:
	int num = 0;
};

//ԭ�Ӳ���   ԭ�� ���� 
mutex m;
const int tCount = 4;
const int mCount = 20;
const int nCount = mCount / tCount;
void workFun(int index)
{
	ClassA* data[nCount];
	for (size_t i = 0; i < nCount; i++)
	{
		data[i] = ClassA::createObject(6);

	}
	for (size_t i = 0; i < nCount; i++)
	{
		ClassA::destroyObject(data[i]);
	}
}//��ռʽ

ClassA& fun(ClassA& pA)
{//���ü���  
	pA.num++;
	return pA;
}

void fun(shared_ptr<ClassA>& pA)
{//���ü���  
	pA->num++;
}

void fun(ClassA* pA)
{//���ü���  
	pA->num++;
}

int main()
{
	
	//thread t[tCount];
	//for (int n = 0; n < tCount; n++)
	//{
	//	t[n] = thread(workFun, n);
	//}
	//TimeStamp tTime;
	//for (int n = 0; n < tCount; n++)
	//{
	//	t[n].join();
	//	//t[n].detach();
	//}
	//cout << tTime.getElapsedTimeInMillSec() << endl;
	//cout << "Hello,main thread." << endl;
	

	/*
	int* a = new int;
	*a = 100;
	delete a;
	//printf("a=%d\n", *a);
	//C++��׼������ָ���һ��
	shared_ptr<int> b = make_shared<int>();
	*b = 100;
	//printf("b=%d\n", *b);
	*/

	
	/*{
		shared_ptr<ClassA> b = make_shared<ClassA>(100);
		b->num = 200;
		TimeStamp tTime;
		for (int n = 0; n < 100000000; n++)
		{
			fun(b);
		}
		cout << tTime.getElapsedTimeInMicroSec() << endl;
	}
	{
		ClassA* b = new ClassA(100);
		b->num = 200;
		TimeStamp tTime;
		for (int n = 0; n < 100000000; n++)
		{
			fun(b);
		}
		cout << tTime.getElapsedTimeInMicroSec() << endl;
	}
	
*/
	/*
	ClassA* a1 = new ClassA(5);
	delete a1;

	ClassA* a2 = ClassA::createObject(6);
	ClassA::destroyObject(a2);

	ClassB* b1 = new ClassB(5, 6);
	delete b1;

	ClassB* b2 = ClassB::createObject(5,6);
	ClassB::destroyObject(b2);
	*/
	//{
	//	ClassA* B1 = new ClassA(0);
	//	shared_ptr<ClassA> s0 = make_shared<ClassA>(5);
	//	shared_ptr<ClassA> s1(new ClassA(5));
	//}
	printf("----1----\n");
	{
		shared_ptr<ClassA> s1 = make_shared<ClassA>(5);
	}

	printf("----2----\n");
	{
		shared_ptr<ClassA> s1(new ClassA(5));
	}
	printf("----3----\n");


	ClassA* a1 = new ClassA(5);
	delete a1;
	printf("----4----\n");


	return 0;
}