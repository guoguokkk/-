#ifndef INETEVENT_H_
#define INETEVENT_H_

#include"ClientSocket.h"

//网络事件接口
class INetEvent
{
public:	
	virtual void OnNetJoin(ClientSock* p_client) = 0;//客户端加入事件
	virtual void OnNetLeave(ClientSock* p_client) = 0;//客户端离开事件
	virtual void OnNetMsg(ClientSock* p_client,Header* header) = 0;//客户端消息事件
};
#endif // !INETEVENT_H_
