#ifndef INETEVENT_H_
#define INETEVENT_H_

#include"ClientSocket.h"
#include"CellServer.h"
#include<memory>
//网络事件接口
class CellServer;
class ClientSock;
class INetEvent
{
public:	
	virtual void onNetJoin(std::shared_ptr<ClientSock>& pClient) = 0;//客户端加入事件
	virtual void onNetLeave(std::shared_ptr<ClientSock>& pClient) = 0;//客户端离开事件
	virtual void onNetMsg(CellServer* pCellServer, std::shared_ptr<ClientSock>& pClient,Header* header) = 0;//客户端消息事件
	virtual void onNetRecv(std::shared_ptr<ClientSock>& pClient) = 0;//接受次数事件
};
#endif // !INETEVENT_H_
