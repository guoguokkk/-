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
	CellServer(SOCKET serverSock = INVALID_SOCKET)
	{
		_serverSock = serverSock;
		_pEvent = nullptr;
	}
	~CellServer();	
	void setEventObj(INetEvent* event);
	void closeServer();//�رշ�����
	bool onRun();//select	
	bool isRun();
	int recvData(ClientSock* pClient);//������Ϣ������ճ�����ٰ�
	virtual void onNetMsg(ClientSock* pClient, Header* header);//��Ӧ��������
	void addClient(ClientSock* pClient);//���ӿͻ���
	void startCellServer();
	size_t getClientCount();
private:
	SOCKET _serverSock;
	std::vector<ClientSock*> _clients;//��ʽ�ͻ�����
	std::vector<ClientSock*> _clientsBuf;//����ͻ�����
	std::mutex _mutex;//������е���
	std::thread _thread;
	INetEvent* _pEvent;//�����¼�����
	char _recvBuf[RECV_BUF_SIZE] = {};//���ջ�����
};

#endif // !CELL_SERVER
