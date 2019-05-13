#ifndef INETEVENT_H_
#define INETEVENT_H_

#include"CellClient.h"
#include"CellServer.h"
#include<memory>

//�����¼��ӿ�
class CellServer;
class INetEvent
{
public:	
	virtual void onNetJoin(std::shared_ptr<CellClient>& pClient) = 0;//�ͻ��˼����¼�
	virtual void onNetLeave(std::shared_ptr<CellClient>& pClient) = 0;//�ͻ����뿪�¼�
	virtual void onNetMsg(CellServer* pCellServer, std::shared_ptr<CellClient>& pClient,Header* header) = 0;//�ͻ�����Ϣ�¼�
	virtual void onNetRecv(std::shared_ptr<CellClient>& pClient) = 0;//���մ����¼�
};
#endif // !INETEVENT_H_
