#ifndef CELL_CLIENT_H_
#define CELL_CLIENT_H_

#include"Message.h"
#include<memory>
#include"ObjectPool.h"

#define CLIENT_HEART_DEAD_TIME 60000//�ͻ����������������ʱ��60��
#define CLIENT_SEND_DATA_TIME 200//��ʱ�������ݵ����ʱ������0.2��

//�ͻ����������ͣ����ö����ԭ�򣺿ͻ���Ƶ�����˳�����
class CellClient :public ObjectPoolBase<CellClient, 10000>
{
public:
	CellClient(SOCKET clientSock = INVALID_SOCKET);
	~CellClient();

	int sendData(std::shared_ptr<netmsg_Header> header);//�������ݣ���������
	int sendDataDirect();//�����������������ݷ��͸��ͻ���
	int sendDataDirect(std::shared_ptr<netmsg_Header> header);//��������

	SOCKET getSockfd() { return _sockfd; }
	char* getMsgBuf() { return _msgBuf; }
	int getLastPos() { return _lastMsgPos; }
	void setLastPos(int pos) { _lastMsgPos = pos; }

	void resetDTHeart() { _dtHeart = 0; }//��������������ʱ
	void resetDTSend() { _dtSend = 0; }//�����ϴη�����Ϣ��ʱ��

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

	//������ݷ��͵�ʱ����
	bool checkSend(time_t dt)
	{
		_dtSend += dt;
		if (_dtSend >= CLIENT_SEND_DATA_TIME)
		{
			//printf("checkSend: _sockfd=%d, time=%d\n", (int)_sockfd, (int)_dtSend);
			//ʱ�䵽�ˣ����������ͻ����������ݷ��ͳ�ȥ
			sendDataDirect();

			//���÷��ͼ�ʱ
			resetDTSend();
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
	time_t _dtSend;//�ϴη�����Ϣ��ʱ��(��ʱ������Ϣ)
};
#endif // !CELL_CLIENT_H_
