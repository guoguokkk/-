#ifndef CELL_SERVER_H_
#define CELL_SERVER_H_

#include<thread>
#include<mutex>
#include<atomic>//ԭ�Ӳ���
#include<map>
#include<vector>
#include"Message.h"
#include"CellClient.h"
#include"INetEvent.h"
#include"CellTask.h"
#include"TimeStamp.h"
#include"CellSemaphore.h"

//������Ϣ���Ͷ���
class SendMsgToClientTask
{
public:
	SendMsgToClientTask(std::shared_ptr<CellClient> pClient, std::shared_ptr<netmsg_Header> header)
	{
		_pClient = pClient;
		_pHeader = header;
	}
private:
	std::shared_ptr<CellClient> _pClient;//Ŀ��ͻ���
	std::shared_ptr<netmsg_Header> _pHeader;//Ҫ���͵�����
};


class INetEvent;

//��Ϣ������
class CellServer {
public:
	CellServer(int id = -1);
	~CellServer();

	void setEventObj(INetEvent* event);//�������¼�	
	void addClient(std::shared_ptr<CellClient> pClient);//���ӿͻ���
	void startCellServer();//���������߳�	
	size_t getClientCount();
	void addSendTask(std::shared_ptr<CellClient> pClient, std::shared_ptr<netmsg_Header> header)
	{
		auto task = std::make_shared<SendMsgToClientTask>(pClient, header);

		//ִ������
		_taskServer.addTask([pClient, header]() {
			pClient->sendData(header);
			});
	}

private:
	void closeServer();//�رշ�����
	void onRun();//select	
	int recvData(std::shared_ptr<CellClient> pClient);//������Ϣ������ճ�����ٰ�
	virtual void onNetMsg(std::shared_ptr<CellClient>& pClient, netmsg_Header* header);//��Ӧ��������
	void readData(fd_set& fd_read);//��������

	void checkTime();//���������Ϣ����ɶ�ʱ�������� 
	void clearClients();
private:
	//�����ǰ�ţ�С������ţ��ڴ����
	std::map<SOCKET, std::shared_ptr<CellClient>> _clients;//��ʽ�ͻ�����
	std::vector<std::shared_ptr<CellClient>> _clientsBuf;//����ͻ�����
	std::mutex _mutex;//������е���
	std::thread _thread;
	INetEvent* _pNetEvent;//�����¼�����
	CellTaskServer _taskServer;
	fd_set _fdReadBack;//�ͻ��б���
	bool _clientsChange;//�ͻ��б��Ƿ�ı�
	SOCKET _maxSock;
	time_t _oldTime = CellTime::getNowInMillSec();//��ʱ���
	bool _isRun;//�жϷ������Ƿ�������
	int _id;
	CellSemaphore _sem;
};

#endif // !CELL_SERVER_H_
