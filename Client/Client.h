#ifndef CLIENT_H_
#define CLIENT_H_

#ifdef _WIN32
#include"../Test/Message.h"
#include"../Test/TimeStamp.h"
#else
#include"Message.h"
#include"TimeStamp.h"
#endif // _WIN32

class Client {
public:
	Client();
	virtual ~Client();
	void initClient();//��ʼ���ͻ���
	int connectToServer(const char* ip, unsigned short port);//���ӷ�����
	void closeClient();//�رտͻ���	
	bool onRun();//����������Ϣ
	bool isRun();
	int recvData(SOCKET clientSock);//������Ϣ
	virtual void onNetMsg(Header* header);//������Ϣ
	int sendData(Header* header, int nLen);//������Ϣ
private:
	SOCKET _clientSock;
	bool _isConnect;
	char _msgBuf[RECV_BUF_SIZE] = {};
	int _lastMsgPos = 0;
};

#endif // !CLIENT_H_
