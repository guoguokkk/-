#define WIN32_LEAN_AND_MEAN//½â¾ö<Windows.h>ºÍ<WinSock2.h>Ã¬¶Ü
#include"../Test/Common.h"
#include"Client.h"
int main()
{
	WORD _version = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_version, &_data);

	Client client;
	client.InitClient();
	client.SendRequenst();
	client.CloseClient();
	
	getchar();
	WSACleanup();
	return 0;
}