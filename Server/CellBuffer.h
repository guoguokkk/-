#ifndef CELL_BUFFER_H_
#define CELL_BUFFER_H_
#include"MemoryAlloc.h"
#include<memory>
#include"CellLog.h"

class CellBuffer
{
public:
	CellBuffer(int size = 8192) :_nSize(size)
	{
		_pBuf = new char[_nSize];
		_nLast = 0;
		_bufFullCount = 0;
	}

	~CellBuffer()
	{
		if (!_pBuf)
			delete[] _pBuf;
	}

	//������ݣ�����Ϊ���ݺ����ݵĳ���
	bool push(const char* pData, int nLen)
	{
		if (_nLast + nLen <= _nSize)
		{
			//û����������²ſ��Է��룬���򷵻ش���
			memcpy(_pBuf + _nLast, pData, nLen);//�����ݷ��뷢�ͻ�����
			_nLast += nLen;//���·��ͻ�����β��λ��

			if (_nLast == _nSize)
			{
				++_bufFullCount;
			}
			return true;
		}
		else
		{
			////д��������ݲ�һ��Ҫ�ŵ��ڴ���
			////Ҳ���Դ洢�����ݿ���ߴ��̵ȴ洢����				
			//int n = (_nLast + nLen) - _nSize;
			////��չBUFF
			//if (n < 8192)
			//	n = 8192;
			//char* buff = new char[_nSize + n];
			//memcpy(buff, _pBuf, _nLast);
			//delete[] _pBuf;
			//_pBuf = buff;

			++_bufFullCount;
			return false;
		}
	}

	//д����
	int write2socket(SOCKET sockfd)
	{
		int ret = 0;
		//������������
		if (_nLast > 0 && sockfd != INVALID_SOCKET)
		{
			ret = send(sockfd, _pBuf, _nLast, 0);//�����ͻ����������ݷ��ͳ�ȥ
			_nLast = 0;//���ͻ�����β������
			_bufFullCount = 0;
		}
		return ret;
	}

	//������
	int read4socket(SOCKET sockfd)
	{
		if (_nSize - _nLast > 0)
		{
			char* recvBuf = _pBuf + _nLast;//���տͻ�����Ϣ��ֱ��ʹ��ÿ���ͻ��˵���Ϣ��������������
			int nLen = recv(sockfd, recvBuf, _nSize - _nLast, 0);
			//�жϿͻ����Ƿ��˳�
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

	//�Ƿ��������һ����Ϣ
	bool hasMsg()
	{
		if (_nLast >= sizeof(netmsg_Header))
		{
			netmsg_Header* header = (netmsg_Header*)_pBuf;
			if (_nLast >= header->dataLength)
			{
				return true;
			}
		}
		return false;
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
		if (_bufFullCount > 0)
			--_bufFullCount;
	}

private:
	char* _pBuf = nullptr;//������
	int _nLast;//������β��λ�ã��������ݳ���
	int _nSize;//�������ܳ���
	int _bufFullCount;//������д���Ĵ���
};

#endif // !CELL_BUFFER_H_

