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
		std::cout << "与服务器断开连接." << std::endl;
		return -1;
	}
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		//接受服务器返回的数据
		LOGIN_RESULT _login_result;
		recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);
		std::cout << "收到服务端消息: CMD_LOGIN_RESULT，数据长度为 " << _login_result.data_length << std::endl;
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		//接受服务器返回的数据
		LOGIN_RESULT _logout_result;
		recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);
		std::cout << "收到服务端消息: CMD_LOGIN_RESULT，数据长度为 " << _logout_result.data_length << std::endl;
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
		std::cout << "不支持的命令" << std::endl;
		break;
	}
}

//输入命令
void Client::CmdThread()
{
	
	while (true)
	{
		char _cmd_buf[256];
		std::cin >> _cmd_buf;
		if (0 == strcmp(_cmd_buf, "exit"))
		{
			g_bRun = false;//告诉主线程客户端要退出
			std::cout << "客户端退出" << std::endl;
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
			std::cout << "无效输入，请重新输入" << std::endl;
		}
	}	
}

void Client::InitClient()
{
	//建立一个socket,ipv4，面向连接的，tcp协议
	_client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_client_sock < 0)
		std::cout << "Client socket error. " << std::endl;
	else
		std::cout << "Client socket build. " << std::endl;

	//连接服务器 connect
	struct sockaddr_in _client_addr;
	_client_addr.sin_addr.S_un.S_addr = inet_addr(CLIENT_IP);//inet_addr要关闭SDL
	_client_addr.sin_port = htons(PORT);
	_client_addr.sin_family = AF_INET;
	if (connect(_client_sock, (sockaddr*)& _client_addr, sizeof(_client_addr)) < 0)
		std::cout << "Client connect error. " << std::endl;
	else
		std::cout << "Client connect build. " << std::endl;
}

void Client::SendRequenst()
{
	std::thread t_cmd(CmdThread);//输入命令的线程
	t_cmd.detach();//线程分离
	//加入select
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
			std::cout << "select 任务结束" << std::endl;
			break;
		}

		//客户端和服务端一对一连接
		if (FD_ISSET(_client_sock, &fd_read))
		{
			FD_CLR(_client_sock, &fd_read);
			if (-1 == Processor(_client_sock))
			{
				std::cout << "select 任务结束" << std::endl;
				break;
			}
		}

		std::cout << "do other things." << std::endl;
	}
}

void Client::CloseClient()
{
	//关闭socket  
	closesocket(_client_sock);
}

