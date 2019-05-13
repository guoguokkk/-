#ifndef INETEVENT_H_
#define INETEVENT_H_

#include"ClientSocket.h"
#include"CellServer.h"
#include<memory>
//�����¼��ӿ�
class CellServer;
class ClientSock;
class INetEvent
{
public:	
	virtual void onNetJoin(std::shared_ptr<ClientSock>& pClient) = 0;//�ͻ��˼����¼�
	virtual void onNetLeave(std::shared_ptr<ClientSock>& pClient) = 0;//�ͻ����뿪�¼�
	virtual void onNetMsg(CellServer* pCellServer, std::shared_ptr<ClientSock>& pClient,Header* header) = 0;//�ͻ�����Ϣ�¼�
	virtual void onNetRecv(std::shared_ptr<ClientSock>& pClient) = 0;//���ܴ����¼�
};
#endif // !INETEVENT_H_
