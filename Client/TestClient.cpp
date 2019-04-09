#define WIN32_LEAN_AND_MEAN//解决<Windows.h>和<WinSock2.h>矛盾
#include<iostream>
#include<Windows.h>
#include<WinSock2.h>
#include"../Test/Common.h"

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
	getchar();
	WSACleanup();
	return 0;
}