#define WIN32_LEAN_AND_MEAN//解决<Windows.h>和<WinSock2.h>矛盾
#include<iostream>
#include<Windows.h>
#include<WinSock2.h>
#define PORT 8082
#define CLIENT_IP "127.0.0.1"

enum CMD_LINE
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};

//数据报头部
struct Header
{
	int data_length;//数据长度
	int cmd;//命令
};

struct LOGIN :public Header
{
	LOGIN()
	{
		data_length = sizeof(LOGIN);
		cmd = CMD_LOGIN;
	}
	char _user_name[32];
	char _user_password[32];
};

struct LOGIN_RESULT :public Header
{
	LOGIN_RESULT()
	{
		data_length = sizeof(LOGIN_RESULT);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct LOGOUT :public Header
{
	LOGOUT()
	{
		data_length = sizeof(LOGOUT);
		cmd = CMD_LOGOUT;
	}
	char _user_name[32];
	char _user_password[32];
};

struct LOGOUT_RESULT :public Header
{
	LOGOUT_RESULT()
	{
		data_length = sizeof(LOGOUT_RESULT);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
int main()
{
	WORD _version = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_version, &_data);

	//建立一个socket,ipv4，面向连接的，tcp协议
	int _client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

	//发送命令给服务器 send	
	while (true)
	{
		std::string _cmd_buf;
		std::cin >> _cmd_buf;
		if (_cmd_buf == "exit")
		{
			break;
		}
		else if (_cmd_buf == "login")
		{
			//向服务器发送请求命令
			LOGIN _login;
			strcpy(_login._user_name, "kzj");
			strcpy(_login._user_password, "123456");
			send(_client_sock, (const char*)& _login, sizeof(_login), 0);

			//接受服务器返回的数据
			LOGIN_RESULT _login_result;
			recv(_client_sock, (char*)& _login_result, sizeof(_login_result), 0);
			std::cout << "login result: " << _login_result.result << std::endl;
		}
		else if (_cmd_buf == "logout")
		{
			//向服务器发送请求命令
			LOGOUT _logout;
			strcpy(_logout._user_name, "kzj");
			strcpy(_logout._user_password, "123456");
			send(_client_sock, (const char*)& _logout, sizeof(_logout), 0);

			//接受服务器返回的数据
			LOGIN_RESULT _logout_result;
			recv(_client_sock, (char*)& _logout_result, sizeof(_logout_result), 0);
			std::cout << "logout result: " << _logout_result.result << std::endl;
		}
	}

	//关闭socket  
	closesocket(_client_sock);

	WSACleanup();
	return 0;
}