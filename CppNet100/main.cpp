#ifndef _CPP_NET_DLL_H
#define _CPP_NET_DLL_H

//按c语言编译，因为给c#语言使用
extern "C"
{
	//__declspec(dllexport) 关键字从 DLL 导出数据、函数、类或类成员函数。
	int _declspec(dllexport) add(int a, int b)
	{
		return a + b;
	}
}

#endif // !_CPP_NET_DLL_H
