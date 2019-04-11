#ifndef CLIENT_H
#define CLIENT_H
#ifdef _WIN32
#include"../Test/Common.h"
#else
#include"Common.h"
#endif // _WIN32
#include<vector>
class Client {
public:
	Client();
	virtual ~Client();
	int Connect(const char* ip, const unsigned short port);//���ӷ�����
	void CloseClient();//�رտͻ���
	void InitClient();//��ʼ���ͻ���	
	bool OnRun();//select
	int RecvData(SOCKET client_sock);//��������,����ճ��,��ְ�
	int SendData(Header* header);//����	
private:
	SOCKET _client_sock;
	char _recv_buf[RECV_BUF_SIZE];//���ջ�����
	char _msg_buf[RECV_BUF_SIZE * 10];//��Ϣ������
	int _last_pos = 0;//�ϴδ�ŵ�λ�ã���Ϣ��������	
	void OnNetMsg(Header* header);//����������Ϣ
	bool IsRun();//�ж��Ƿ������������	
};

#endif // !CLIENT_H
