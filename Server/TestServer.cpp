#define WIN32_LEAN_AND_MEAN//解决<Windows.h>和<WinSock2.h>矛盾
#include<iostream>
#include<Windows.h>
#include<WinSock2.h>
#define PORT 8082
#define SERVER_IP "127.0.0.1"
int main()
{
	WORD _version = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_version, &_data);

	//建立一个socket,ipv4，面向连接的，tcp协议
	int _server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_sock < 0)
		std::cout << "Server socket error. " << std::endl;
	else
		std::cout << "Server socket build. " << std::endl;

	//绑定接受客户端连接的端口bind 
	struct sockaddr_in _server_addr;
	_server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);//inet_addr要关闭SDL
	_server_addr.sin_port = htons(PORT);
	_server_addr.sin_family = AF_INET;
	if (bind(_server_sock, (sockaddr*)& _server_addr, sizeof(_server_addr)) < 0)
		std::cout << "Server bind error. " << std::endl;
	else
		std::cout << "Server bind build. " << std::endl;

	//监听网络端口listen 
	if (listen(_server_sock, 5) < 0)
		std::cout << "Server listen error. " << std::endl;
	else
		std::cout << "Server listen build. " << std::endl;

	//等待接受客户端连接accept	
	struct sockaddr_in _client_addr;
	int _client_addr_size = sizeof(_client_addr);
	int _client_sock = accept(_server_sock, (sockaddr*)& _client_addr, &_client_addr_size);
	if (_client_sock < 0)
		std::cout << "Server accept error. " << std::endl;
	else
		std::cout << "New Client " << _client_sock << ", ip: " << inet_ntoa(_client_addr.sin_addr) << std::endl;//inet_ntoa

	//向客户端发送一条数据send 
	char _send_buf[] = { "hello, this is server. " };
	if (send(_client_sock, _send_buf, sizeof(_send_buf), 0) < 0)
		std::cout << "Server send error. " << std::endl;
	else
		std::cout << "Server send: " << _send_buf << std::endl;

	//关闭socket  
	closesocket(_server_sock);

	WSACleanup();
	return 0;
}