#define WIN32_LEAN_AND_MEAN//���<Windows.h>��<WinSock2.h>ì��
#include<iostream>
#include<string>
#include<Windows.h>
#include<WinSock2.h>
#include<vector>
#include"../Test/Common.h"
#include"Server.h"

int main()
{
	WORD _version = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_version, &_data);

	Server server;
	server.InitServer();	
	server.HandleClientRequest();//���������Ϊselectģ�ʹ����ͻ���
	server.CloseServer();

	WSACleanup();
	return 0;
}