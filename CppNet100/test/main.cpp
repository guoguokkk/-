#ifndef _CPP_NET_DLL_H
#define _CPP_NET_DLL_H
#include<string>
#include"../client/Client.h"
#include"../tool/CellMsgStream.h"

#ifdef _WIN32
#define EXPORT_DLL
#define EXPORT_DLL _declspec(dllexport)//_declspec(dllexport)关键字从 DLL 导出数据、函数、类或类成员函数
#else
#define EXPORT_DLL
#endif // !_WIN32

extern "C"
{
	typedef void(*OnNetMsgCallBack)(void* csObj, void* getData, int len);
}

class NativeTCPClient :public Client
{
public:
	virtual void onNetMsg(netmsg_DataHeader* header)
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
	//////////////////////////////CellClient
	//创建
	EXPORT_DLL void* CellClient_Create(void* csObj, OnNetMsgCallBack cb, int sendSize, int recvSize)
	{
		NativeTCPClient* pClient = new NativeTCPClient();
		pClient->setCallBack(csObj, cb);
		pClient->initClient(sendSize, recvSize);
		return pClient;
	}

	//连接
	EXPORT_DLL bool CellClient_Connect(NativeTCPClient* pClient, const char* ip, short port)
	{
		if (pClient && ip)
			return pClient->connectToServer(ip, port) != SOCKET_ERROR;
		return false;
	}

	//连接
	EXPORT_DLL bool CellClient_OnRun(NativeTCPClient* pClient)
	{
		if (pClient)
			return pClient->onRun();
		return false;
	}

	//关闭
	EXPORT_DLL void CellClient_Close(NativeTCPClient* pClient)
	{
		if (pClient)
		{
			pClient->closeClient();
			delete pClient;
		}
	}

	//发数据
	EXPORT_DLL int CellClient_SendData(NativeTCPClient* pClient, const char* pData, int len)
	{
		if (pClient)
			return pClient->sendData(pData, len);
		return 0;
	}

	//发送流数据
	EXPORT_DLL int CellClient_SendStream(NativeTCPClient* pClient, CellWriteStream* wStream)
	{
		if (pClient && wStream)
		{
			wStream->finish();
			return pClient->sendData(wStream->getData(), wStream->getWritePos());
		}
		return 0;
	}

	//////////////////////////////CellStream
	//////////CellWriteStream
	EXPORT_DLL void* CellWriteStream_Create(int nSize)
	{
		CellWriteStream* wStream = new CellWriteStream(nSize);
		return wStream;
	}
	EXPORT_DLL bool CellWriteStream_WriteInt8(CellWriteStream* wStream, int8_t n)
	{
		if (wStream)
			return wStream->writeInt8(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteInt16(CellWriteStream* wStream, int16_t n)
	{
		if (wStream)
			return wStream->writeInt16(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteInt32(CellWriteStream* wStream, int32_t n)
	{
		if (wStream)
			return wStream->writeInt32(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteInt64(CellWriteStream* wStream, int64_t n)
	{
		if (wStream)
			return wStream->writeInt64(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteUInt8(CellWriteStream* wStream, uint8_t n)
	{
		if (wStream)
			return wStream->writeUInt8(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteUInt16(CellWriteStream* wStream, uint16_t n)
	{
		if (wStream)
			return wStream->writeUInt16(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteUInt32(CellWriteStream* wStream, uint32_t n)
	{
		if (wStream)
			return wStream->writeUInt32(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteUInt64(CellWriteStream* wStream, uint64_t n)
	{
		if (wStream)
			return wStream->writeUInt64(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteFloat(CellWriteStream* wStream, float n)
	{
		if (wStream)
			return wStream->writeFloat(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteDouble(CellWriteStream* wStream, double n)
	{
		if (wStream)
			return wStream->writeDouble(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteString(CellWriteStream* wStream, char* n)
	{
		if (wStream)
			return wStream->writeString(n);
		return false;
	}
	EXPORT_DLL bool CellWriteStream_WriteArray(CellWriteStream* wStream, int32_t* pData, uint32_t len)
	{
		if (wStream)
			return wStream->writeArray(pData, len);
		return false;
	}
	EXPORT_DLL void CellWriteStream_Release(CellWriteStream* wStream)
	{
		if (wStream)
			delete wStream;
	}

	//////////CellReadStream
	EXPORT_DLL void* CellReadStream_Create(char* getData, int len)
	{
		CellReadStream* rStream = new CellReadStream(getData, len);
		return rStream;
	}
	EXPORT_DLL int8_t CellReadStream_ReadInt8(CellReadStream* rStream)
	{
		int8_t n;
		if (rStream)
		{
			rStream->readInt8(n);
			return n;
		}
		return 0;
	}
	EXPORT_DLL int16_t CellReadStream_ReadInt16(CellReadStream* rStream)
	{
		int16_t n;
		if (rStream)
		{
			rStream->readInt16(n);
			return n;
		}
		return 0;
	}
	EXPORT_DLL int32_t CellReadStream_ReadInt32(CellReadStream* rStream)
	{
		int32_t n;
		if (rStream)
		{
			rStream->readInt32(n);
			return n;
		}
		return 0;
	}
	EXPORT_DLL int64_t CellReadStream_ReadInt64(CellReadStream* rStream)
	{
		int64_t n;
		if (rStream)
		{
			rStream->readInt64(n);
			return n;
		}
		return 0;
	}
	EXPORT_DLL uint8_t CellReadStream_ReadUInt8(CellReadStream* rStream)
	{
		uint8_t n;
		if (rStream)
		{
			rStream->readUInt8(n);
			return n;
		}
		return 0;
	}
	EXPORT_DLL uint16_t CellReadStream_ReadUInt16(CellReadStream* rStream)
	{
		uint16_t n;
		if (rStream)
		{
			rStream->readUInt16(n);
			return n;
		}
		return 0;
	}
	EXPORT_DLL uint32_t CellReadStream_ReadUInt32(CellReadStream* rStream)
	{
		uint32_t n;
		if (rStream)
		{
			rStream->readUInt32(n);
			return n;
		}
		return 0;
	}
	EXPORT_DLL uint64_t CellReadStream_ReadUInt64(CellReadStream* rStream)
	{
		uint64_t n;
		if (rStream)
		{
			rStream->readUInt64(n);
			return n;
		}
		return 0;
	}
	EXPORT_DLL float CellReadStream_ReadFloat(CellReadStream* rStream)
	{
		float n;
		if (rStream)
		{
			rStream->readFloat(n);
			return n;
		}
		return 0;
	}
	EXPORT_DLL double CellReadStream_ReadDouble(CellReadStream* rStream)
	{
		double n;
		if (rStream)
		{
			rStream->readDouble(n);
			return n;
		}
		return 0;
	}
	EXPORT_DLL bool CellReadStream_ReadString(CellReadStream* rStream, char* pBuf, uint32_t len)
	{
		if (rStream)
		{
			return rStream->readArray(pBuf, len);
		}
		return false;
	}
	EXPORT_DLL uint32_t CellReadStream_OnlyReadUint32(CellReadStream* rStream)
	{
		uint32_t len = 0;
		if (rStream)
		{
			return rStream->onlyRead(len);
		}
		return len;
	}
	EXPORT_DLL void CellReadStream_Release(CellReadStream* rStream)
	{
		if (rStream)
			delete rStream;
	}
}

#endif // !_CPP_NET_DLL_H
