#include "CellServer.h"
#include<iostream>
#include<functional>
#include<chrono>

CellServer::CellServer(SOCKET server_sock = INVALID_SOCKET)
{
	_server_sock = server_sock;
	_pEvent = nullptr;
}

CellServer::~CellServer()
{
	CloseServer();
	_server_sock = INVALID_SOCKET;
}

void CellServer::CloseServer()
{
	//�����ظ��رգ�
	if (_server_sock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	//�ر�ÿ���ͻ��ˣ�
	for (int i = 0; i < _clients.size(); ++i)
	{
		closesocket(_clients[i]->GetSock());
		delete _clients[i];
	}
	closesocket(_server_sock);
#else
	for (int i = 0; i < _client_groups.size(); ++i)
	{
		close(_client_groups[i]->GetSock());
		delete _client_groups[i];
	}
	close(_server_sock);
#endif // _WIN32
	_clients.clear();
}

bool CellServer::OnRun()
{
	while (true)
	{
		if (_server_sock == INVALID_SOCKET)
		{
			return false;
		}

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

		SOCKET max_sock = _clients[0]->GetSock();
		for (int i = 0; i < _clients.size(); ++i)
		{
			FD_SET(_clients[i]->GetSock(), &fd_read);
			if (max_sock < _clients[i]->GetSock())
			{
				max_sock = _clients[i]->GetSock();
			}
		}

		int ret = select(max_sock + 1, &fd_read, nullptr, nullptr, nullptr);//����Ҫ����
		if (ret < 0)
		{
			printf("Server %d select error.\n", (int)_server_sock);
			CloseServer();
			return false;
		}

		for (int i = 0; i < _clients.size(); ++i)
		{
			if (FD_ISSET(_clients[i]->GetSock(), &fd_read))
			{
				int ret = RecvData(_clients[i]);//������Ϣ
				if (ret == -1)
				{
					auto iter = _clients.begin() + i;
					if (iter != _clients.end())
					{
						if (_pEvent)
						{
							_pEvent->OnNetLeave(_clients[i]);
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

//������Ϣ������ճ�����ٰ�
int CellServer::RecvData(ClientSock* pClient)
{
	//���տͻ�����Ϣ
	int len = (int)recv(pClient->GetSock(), _recv_buf, RECV_BUF_SIZE, 0);
	//�жϿͻ����Ƿ��˳�
	if (len <= 0)
	{
		printf("Client %d exit.\n", (int)pClient->GetSock());
		return -1;
	}

	//���յ������ݷ����Ӧ�ͻ�����Ϣ������
	memcpy(pClient->GetMsgBuf() + pClient->GetLastPos(), _recv_buf, len);
	pClient->SetLastPos(pClient->GetLastPos() + len);

	//����ճ�����ٰ�����
	while (pClient->GetLastPos() >= sizeof(Header))
	{
		Header* header = (Header*)pClient->GetMsgBuf();
		if (pClient->GetLastPos() >= header->data_length)
		{
			//������Ϣ������
			int temp_last_pos = pClient->GetLastPos() - (header->data_length);
			OnNetMsg(pClient, header);
			memcpy(pClient->GetMsgBuf(), pClient->GetMsgBuf() + (header->data_length), temp_last_pos);
			pClient->SetLastPos(temp_last_pos);
		}
		else
		{
			break;//��Ϣ������ʣ�����ݲ���һ��������Ϣ
		}
	}
	return 0;
}

//��Ӧ��������
void CellServer::OnNetMsg(ClientSock* pClient, Header* header)
{
	_pEvent->OnNetMsg(pClient, header);
}

//������ȡ����������еĿͻ���
void CellServer::AddClient(ClientSock* pClient)
{
	//�������Խ���
	std::lock_guard<std::mutex> lock(_mutex);
	_clientsBuf.push_back(pClient);
}

void CellServer::StartCellServer()
{
	_pThread = new std::thread(std::mem_fun(&CellServer::OnRun), this);//������ std::mem_fun
}

size_t CellServer::GetClientCount()
{
	return size_t(_clients.size()+_clientsBuf.size());
}
