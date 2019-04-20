#ifndef CELL_SERVER_
#define CELL_SERVER_

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
#include"Task.h"



//������Ϣ���Ͷ���
class SendMsgToClientTask :public Task
{
public:
	SendMsgToClientTask(ClientSock* pClient, Header* header)
	{
		_pClient = pClient;
		_pHeader = header;
	}

	//ִ�����񣬷�����Ϣ
	void doTask()
	{
		_pClient->sendData(_pHeader);
		delete _pHeader;
	}
private:
	ClientSock* _pClient;//Ŀ��ͻ���
	Header* _pHeader;//Ҫ���͵�����
};

class INetEvent;

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
	void addSendTask(ClientSock* pClient, Header* header)
	{
		SendMsgToClientTask* task = new SendMsgToClientTask(pClient, header);
		_taskServer.addTask(task);
	}
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
	TaskServer _taskServer;
};

#endif // !CELL_SERVER_
