#include"Server.h"
#include<iostream>
#ifdef _WIN32

#else
#include<algorithm>
#endif // _WIN32
using std::cout;
using std::endl;
using std::cin;
Server::Server()
{
	InitServer();
}

Server::~Server()
{
	CloseServer();
}

//��ʼ��������
void Server::InitServer()
{
#ifdef _WIN32
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);//LPWSADATA��һ��ָ��WSADATA�ṹ��ָ��
#endif // _WIN32

	//��ֹ�ظ���ʼ��
	if (_server_sock < 0)
	{
		_server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_server_sock < 0)
			cout << "Server socket error." << endl;
		else
			cout << "Server socket success." << endl;
	}
	else
	{
		CloseServer();//������Ҫ�ȹر�
		cout << "Server " << _server_sock << " close." << endl;
	}
}

//��ip�Ͷ˿ں�
int Server::Bind(const char* ip, const unsigned short port)
{
	sockaddr_in server_addr;
#ifdef _WIN32
	if (ip)
		server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
	else
		server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
	if (ip)
		server_addr.sin_addr.s_addr = inet_addr(ip);
	else
		server_addr.sin_addr.s_addr = INADDR_ANY;
#endif // _WIN32	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	int ret = bind(_server_sock, (const sockaddr*)& server_addr, sizeof(server_addr));
	if (ret < 0)
		cout << "Server " << _server_sock << " bind error." << endl;
	else
		cout << "Server " << _server_sock << " bind success." << endl;
	return ret;
}

void Server::Listen(int n)
{
	int ret = listen(_server_sock, n);
	if (ret < 0)
		cout << "Server " << _server_sock << " listen error." << endl;
	else
		cout << "Server " << _server_sock << " listen success." << endl;
}

void Server::Accept()
{
	int client_sock;
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
#ifdef _WIN32
	client_sock = accept(_server_sock, (sockaddr*)& client_addr, &client_addr_size);
#else
	client_sock = accept(_server_sock, (sockaddr*)& client_addr, (size_t*)& client_addr_size);
#endif // _WIN32
	if (client_sock < 0)
		cout << "Invalid Received Client" << endl;
	else
	{
		//�������ͻ��˷�����Ϣ
		NewUserJoin new_user_join;
		SendData2All(&new_user_join);
		//���뵽�ͻ�������
		_group_clients.push_back(client_sock);
		cout << "New Client join : " << "IP= " << inet_ntoa(client_addr.sin_addr) << endl;
	}
}

//�رշ�����
void Server::CloseServer()
{
	//��ֹ�ظ��ر�
	if (_server_sock < 0)
	{
		return;
	}
#ifdef _WIN32
	closesocket(_server_sock);
	WSACleanup();
#else
	close(_server_sock);
#endif // _WIN32
	_server_sock = -1;
}

//select
bool Server::OnRun()
{
	if (!IsRun())
		return false;

	fd_set fds_read, fds_write, fds_exc;
	FD_ZERO(&fds_read);
	FD_ZERO(&fds_write);
	FD_ZERO(&fds_exc);

	FD_SET(_server_sock, &fds_read);
	FD_SET(_server_sock, &fds_write);
	FD_SET(_server_sock, &fds_exc);
	for (int i = 0; i < _group_clients.size(); ++i)
	{
		FD_SET(_group_clients[i], &fds_read);
	}

	//����ʱ��
	timeval time_val;
	time_val.tv_sec = 1;//��
	time_val.tv_usec = 0;//����
	int ret = select(_server_sock, &fds_read, &fds_write, &fds_exc, &time_val);	
	if (ret < 0)
	{
		cout << "Server " << _server_sock << "select task end 1." << endl;
		CloseServer();
		return false;
	}	
	
	if (FD_ISSET(_server_sock, &fds_read))
	{
		FD_CLR(_server_sock, &fds_read);
		Accept();		
	}
	//���δ������пͻ��˵�����
	for (int i = 0; i < _group_clients.size(); ++i)
	{
		if (FD_ISSET(_group_clients[i], &fds_read))
		{
			int ret = RecvData(_group_clients[i]);
			if (ret == -1)
			{//�ÿͻ����˳�����Ҫ�ӿͻ���������ɾ��
				auto iter = _group_clients.begin() + i;
				if (iter != _group_clients.end())
					_group_clients.erase(iter);
			}
		}
	}
	cout << "do other things" << endl;
	return false;
}

bool Server::IsRun()
{
	return _server_sock >= 0;
}

//��������
int Server::RecvData(int client_sock)
{
	char recv_buf[4096];//���ջ�����
	int len = (int)recv(client_sock, recv_buf, sizeof(Header), 0);//�������ݵ�ͷ��	
	if (len <= 0)
	{
		cout << "Client exit." << endl;
		return -1;
	}
	Header* header = (Header*)recv_buf;//ͷ��
	recv(client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);//���ܳ�ͷ�������������
	OnNetMsg(client_sock,header, recv_buf);
	return 0;

}

void Server::OnNetMsg(int client_sock,Header* header, char* recv_buf)
{
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		Login* login = (Login*)recv_buf;
		cout << "login: name is " << login->name << " , password is " << login->password<< endl;
				
		LoginResult login_result;//���ؽ��
		SendData2All(client_sock, &login_result);
	}
	break;
	case CMD_LOGOUT:
	{
		Logout* logout = (Logout*)recv_buf;
		cout << "Logout: name is " << logout->name << endl;
		
		LogoutResult logout_result;//���ؽ��
		SendData2All(client_sock, &logout_result);
	}
	break;
	default:
	{
		Header header;
		header.cmd = CMD_ERROR;
		header.data_length = 0;
		SendData2All(client_sock, &header);
	}
	break;
	}
}

int Server::SendData2All(int client_sock, Header* header)
{
	//�ж���Ϣ�Ƿ�Ϊ��
	if (IsRun() && header)
	{
		send(client_sock, (const char*)header, header->data_length, 0);
		return 0;
	}
	return -1;
}

void Server::SendData2All(Header* header)
{
	//�ж���Ϣ�Ƿ�Ϊ��
	if (IsRun() && header)
	{
		for (int i = 0; i < _group_clients.size(); ++i)
		{
			send(_group_clients[i], (const char*)header, header->data_length, 0);
		}		
	}
}

