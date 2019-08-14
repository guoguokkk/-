#ifndef CELL_BUFFER_H_
#define CELL_BUFFER_H_
#include"../memory/MemoryAlloc.h"
#include"../comm/Common.h"

//消息缓冲区
class CellBuffer
{
public:
	//构造函数，参数为缓冲区大小 size=8192
	CellBuffer(int nSize = 8192)
	{
		_nSize = nSize;
		_pBuf = new char[_nSize];//新建缓冲区
	}

	~CellBuffer()
	{
		if (_pBuf)
		{
			delete[] _pBuf;//删除缓冲区
			_pBuf = nullptr;
		}
	}

	//获取缓冲区
	char* GetData() { return _pBuf; }

	//向缓冲区中添加数据，参数为要添加的数据 pData，数据的长度 nLen
	bool Push(const char* pData, int nLen)
	{
		//没有满的情况下才可以放入，否则返回错误
		if (_nLast + nLen <= _nSize)
		{
			memcpy(_pBuf + _nLast, pData, nLen);//将数据放入缓冲区
			_nLast += nLen;//更新缓冲区尾部位置
			if (_nLast == SEND_BUF_SIZE)
			{
				++_fullCount;//写满了
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
		}
		return false;
	}

	//弹出缓冲区头部的数据，参数为要弹出的数据长度 nLen
	void Pop(int nLen)
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

	//将数据写入内核-用于发送，参数为描述符 sockfd
	int Write2Socket(SOCKET sockfd)
	{
		int ret = 0;
		//缓冲区有数据
		if (_nLast > 0 && sockfd != INVALID_SOCKET)
		{
			ret = send(sockfd, _pBuf, _nLast, 0);//将缓冲区的数据写入内核-用于发送
			if (ret <= 0)
			{
				CELLLOG_ERROR("write2socket1:sockfd<%d> nSize<%d> nLast<%d> ret<%d>", sockfd, _nSize, _nLast, ret);
				return SOCKET_ERROR;
			}
			if (ret == _nLast)
			{
				//_nLast=2000 实际发送ret=2000				
				_nLast = 0;//数据尾部位置清零
			}
			else
			{
				//_nLast=2000 实际发送ret=1000
				//CELLLog_Info("write2socket2:sockfd<%d> nSize<%d> nLast<%d> ret<%d>", sockfd, _nSize, _nLast, ret);
				_nLast -= ret;//保存剩余数据
				memcpy(_pBuf, _pBuf + ret, _nLast);
			}
			_fullCount = 0;
		}
		return ret;
	}

	//从内核读取数据-用于接收，返回值为消息长度，参数为描述符 sockfd
	int Read4Socket(SOCKET sockfd)
	{
		if (_nSize - _nLast > 0)
		{
			char* recvBuf = _pBuf + _nLast;
			int nLen = (int)recv(sockfd, recvBuf, _nSize - _nLast, 0);//接收客户端消息
			//判断客户端是否退出
			if (nLen <= 0)
			{
				CELLLOG_ERROR("read4socket:sockfd<%d> nSize<%d> nLast<%d> nLen<%d>", sockfd, _nSize, _nLast, nLen);
				return SOCKET_ERROR;
			}
			_nLast += nLen;//消息缓冲区的数据尾部位置后移
			return nLen;
		}
		return 0;
	}

	//是否包含至少一条消息
	bool HasMsg()
	{
		//判断消息缓冲区的数据长度大于消息头netmsg_DataHeader长度
		if (_nLast >= sizeof(netmsg_DataHeader))
		{
			netmsg_DataHeader* header = (netmsg_DataHeader*)_pBuf;//当前消息的总长度
			return _nLast >= header->dataLength;
		}
		return false;
	}

	//是否能写入内核
	bool NeedWrite()
	{
		return _nLast > 0;
	}

private:
	char* _pBuf = nullptr;//缓冲区
	int _nLast = 0;//缓冲区尾部位置，已有数据长度
	int _nSize = 0;//缓冲区总大小
	int _fullCount = 0;//缓冲区写满的次数
};

#endif // !CELL_BUFFER_H_
