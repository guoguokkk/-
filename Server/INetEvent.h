#ifndef INETEVENT_H_
#define INETEVENT_H_

#include"ClientSocket.h"

//�����¼��ӿ�
class INetEvent
{
public:	
	virtual void onNetJoin(ClientSock* pClient) = 0;//�ͻ��˼����¼�
	virtual void onNetLeave(ClientSock* pClient) = 0;//�ͻ����뿪�¼�
	virtual void onNetMsg(ClientSock* pClient,Header* header) = 0;//�ͻ�����Ϣ�¼�
};
#endif // !INETEVENT_H_
