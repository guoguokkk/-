#include"Server.h"

int main()
{
#ifdef _WIN32
	WORD _version = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_version, &_data);
#endif

	Server server;
	server.InitServer();
	server.HandleClientRequest();//服务端升级为select模型处理多客户端
	server.CloseServer();

#ifdef _WIN32
	getchar();
	WSACleanup();
#endif
	return 0;
}