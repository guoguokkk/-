#ifndef CELL_SERVER_H_
#define CELL_SERVER_H_

#include"Message.h"
#include<vector>
#include"CellClient.h"
#include"INetEvent.h"
#include<thread>
#include<mutex>
#include<atomic>//ԭ�Ӳ���
#include<map>
#include"CellTask.h"

class INetEvent;

//������Ϣ���Ͷ���
class SendMsgToClientTask
{
public:
	SendMsgToClientTask(std::shared_ptr<CellClient> pClient, std::shared_ptr<Header> header)
	{
		_pClient = pClient;
		_pHeader = header;
	}
private:
	std::shared_ptr<CellClient> _pClient;//Ŀ��ͻ���
	std::shared_ptr<Header> _pHeader;//Ҫ���͵�����
};

//��Ϣ������
class CellServer {
public:
	CellServer(SOCKET serverSock = INVALID_SOCKET);
	~CellServer();

	void setEventObj(INetEvent* event);//�������¼�	
	void addClient(std::shared_ptr<CellClient> pClient);//���ӿͻ���
	void startCellServer();
	size_t getClientCount();
	void addSendTask(std::shared_ptr<CellClient> pClient, std::shared_ptr<Header> header)
	{
		auto task = std::make_shared<SendMsgToClientTask>(pClient, header);

		//ִ������
		_taskServer.addTask([pClient, header]() {
			pClient->sendData(header);
			});
	}

private:
	void closeServer();//�رշ�����
	bool onRun();//select	
	bool isRun();//�жϷ������Ƿ�������
	int recvData(std::shared_ptr<CellClient> pClient);//������Ϣ������ճ�����ٰ�
	virtual void onNetMsg(std::shared_ptr<CellClient>& pClient, Header* header);//��Ӧ��������

private:
	SOCKET _serverSock;
	std::map<SOCKET, std::shared_ptr<CellClient>> _clients;//��ʽ�ͻ�����
	std::vector<std::shared_ptr<CellClient>> _clientsBuf;//����ͻ�����
	std::mutex _mutex;//������е���
	std::thread _thread;
	INetEvent* _pEvent;//�����¼�����
	fd_set _fdReadBack;//�ͻ��б���
	bool _clientsChange;//�ͻ��б��Ƿ�ı�
	SOCKET _maxSock;
	TaskServer _taskServer;
};

#endif // !CELL_SERVER_H_
