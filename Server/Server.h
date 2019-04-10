#ifndef SERVER_H
#define SERVER_H
#include"../Test/Common.h"
#include<vector>
class Server
{
public:
	Server();
	virtual ~Server();
	void InitServer();//��ʼ��������
	int Bind(const char* ip, const unsigned short port);//��ip�Ͷ˿ں�
	void Listen(int n);//�����˿�
	void Accept();//���տͻ�������
	void CloseServer();//�رտͻ���
	bool OnRun();//��ѯ
	bool IsRun();//�ж��Ƿ������������
	int RecvData(int client_sock);//��������
	void OnNetMsg(int client_sock,Header* header, char* recv_buf);//����������Ϣ
	int SendData2All(int client_sock, Header* header);//����
	void SendData2All(Header* header);//Ⱥ��
private:
	int _server_sock;
	std::vector<int> _group_clients;
};

#endif // !SERVER_H
