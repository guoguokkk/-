#include"Server.h"
#include<algorithm>
Server::Server()
{
}

Server::~Server()
{
}

void Server::CloseServer()
{
	//�ر�socket  
#ifdef _WIN32
	closesocket(_server_sock);
#else
	close(_server_sock);
#endif // _WIN32	
}

void Server::InitServer()
{
	//����һ��socket,ipv4���������ӵģ�tcpЭ��
	_server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_sock < 0)
		HandleError("Server socket error.");
	else
		HandleSuccess("Server socket success.");

	//�󶨽��ܿͻ������ӵĶ˿�
	sockaddr_in _server_addr;
#ifdef _WIN32
	_server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);//inet_addrҪ�ر�SDL
#else
	_server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
#endif

	_server_addr.sin_port = htons(PORT);
	_server_addr.sin_family = AF_INET;
	if (-1 == bind(_server_sock, (sockaddr*)& _server_addr, sizeof(_server_addr)))
		HandleError("Server bind error.");
	else
		HandleSuccess("Server bind success.");

	//��������˿�
	if (-1 == listen(_server_sock, 5))
		HandleError("Server listen error.");
	else
		HandleSuccess("Server listen success.");
}

//�������󣬷���������
int Server::processor(SOCKET _client_sock)
{
	char recv_buf[4096];
	int len = (int)recv(_client_sock, recv_buf, sizeof(Header), 0);

	//ǿ������ת��
	Header* header = (Header*)recv_buf;
	if (len < 0)
	{
		std::cout << "client " << _client_sock << " is exit." << std::endl;
		return -1;
	}
	else
	{
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			//���տͻ��˷��͵�����
			recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);
			LOGIN* login = (LOGIN*)recv_buf;
			std::cout << login->_user_name << " : " << login->_user_password << std::endl;

			//��������
			LOGIN_RESULT login_result;
			send(_client_sock, (char*)& login_result, sizeof(login_result), 0);
		}
		break;
		case CMD_LOGOUT:
		{
			//���տͻ��˷��͵�����
			recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);
			LOGOUT* logout = (LOGOUT*)recv_buf;
			std::cout << logout->_user_name << " : " << logout->_user_password << std::endl;

			//��������
			LOGOUT_RESULT logout_result;
			send(_client_sock, (char*)& logout_result, sizeof(logout_result), 0);
		}
		break;
		default:
		{
			Header header = { 0,CMD_ERROR };
			send(_client_sock, (char*)& header, sizeof(header), 0);
		}
		break;
		}
	}
}

void Server::HandleClientRequest()
{

	//�������ͻ��˵�����
	while (true)
	{
		//�������׽���
		fd_set _fd_read;//���������ϣ�������ָ����socket
		FD_ZERO(&_fd_read);//������
		FD_SET(_server_sock, &_fd_read);//������������������set����

		SOCKET _max_sock = _server_sock;
		//���ͻ�������������set����
		for (int i = 0; i < _group_clients.size(); ++i)
		{
			FD_SET(_group_clients[i], &_fd_read);
			if (_max_sock < _group_clients[i])
			{
				_max_sock = _group_clients[i];
			}
		}

		timeval _time_val;
		_time_val.tv_sec = 1;
		_time_val.tv_usec = 0;
		//�ж��������Ƿ��ڼ�����
		int ret = select(_max_sock + 1, &_fd_read, NULL, NULL, &_time_val);
		if (ret < 0)
		{
			HandleError("Server select end.");
			break;
		}

		//�����select�������غ�ĳ���������Ƿ�׼����
		if (FD_ISSET(_server_sock, &_fd_read))
		{
			FD_CLR(_server_sock, &_fd_read);

			//�ȴ����ܿͻ�������
			SOCKET _client_sock;
			sockaddr_in _client_addr;

#ifdef _WIN32
			int _client_addr_size = sizeof(_client_addr);
#else
			socklen_t _client_addr_size = sizeof(_client_addr);
#endif // _WIN32	
			_client_sock = accept(_server_sock, (sockaddr*)& _client_addr, &_client_addr_size);
			if (-1 == _client_sock)
				HandleError("Server accept error.");//��Ч�ͻ���
			else
			{//���ӿͻ��˳ɹ�
				//�������пͻ������¿ͻ��˼���
				NEW_USER_JOIN _new_user_join;
				for (int i = 0; i < _group_clients.size(); ++i)
				{
					send(_group_clients[i], (const char*)& _new_user_join, sizeof(_new_user_join), 0);
				}
				std::cout << "New Client " << _client_sock << ", ip: "
					<< inet_ntoa(_client_addr.sin_addr) << std::endl;//inet_ntoa
				_group_clients.push_back(_client_sock);//����ÿͻ���
			}
		}

		//���δ������е�fd,linux:error: ��struct fd_set�� has no member named ��fd_count��
		// for (int i = 0; i < _fd_read.fd_count; ++i)
		// {
		// 	int ret = processor(_fd_read.fd_array[i]);
		// 	if (ret == -1)
		// 	{//���ĳ���ͻ��˽���������������ɾ��
		// 		auto iter = find(_group_clients.begin(), _group_clients.end(), _group_clients[i]);
		// 		if (iter != _group_clients.end())
		// 		{
		// 			_group_clients.erase(iter);
		// 		}
		// 	}
		// }

		for (int i = 0; i < _group_clients.size(); ++i)
		{
			if (FD_ISSET(_group_clients[i], &_fd_read))
			{
				if (processor(_group_clients[i]) == -1)
				{//���ĳ���ͻ��˽���������������ɾ��
					auto iter = find(_group_clients.begin(), _group_clients.end(), _group_clients[i]);
					if (iter != _group_clients.end())
					{
						_group_clients.erase(iter);
					}
				}
			}
		}

		std::cout << "do other things." << std::endl;
	}

	for (int i = 0; i < _group_clients.size(); ++i)
	{
		//�ر�socket  
#ifdef _WIN32
		closesocket(_group_clients[i]);
#else
		close(_group_clients[i]);
#endif // _WIN32	
	}
}
