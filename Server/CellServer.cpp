#include "CellServer.h"
#include<iostream>
#include<functional>
#include<chrono>

CellServer::~CellServer()
{
	closeServer();
	_serverSock = INVALID_SOCKET;
}

void CellServer::setEventObj(INetEvent* event)
{
	_pEvent = event;
}

void CellServer::closeServer()
{
	//�����ظ��رգ�
	if (_serverSock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	//�ر�ÿ���ͻ��ˣ�
	for (int i = 0; i < _clients.size(); ++i)
	{
		closesocket(_clients[i]->getSock());
		delete _clients[i];
	}
	closesocket(_serverSock);
#else
	//�ر�ÿ���ͻ��ˣ�
	for (int i = 0; i < _clients.size(); ++i)
	{
		close(_clients[i]->getSock());
		delete _clients[i];
	}
	close(_serverSock);
#endif // _WIN32
	_clients.clear();
}

bool CellServer::onRun()
{
	while (isRun())
	{
		//�ӻ��������ȡ���ͻ����ݣ�������ʽ�ͻ�����
		if (_clientsBuf.size() > 0)
		{
			std::lock_guard<std::mutex> lock(_mutex);//�Խ���
			for (auto pClient : _clientsBuf)
			{
				_clients.push_back(pClient);
			}
			_clientsBuf.clear();
		}

		//���û����Ҫ����Ŀͻ��ˣ�����
		if (_clients.empty())
		{
			std::chrono::milliseconds t(1);//1����
			std::this_thread::sleep_for(t);//����
			continue;
		}

		fd_set fd_read;
		FD_ZERO(&fd_read);

		SOCKET maxSock = _clients[0]->getSock();
		for (int i = 0; i < _clients.size(); ++i)
		{
			FD_SET(_clients[i]->getSock(), &fd_read);
			if (maxSock < _clients[i]->getSock())
			{
				maxSock = _clients[i]->getSock();
			}
		}

		int ret = select(maxSock + 1, &fd_read, nullptr, nullptr, nullptr);//����Ҫ����
		if (ret < 0)
		{
			printf("Server %d select error.\n", (int)_serverSock);
			closeServer();
			return false;
		}

		for (int i = 0; i < _clients.size(); ++i)
		{
			if (FD_ISSET(_clients[i]->getSock(), &fd_read))
			{
				int ret = recvData(_clients[i]);//������Ϣ
				if (ret == -1)
				{
					auto iter = _clients.begin() + i;
					if (iter != _clients.end())
					{
						if (_pEvent)
						{
							_pEvent->onNetLeave(_clients[i]);
						}

						//Ҫɾdelete��erase�������������
						delete _clients[i];
						_clients.erase(iter);
					}
				}
			}
		}
	}
}

bool CellServer::isRun()
{
	return _serverSock != INVALID_SOCKET;
}

//������Ϣ������ճ�����ٰ�
int CellServer::recvData(ClientSock* pClient)
{
	//���տͻ�����Ϣ
	int nLen = (int)recv(pClient->getSock(), _recvBuf, RECV_BUF_SIZE, 0);
	//�жϿͻ����Ƿ��˳�
	if (nLen <= 0)
	{
		printf("Client %d exit.\n", (int)pClient->getSock());
		return -1;
	}

	//���յ������ݷ����Ӧ�ͻ�����Ϣ������
	memcpy(pClient->getMsgBuf() + pClient->getLastPos(), _recvBuf, nLen);
	pClient->setLastPos(pClient->getLastPos() + nLen);

	//����ճ�����ٰ�����
	while (pClient->getLastPos() >= sizeof(Header))
	{
		Header* header = (Header*)pClient->getMsgBuf();
		if (pClient->getLastPos() >= header->data_length)
		{
			//������Ϣ������
			int temp_pos = pClient->getLastPos() - header->data_length;
			onNetMsg(pClient, header);
			memcpy(pClient->getMsgBuf(), pClient->getMsgBuf() + header->data_length, temp_pos);
			pClient->setLastPos(temp_pos);
		}
		else
		{
			break;//��Ϣ������ʣ�����ݲ���һ��������Ϣ
		}
	}
	return 0;
}

//��Ӧ��������
void CellServer::onNetMsg(ClientSock* pClient, Header* header)
{
	_pEvent->onNetMsg(pClient, header);
}

//������ȡ����������еĿͻ���
void CellServer::addClient(ClientSock* pClient)
{
	//�������Խ���
	std::lock_guard<std::mutex> lock(_mutex);
	_clientsBuf.push_back(pClient);
}

void CellServer::startCellServer()
{
	_thread = std::thread(std::mem_fn(&CellServer::onRun), this);//������ std::mem_fun
}

size_t CellServer::getClientCount()
{
	return size_t(_clients.size() + _clientsBuf.size());
}
