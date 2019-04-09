#define WIN32_LEAN_AND_MEAN//解决<Windows.h>和<WinSock2.h>矛盾
#include"Server.h"

int main()
{
	WORD _version = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_version, &_data);

	Server server;
	server.InitServer();	
	server.HandleClientRequest();//服务端升级为select模型处理多客户端
	server.CloseServer();

	WSACleanup();
	return 0;
}