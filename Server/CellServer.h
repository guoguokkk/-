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
#include<map>
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
	std::map<SOCKET, ClientSock*> _clients;//��ʽ�ͻ�����
	std::vector<ClientSock*> _clientsBuf;//����ͻ�����
	std::mutex _mutex;//������е���
	std::thread _thread;
	INetEvent* _pEvent;//�����¼�����
	fd_set _fdReadBack;//�ͻ��б���
	bool _clientsChange;//�ͻ��б��Ƿ�ı�
	SOCKET _maxSock;
};

#endif // !CELL_SERVER
