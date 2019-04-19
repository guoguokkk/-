#ifndef INETEVENT_H_
#define INETEVENT_H_

#include"ClientSocket.h"

//网络事件接口
class INetEvent
{
public:	
	virtual void onNetJoin(ClientSock* pClient) = 0;//客户端加入事件
	virtual void onNetLeave(ClientSock* pClient) = 0;//客户端离开事件
	virtual void onNetMsg(ClientSock* pClient,Header* header) = 0;//客户端消息事件
	virtual void onNetRecv(ClientSock* pClient) = 0;//接受次数事件
};
#endif // !INETEVENT_H_
