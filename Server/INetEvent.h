#ifndef INETEVENT_H_
#define INETEVENT_H_

#include"ClientSocket.h"

//�����¼��ӿ�
class INetEvent
{
public:	
	virtual void OnNetJoin(ClientSock* p_client) = 0;//�ͻ��˼����¼�
	virtual void OnNetLeave(ClientSock* p_client) = 0;//�ͻ����뿪�¼�
	virtual void OnNetMsg(ClientSock* p_client,Header* header) = 0;//�ͻ�����Ϣ�¼�
};
#endif // !INETEVENT_H_
