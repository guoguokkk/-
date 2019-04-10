#ifndef CLIENT_H
#define CLIENT_H
#ifdef _WIN32
#include"../Test/Common.h"
#else
#include"Common.h"
#endif // _WIN32
#include<vector>
class Client
{
public:
	Client();
	virtual ~Client();
	void InitClient();//��ʼ���ͻ���
	int Connect(const char* ip, const unsigned short port);//���ӷ�����
	void CloseClient();//�رտͻ���
	bool OnRun();//��ѯ
	bool IsRun();//�ж��Ƿ������������
	int RecvData();//��������
	int SendData(Header* header);//����
	void OnNetMsg(Header* header, char* recv_buf);//����������Ϣ
private:
	int _client_sock;
};

#endif // !CLIENT_H
