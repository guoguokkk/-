#include"Client.h"
#include<thread>

SOCKET Client::_client_sock;
bool Client::g_bRun = true;

Client::Client()
{
}

Client::~Client()
{
}

int Client::Processor(SOCKET _client_sock)
{
	char recv_buf[4096];
	int ret = recv(_client_sock, recv_buf, sizeof(Header), 0);
	Header* header = (Header*)recv_buf;
	if (ret <= 0)
	{
		std::cout << "��������Ͽ�����." << std::endl;
		return -1;
	}
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		//���ܷ��������ص�����
		LOGIN_RESULT _login_result;
		recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);
		std::cout << "�յ��������Ϣ: CMD_LOGIN_RESULT�����ݳ���Ϊ " << _login_result.data_length << std::endl;
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		//���ܷ��������ص�����
		LOGIN_RESULT _logout_result;
		recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);
		std::cout << "�յ��������Ϣ: CMD_LOGIN_RESULT�����ݳ���Ϊ " << _logout_result.data_length << std::endl;
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		NEW_USER_JOIN _new_user_join;
		recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);
		std::cout << "new user is " << _new_user_join.sock << std::endl;
	}
	break;
	default:
		std::cout << "��֧�ֵ�����" << std::endl;
		break;
	}
}

//��������
void Client::CmdThread()
{
	
	while (true)
	{
		char _cmd_buf[256];
		std::cin >> _cmd_buf;
		if (0 == strcmp(_cmd_buf, "exit"))
		{
			g_bRun = false;//�������߳̿ͻ���Ҫ�˳�
			std::cout << "�ͻ����˳�" << std::endl;
			return;
		}
		else if (0 == strcmp(_cmd_buf, "login"))
		{
			LOGIN login;
			strcpy(login._user_name, "kzj");
			strcpy(login._user_password, "123456");
			send(_client_sock, (const char*)& login, sizeof(login), 0);
		}
		else if (0 == strcmp(_cmd_buf, "logout"))
		{
			LOGOUT logout;
			strcpy(logout._user_name, "kzj");
			strcpy(logout._user_password, "123456");
			send(_client_sock, (const char*)& logout, sizeof(logout), 0);
		}
		else
		{
			std::cout << "��Ч���룬����������" << std::endl;
		}
	}	
}

void Client::InitClient()
{
	//����һ��socket,ipv4���������ӵģ�tcpЭ��
	_client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_client_sock < 0)
		std::cout << "Client socket error. " << std::endl;
	else
		std::cout << "Client socket build. " << std::endl;

	//���ӷ����� connect
	struct sockaddr_in _client_addr;
	_client_addr.sin_addr.S_un.S_addr = inet_addr(CLIENT_IP);//inet_addrҪ�ر�SDL
	_client_addr.sin_port = htons(PORT);
	_client_addr.sin_family = AF_INET;
	if (connect(_client_sock, (sockaddr*)& _client_addr, sizeof(_client_addr)) < 0)
		std::cout << "Client connect error. " << std::endl;
	else
		std::cout << "Client connect build. " << std::endl;
}

void Client::SendRequenst()
{
	std::thread t_cmd(CmdThread);//����������߳�
	t_cmd.detach();//�̷߳���
	//����select
	while (g_bRun)
	{
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(_client_sock, &fd_read);

		timeval _time_val;
		_time_val.tv_sec = 1;
		_time_val.tv_usec = 0;
		int ret = select(_client_sock, &fd_read, NULL, NULL, &_time_val);
		if (ret < 0)
		{
			std::cout << "select �������" << std::endl;
			break;
		}

		//�ͻ��˺ͷ����һ��һ����
		if (FD_ISSET(_client_sock, &fd_read))
		{
			FD_CLR(_client_sock, &fd_read);
			if (-1 == Processor(_client_sock))
			{
				std::cout << "select �������" << std::endl;
				break;
			}
		}

		std::cout << "do other things." << std::endl;
	}
}

void Client::CloseClient()
{
	//�ر�socket  
	closesocket(_client_sock);
}

