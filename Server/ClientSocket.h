#ifndef CLIENT_SOCK_H_
#define CLIENT_SOCK_H_

#ifdef _WIN32
#include"../Test/Message.h"
#else
#include"Message.h"
#endif // _WIN32

//�ͻ�����������
class ClientSock {
public:
	ClientSock(SOCKET clientSock = INVALID_SOCKET)
	{
		_sockfd = clientSock;
		memset(_msgBuf, 0, sizeof(_msgBuf));//��ʼ����Ϣ������
		_lastPos = 0;
	}
	SOCKET getSock() { return _sockfd; }
	char* getMsgBuf() { return _msgBuf; }	
	int getLastPos() { return _lastPos; }
	void setLastPos(int pos) { _lastPos = pos; }	

	//�������ݣ���ʱ��������
	int sendData(Header* header)
	{
		if (header)
		{//��ʱ ����
			//return send(_client_sock, (char*) &header, header->data_length, 0);//!!!�������㣬һֱ����ʶ������
			return send(_sockfd, (const char*)header, header->data_length, 0);
		}
		return SOCKET_ERROR;
	}
	
private:
	char _msgBuf[RECV_BUF_SIZE];//��Ϣ��������!!!һ��Ҫ���ڽ��ջ�����
	int _lastPos;//��Ϣ���������һ��λ��
	SOCKET _sockfd;//�ͻ���socket
	char _sendBuf[SEND_BUF_SIZE];//���ͻ�����
};
#endif // !CLIENT_SOCK_H_
