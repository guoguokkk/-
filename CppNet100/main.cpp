#ifndef _CPP_NET_DLL_H
#define _CPP_NET_DLL_H
#include<string>
#include"client/Client.h"

#ifdef _WIN32
#define EXPORT_DLL
#define EXPORT_DLL _declspec(dllexport)//_declspec(dllexport)关键字从 DLL 导出数据、函数、类或类成员函数
#else
#define EXPORT_DLL
#endif // !_WIN32

extern "C"
{
	typedef void(*OnNetMsgCallBack)(void* csObj, void* data, int len);
}
  
class NativeTCPClient :public Client
{
public:
	virtual void onNetMsg(netmsg_Header* header)
	{
		if (_callBack)
			_callBack(_csObj, header, header->dataLength);
	}

	void setCallBack(void* csObj, OnNetMsgCallBack cb)
	{
		_csObj = csObj;
		_callBack = cb;
	}

private:
	void* _csObj = nullptr;
	OnNetMsgCallBack _callBack = nullptr;
};

//按c语言编译，因为给c#语言使用，需要导出的函数
extern "C"
{
	//回调函数
	//创建
	EXPORT_DLL void* Client_Create(void* csObj, OnNetMsgCallBack cb)
	{
		NativeTCPClient* pClient = new NativeTCPClient();
		pClient->setCallBack(csObj, cb);
		return pClient;
	}

	//连接
	EXPORT_DLL bool Client_Connect(NativeTCPClient* pClient, const char* ip, short port)
	{
		if (pClient && ip)
			return pClient->connectToServer(ip, port) != SOCKET_ERROR;
		return false;
	}

	//连接
	EXPORT_DLL bool Client_OnRun(NativeTCPClient* pClient)
	{
		if (pClient)
			return pClient->onRun();
		return false;
	}

	//关闭
	EXPORT_DLL void Client_Close(NativeTCPClient* pClient)
	{
		if (pClient)
		{
			pClient->closeClient();
			delete pClient;
		}
	}

	//发数据
	EXPORT_DLL int Client_SendData(NativeTCPClient* pClient, const char* pData, int len)
	{
		if (pClient)
			return pClient->sendData(pData, len);
		return 0;
	}
}

#endif // !_CPP_NET_DLL_H
