#ifndef CLIENT_SOCK_H_
#define CLIENT_SOCK_H_

#ifdef _WIN32
#include"../Test/Message.h"
#else
#include"Message.h"
#endif // _WIN32

//客户端数据类型
class ClientSock {
public:
	ClientSock(SOCKET clientSock = INVALID_SOCKET)
	{
		_sockfd = clientSock;
		memset(_msgBuf, 0, RECV_BUF_SIZE);//初始化消息缓冲区
		_lastMsgPos = 0;

		memset(_sendBuf, 0, SEND_BUF_SIZE);//初始化消息缓冲区
		_lastSendPos = 0;
	}
	SOCKET getSock() { return _sockfd; }
	char* getMsgBuf() { return _msgBuf; }
	int getLastPos() { return _lastMsgPos; }
	void setLastPos(int pos) { _lastMsgPos = pos; }

	//发送数据，定时定量发送
	int sendData(Header* header)
	{
		int ret = SOCKET_ERROR;
		int nSendLen = header->data_length;//需要发送数据的长度
		const char* pSendData = (const char*)header;//需要发送的数据，不允许修改

		//超出大小
		while (true)
		{
			if (_lastSendPos + nSendLen >= SEND_BUF_SIZE)
			{
				int nCopyLen = SEND_BUF_SIZE - _lastSendPos;//可以拷贝的数据长度
				memcpy(_sendBuf + _lastSendPos, pSendData, nCopyLen);//拷贝数据
				pSendData = pSendData + nCopyLen;
				nSendLen = nSendLen - nCopyLen;
				//return send(_client_sock, (char*) &header, header->data_length, 0);//!!!画蛇添足，一直不能识别数据
				//ret = send(_sockfd, (const char*)header, header->data_length, 0);
				ret = send(_sockfd, _sendBuf, SEND_BUF_SIZE, 0);
				_lastSendPos = 0;
				if (ret == SOCKET_ERROR)
				{
					return ret;
				}
			}
			else
			{
				memcpy(_sendBuf + _lastSendPos, pSendData, nSendLen);//拷贝数据
				_lastSendPos = _lastSendPos + nSendLen;
				break;//如果不足够发一次的时候，就跳出循环
			}
		}

		return ret;
	}

private:
	char _msgBuf[RECV_BUF_SIZE];//消息缓冲区，!!!一定要大于接收缓冲区
	int _lastMsgPos;//消息缓冲区最后一个位置
	SOCKET _sockfd;//客户端socket
	char _sendBuf[SEND_BUF_SIZE];//发送缓冲区
	int _lastSendPos;//发送缓冲区最后一个位置
};
#endif // !CLIENT_SOCK_H_
