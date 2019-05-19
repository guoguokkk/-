#include "CellClient.h"

CellClient::CellClient(SOCKET sockfd)
{
	_sockfd = sockfd;
	memset(_msgBuf, 0, RECV_BUF_SIZE);//初始化消息缓冲区
	_lastMsgPos = 0;

	memset(_sendBuf, 0, SEND_BUF_SIZE);//初始化发送缓冲区
	_lastSendPos = 0;

	resetDTHeart();//心跳死亡计时初始化
	resetDTSend();//重置上次发送消息的时间
}

CellClient::~CellClient()
{
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
			memcpy(_sendBuf + _lastSendPos, pSendData, nCopyLen);//填满缓冲区
			pSendData = pSendData + nCopyLen;//未能放入发送缓冲区的数据
			nSendLen = nSendLen - nCopyLen;//更新长度
			ret = send(_sockfd, _sendBuf, SEND_BUF_SIZE, 0);

			resetDTSend();//发送成功，需要重置上次发送成功的时间
			_lastSendPos = 0;
			if (ret == SOCKET_ERROR)
			{
				return ret;
			}
		}
		else
		{
			memcpy(_sendBuf + _lastSendPos, pSendData, nSendLen);//将数据放入发送缓冲区
			_lastSendPos = _lastSendPos + nSendLen;//更新发送缓冲区尾部位置
			break;
		}
	}

	return ret;
}

//立即将缓冲区的数据发送给客户端
int CellClient::sendDataDirect()
{
	int ret = SOCKET_ERROR;
	//缓冲区有数据
	if (_lastSendPos > 0&&_sockfd!=SOCKET_ERROR)
	{		
		ret = send(_sockfd, _sendBuf, _lastSendPos, 0);//将发送缓冲区的数据发送出去
		_lastSendPos = 0;//发送缓冲区尾部清零
		resetDTSend();//重置发送时间
	}
	return ret;
}

int CellClient::sendDataDirect(std::shared_ptr<netmsg_Header> header)
{
	sendData(header);
	sendDataDirect();
	return 0;
}
