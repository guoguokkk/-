#ifndef CELL_CLIENT_H_
#define CELL_CLIENT_H_

#include"Message.h"
#include<memory>
#include"ObjectPool.h"

#define CLIENT_HEART_DEAD_TIME 5000//�ͻ����������������ʱ��5000����

//�ͻ����������ͣ����ö����ԭ�򣺿ͻ���Ƶ�����˳�����
class CellClient :public ObjectPoolBase<CellClient, 10000>
{
public:
	CellClient(SOCKET clientSock = INVALID_SOCKET);
	~CellClient();

	int sendData(std::shared_ptr<netmsg_Header> header);//�������ݣ���ʱ��������

	SOCKET getSockfd() { return _sockfd; }
	char* getMsgBuf() { return _msgBuf; }
	int getLastPos() { return _lastMsgPos; }
	void setLastPos(int pos) { _lastMsgPos = pos; }

	void resetDTHeart() { _dtHeart = 0; }//��������������ʱ

	//�������
	bool checkHeart(time_t dt)
	{
		_dtHeart += dt;
		if (_dtHeart >= CLIENT_HEART_DEAD_TIME)
		{
			printf("checkHeart dead: _sockfd=%d, time=%d\n", (int)_sockfd, (int)_dtHeart);
			return true;
		}
		return false;
	}
private:
	SOCKET _sockfd;//�ͻ���socket

	char _msgBuf[RECV_BUF_SIZE];//��Ϣ������
	int _lastMsgPos;//��Ϣ������β��λ��

	char _sendBuf[SEND_BUF_SIZE];//���ͻ�����
	int _lastSendPos;//���ͻ�����β��λ��

	time_t _dtHeart;//����������ʱ
};
#endif // !CELL_CLIENT_H_
