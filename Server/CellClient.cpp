#include "CellClient.h"

CellClient::CellClient(SOCKET sockfd)
{
	static int n = 1;
	id = n++;
	_sockfd = sockfd;
	memset(_msgBuf, 0, RECV_BUF_SIZE);//初始化消息缓冲区
	_lastMsgPos = 0;

	memset(_szSendBuf, 0, SEND_BUF_SIZE);//初始化发送缓冲区
	_lastSendPos = 0;

	resetDTHeart();//心跳死亡计时初始化
	resetDTSend();//重置上次发送消息的时间
}

CellClient::~CellClient()
{
	printf("s=%d CellClient%d.~CellClient\n", serverId, id);
	if (_sockfd != INVALID_SOCKET)
	{
#ifdef _WIN32
		closesocket(_sockfd);
#else
		close(_sockfd);
#endif // _WIN32
	}
	_sockfd = INVALID_SOCKET;
}

//缓冲区的控制根据业务需求的差异而调整，异步发送数据
int CellClient::sendDataAsynchronous(std::shared_ptr<netmsg_Header> header)
{
	int ret = SOCKET_ERROR;//缓冲区不足
	int nSendLen = header->dataLength;//待发送数据的长度
	const char* pSendData = (const char*)header.get();//待发送数据

	//发送数据的条件：发送缓冲区满
	//判断缓冲区中已有数据和待发送数据总长度
	//先判断是不是满
	if (_lastSendPos + nSendLen <= SEND_BUF_SIZE)
	{
		//没有满的情况下才可以放入，否则返回错误
		memcpy(_szSendBuf + _lastSendPos, pSendData, nSendLen);//将数据放入发送缓冲区
		_lastSendPos = _lastSendPos + nSendLen;//更新发送缓冲区尾部位置

		if (_lastSendPos == SEND_BUF_SIZE)
		{
			++_sendBufFullCount;
		}
		return nSendLen;
	}
	else
	{
		++_sendBufFullCount;//缓冲区满了
	}
	return ret;
}

/**
 * @brief	服务器向该客户端发送数据
 * @param	header	待发送数据
 * @return	是否成功
 */
int CellClient::sendData(std::shared_ptr<netmsg_Header> header)
{
	int ret = SOCKET_ERROR;
	int nSendLen = header->dataLength;//待发送数据的长度
	const char* pSendData = (const char*)header.get();//待发送数据

	while (true)
	{
		//发送数据的条件：发送缓冲区满
		//判断缓冲区中已有数据和待发送数据总长度
		if (_lastSendPos + nSendLen >= SEND_BUF_SIZE)
		{
			int nCopyLen = SEND_BUF_SIZE - _lastSendPos;//发送缓冲区剩余大小
			memcpy(_szSendBuf + _lastSendPos, pSendData, nCopyLen);//填满缓冲区
			pSendData = pSendData + nCopyLen;//未能放入发送缓冲区的数据
			nSendLen = nSendLen - nCopyLen;//更新长度
			ret = send(_sockfd, _szSendBuf, SEND_BUF_SIZE, 0);
			_lastSendPos = 0;
			resetDTSend();//发送成功，需要重置上次发送成功的时间

			if (ret == SOCKET_ERROR)
			{
				return ret;
			}
		}
		else
		{
			memcpy(_szSendBuf + _lastSendPos, pSendData, nSendLen);//将数据放入发送缓冲区
			_lastSendPos = _lastSendPos + nSendLen;//更新发送缓冲区尾部位置
			break;
		}
	}
	return ret;
}

//立即将缓冲区的数据发送给客户端
int CellClient::sendDataReal()
{
	int ret = 0;
	//缓冲区有数据
	if (_lastSendPos > 0 && _sockfd != INVALID_SOCKET)
	{
		ret = send(_sockfd, _szSendBuf, _lastSendPos, 0);//将发送缓冲区的数据发送出去
		_lastSendPos = 0;//发送缓冲区尾部清零
		_sendBufFullCount = 0;
		resetDTSend();//重置发送时间
	}
	return ret;
}

bool CellClient::checkHeart(time_t dt)
{
	_dtHeart += dt;
	if (_dtHeart >= CLIENT_HEART_DEAD_TIME)
	{
		printf("checkHeart dead:s=%d,time=%d\n", _sockfd, _dtHeart);
		return true;
	}
	return false;
}

bool CellClient::checkSend(time_t dt)
{
	_dtSend += dt;
	if (_dtSend >= CLIENT_SEND_BUF_TIME)
	{
		//printf("checkSend: _sockfd=%d, time=%d\n", (int)_sockfd, (int)_dtSend);
		//时间到了，立即将发送缓冲区的数据发送出去
		sendDataReal();

		//重置发送计时
		resetDTSend();
		return true;
	}
	return false;
}
