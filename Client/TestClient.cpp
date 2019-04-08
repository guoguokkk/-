#define WIN32_LEAN_AND_MEAN//���<Windows.h>��<WinSock2.h>ì��
#include<iostream>
#include<Windows.h>
#include<WinSock2.h>
#define PORT 8082
#define CLIENT_IP "127.0.0.1"
int main()
{
	WORD _version = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_version, &_data);

	//����һ��socket,ipv4���������ӵģ�tcpЭ��
	int _client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_client_sock < 0)
		std::cout << "Client socket error. " << std::endl;
	else
		std::cout << "Client socket build. " << std::endl;

	//���ӷ����� connect
	struct sockaddr_in _client_addr;
	_client_addr.sin_addr.S_un.S_addr = inet_addr(CLIENT_IP);//inet_addrҪ�ر�SDL
	_client_addr.sin_port = htons(PORT);
	_client_addr.sin_family = AF_INET;
	int _server_sock = connect(_client_sock, (sockaddr*)& _client_addr, sizeof(_client_addr));
	if (_server_sock < 0)
		std::cout << "Client connect error. " << std::endl;
	else
		std::cout << "Client connect build. " << std::endl;

	//���շ�������Ϣ recv
	char _recv_buf[128];
	if (recv(_client_sock, _recv_buf, sizeof(_recv_buf), 0) < 0)
		std::cout << "Client recv error. " << std::endl;
	else
		std::cout << "Data: " << _recv_buf << std::endl;

	//�ر�socket  
	closesocket(_client_sock);

	WSACleanup();
	return 0;
}