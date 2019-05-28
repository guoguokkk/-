#include "CellClient.h"

CellClient::CellClient(SOCKET sockfd) :_SendBuf(SEND_BUF_SIZE), _recvBuf(RECV_BUF_SIZE)
{
	static int n = 1;
	id = n++;
	_sockfd = sockfd;

	resetDTHeart();//心跳死亡计时初始化
	resetDTSend();//重置上次发送消息的时间
}

CellClient::~CellClient()
{
	CellLog::Info("s=%d CellClient%d.~CellClient\n", serverId, id);
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
	if (_SendBuf.push(pSendData, nSendLen))
	{
		return nSendLen;
	}
	return ret;
}

//立即将缓冲区的数据发送给客户端
int CellClient::sendDataReal()
{
	resetDTSend();//重置发送时间
	return _SendBuf.write2socket(_sockfd);
}

bool CellClient::checkHeart(time_t dt)
{
	_dtHeart += dt;
	if (_dtHeart >= CLIENT_HEART_DEAD_TIME)
	{
		CellLog::Info("checkHeart dead:s=%d,time=%ld\n", _sockfd, _dtHeart);
		return true;
	}
	return false;
}

bool CellClient::checkSend(time_t dt)
{
	_dtSend += dt;
	if (_dtSend >= CLIENT_SEND_BUF_TIME)
	{
		//CellLog::Info("checkSend: _sockfd=%d, time=%d\n", (int)_sockfd, (int)_dtSend);
		//时间到了，立即将发送缓冲区的数据发送出去
		sendDataReal();

		//重置发送计时
		resetDTSend();
		return true;
	}
	return false;
}
