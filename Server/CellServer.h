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
	SendMsgToClientTask(std::shared_ptr<ClientSock> pClient, std::shared_ptr<Header> header)
	{
		_pClient = pClient;
		_pHeader = header;
	}

	//ִ�����񣬷�����Ϣ
	void doTask()
	{
		_pClient->sendData(_pHeader);
	}
private:
	std::shared_ptr<ClientSock> _pClient;//Ŀ��ͻ���
	std::shared_ptr<Header> _pHeader;//Ҫ���͵�����
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
	int recvData(std::shared_ptr<ClientSock> pClient);//������Ϣ������ճ�����ٰ�
	virtual void onNetMsg(std::shared_ptr<ClientSock>& pClient, Header* header);//��Ӧ��������
	void addClient(std::shared_ptr<ClientSock> pClient);//���ӿͻ���
	void startCellServer();
	size_t getClientCount();
	void addSendTask(std::shared_ptr<ClientSock> pClient, std::shared_ptr<Header> header)
	{
		auto task = std::make_shared<SendMsgToClientTask>(pClient, header);
		_taskServer.addTask((std::shared_ptr<Task>)task);
	}
private:
	SOCKET _serverSock;
	std::map<SOCKET, std::shared_ptr<ClientSock>> _clients;//��ʽ�ͻ�����
	std::vector<std::shared_ptr<ClientSock>> _clientsBuf;//����ͻ�����
	std::mutex _mutex;//������е���
	std::thread _thread;
	INetEvent* _pEvent;//�����¼�����
	fd_set _fdReadBack;//�ͻ��б���
	bool _clientsChange;//�ͻ��б��Ƿ�ı�
	SOCKET _maxSock;
	TaskServer _taskServer;
};

#endif // !CELL_SERVER_
