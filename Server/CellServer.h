#ifndef CELL_SERVER
#define CELL_SERVER

#ifdef _WIN32
#include"../Test/Message.h"
#else
#include"Message.h"
#endif // _WIN32

#include<vector>
#include"ClientSocket.h"
#include"INetEvent.h"
#include<thread>
#include<mutex>
#include<atomic>//ԭ�Ӳ���

//��Ϣ����
class CellServer {
public:
	CellServer(SOCKET server_sock);
	~CellServer();	
	void SetEventObj(INetEvent* event) { _pEvent = event; }
	void CloseServer();//�رշ�����
	bool OnRun();//select	
	int RecvData(ClientSock* pClient);//������Ϣ������ճ�����ٰ�
	void OnNetMsg(ClientSock* pClient, Header* header);//��Ӧ��������
	void AddClient(ClientSock* pClient);//���ӿͻ���
	void StartCellServer();
	size_t GetClientCount();
private:
	SOCKET _server_sock;
	std::vector<ClientSock*> _clients;//��ʽ�ͻ�����
	std::vector<ClientSock*> _clientsBuf;//����ͻ�����
	char _recv_buf[RECV_BUF_SIZE];//���ջ�����
	std::mutex _mutex;//������е���
	std::thread* _pThread;
	INetEvent* _pEvent;//�����¼�����
};

#endif // !CELL_SERVER
