#ifndef CELL_CLIENT_H_
#define CELL_CLIENT_H_

#include"Message.h"
#include<memory>
#include"ObjectPool.h"

//�ͻ����������ͣ����ö����ԭ�򣺿ͻ���Ƶ�����˳�����
class CellClient :public ObjectPoolBase<CellClient, 10000>
{
public:
	CellClient(SOCKET clientSock = INVALID_SOCKET);
	~CellClient();

	int sendData(std::shared_ptr<Header> header);//�������ݣ���ʱ��������
	
	SOCKET getSockfd() { return _sockfd; }
	char* getMsgBuf() { return _msgBuf; }
	int getLastPos() { return _lastMsgPos; }
	void setLastPos(int pos) { _lastMsgPos = pos; }

private:
	SOCKET _sockfd;//�ͻ���socket

	char _msgBuf[RECV_BUF_SIZE];//��Ϣ������
	int _lastMsgPos;//��Ϣ������β��λ��
	
	char _sendBuf[SEND_BUF_SIZE];//���ͻ�����
	int _lastSendPos;//���ͻ�����β��λ��
};
#endif // !CELL_CLIENT_H_
