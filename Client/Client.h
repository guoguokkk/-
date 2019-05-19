#ifndef CLIENT_H_
#define CLIENT_H_

#include"Message.h"
#include"TimeStamp.h"

class Client {
public:
	Client();
	virtual ~Client();

	int connectToServer(const char* ip, unsigned short port);//���ӷ�����
	void closeClient();//�رտͻ���	
	bool onRun();//����������Ϣ	
	int recvData(SOCKET clientSock);//������Ϣ
	virtual void onNetMsg(netmsg_Header* header);//������Ϣ
	int sendData(netmsg_Header* header, int nLen);//������Ϣ

private:
	bool isRun();
	void initClient();//��ʼ���ͻ���

private:
	SOCKET _clientSock;
	bool _isConnect;
	char _msgBuf[RECV_BUF_SIZE] = {};
	int _lastMsgPos = 0;
};

#endif // !CLIENT_H_
