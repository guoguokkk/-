#ifndef CELL_BUFFER_H_
#define CELL_BUFFER_H_
#include"../memory/MemoryAlloc.h"
#include<memory>
#include"../tool/CellLog.h"

class CellBuffer
{
public:
	CellBuffer(int size = 8192) :_nSize(size)
	{
		_pBuf = new char[_nSize];
		_nLast = 0;
		_fullCount = 0;
	}

	~CellBuffer()
	{
		if (!_pBuf)
			delete[] _pBuf;
	}

	//添加数据，参数为数据和数据的长度
	bool push(const char* pData, int nLen)
	{
		if (_nLast + nLen <= _nSize)
		{
			//没有满的情况下才可以放入，否则返回错误
			memcpy(_pBuf + _nLast, pData, nLen);//将数据放入发送缓冲区
			_nLast += nLen;//更新发送缓冲区尾部位置

			if (_nLast == _nSize)
			{
				++_fullCount;
			}
			return true;
		}
		else
		{
			////写入大量数据不一定要放到内存中
			////也可以存储到数据库或者磁盘等存储器中				
			//int n = (_nLast + nLen) - _nSize;
			////拓展BUFF
			//if (n < 8192)
			//	n = 8192;
			//char* buff = new char[_nSize + n];
			//memcpy(buff, _pBuf, _nLast);
			//delete[] _pBuf;
			//_pBuf = buff;

			++_fullCount;
			return false;
		}
	}

	//写数据
	int write2socket(SOCKET sockfd)
	{
		int ret = 0;
		//缓冲区有数据
		if (_nLast > 0 && sockfd != INVALID_SOCKET)
		{
			ret = send(sockfd, _pBuf, _nLast, 0);//将发送缓冲区的数据发送出去
			_nLast = 0;//发送缓冲区尾部清零
			_fullCount = 0;
		}
		return ret;
	}

	//读数据
	int read4socket(SOCKET sockfd)
	{
		if (_nSize - _nLast > 0)
		{
			char* recvBuf = _pBuf + _nLast;//接收客户端消息，直接使用每个客户端的消息缓冲区接收数据
			int nLen = recv(sockfd, recvBuf, _nSize - _nLast, 0);
			//判断客户端是否退出
			if (nLen <= 0)
			{
				CellLog::Info("Client %d exit.\n", sockfd);
				return -1;
			}
			_nLast += nLen;
			return nLen;
		}
		return 0;
	}

	//是否包含至少一条消息
	bool hasMsg()
	{
		if (_nLast >= sizeof(netmsg_Header))
		{
			netmsg_Header* header = (netmsg_Header*)_pBuf;
			return _nLast >= header->dataLength;
		}
		return false;
	}

	//是否需要写
	bool needWrite()
	{
		if (_nLast >= sizeof(netmsg_Header))
		{
			netmsg_Header* header = (netmsg_Header*)_pBuf;
			return _nLast >= header->dataLength;
		}
		return _nLast>0;
	}

	char* data() { return _pBuf; }

	void pop(int nLen)
	{
		int n = _nLast - nLen;
		if (n > 0)
		{
			memcpy(_pBuf, _pBuf + nLen, n);
		}
		_nLast = n;
		if (_fullCount > 0)
			--_fullCount;
	}

private:
	char* _pBuf = nullptr;//缓冲区
	int _nLast;//缓冲区尾部位置，已有数据长度
	int _nSize;//缓冲区总长度
	int _fullCount;//缓冲区写满的次数
};

#endif // !CELL_BUFFER_H_

