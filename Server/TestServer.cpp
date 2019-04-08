#define WIN32_LEAN_AND_MEAN//���<Windows.h>��<WinSock2.h>ì��
#include<iostream>
#include<string>
#include<Windows.h>
#include<WinSock2.h>
#define PORT 8082
#define SERVER_IP "127.0.0.1"

enum CMD_LINE
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};

//���ݱ�ͷ��
struct Header
{
	int data_length;//���ݳ���
	int cmd;//����
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

	//����һ��socket,ipv4���������ӵģ�tcpЭ��
	int _server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_sock < 0)
		std::cout << "Server socket error. " << std::endl;
	else
		std::cout << "Server socket build. " << std::endl;

	//�󶨽��ܿͻ������ӵĶ˿�bind 
	struct sockaddr_in _server_addr;
	_server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);//inet_addrҪ�ر�SDL
	_server_addr.sin_port = htons(PORT);
	_server_addr.sin_family = AF_INET;
	if (bind(_server_sock, (sockaddr*)& _server_addr, sizeof(_server_addr)) < 0)
		std::cout << "Server bind error. " << std::endl;
	else
		std::cout << "Server bind build. " << std::endl;

	//��������˿�listen 
	if (listen(_server_sock, 5) < 0)
		std::cout << "Server listen error. " << std::endl;
	else
		std::cout << "Server listen build. " << std::endl;

	//�ȴ����ܿͻ�������accept	
	struct sockaddr_in _client_addr;
	int _client_addr_size = sizeof(_client_addr);
	int _client_sock = accept(_server_sock, (sockaddr*)& _client_addr, &_client_addr_size);
	if (_client_sock < 0)
		std::cout << "Server accept error. " << std::endl;
	else
		std::cout << "New Client " << _client_sock << ", ip: " << inet_ntoa(_client_addr.sin_addr) << std::endl;//inet_ntoa


	//�����������������������
	while (true)
	{
		Header _header;
		int _length_header = recv(_client_sock, (char*)& _header, sizeof(_header), 0);
		if (_length_header <= 0)
		{
			std::cout << "cmd is exit" << std::endl;
			break;
		}
		switch (_header.cmd)
		{
		case CMD_LOGIN:
		{
			std::cout << "cmd is login" << std::endl;
			//���ܿͻ�������
			LOGIN _login;
			recv(_client_sock, (char*)& _login + sizeof(_header),
				sizeof(_login) - sizeof(_header), 0);//ע�ⳤ��
			std::cout << "username is: " << _login._user_name << " ,password: "
				<< _login._user_password << std::endl;

			//���ؽ��
			LOGIN_RESULT _login_result;
			send(_client_sock, (const char*)& _login_result, sizeof(LOGIN_RESULT), 0);

		}
		break;
		case CMD_LOGOUT:
		{
			std::cout << "cmd is logout" << std::endl;
			//���ܿͻ�������
			LOGIN _logout;
			recv(_client_sock, (char*)& _logout + sizeof(_header),
				sizeof(_logout) - sizeof(_header), 0);//ע�ⳤ��
			std::cout << "username is: " << _logout._user_name << " ,password: "
				<< _logout._user_password << std::endl;

			//���ؽ��
			LOGIN_RESULT _logout_result;
			send(_client_sock, (const char*)& _logout_result, sizeof(_logout_result), 0);
			break;
		}
		default:
			break;
		}
	}

	//�ر�socket  
	closesocket(_server_sock);

	WSACleanup();
	return 0;
}