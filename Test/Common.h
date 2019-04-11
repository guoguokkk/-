#ifndef COMMON_H
#define COMMON_H
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define FD_SETSIZE 1024
#include<Windows.h>
#include<WinSock2.h>
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
typedef int SOCKET;
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#endif // _WIN32
#include<string>
#ifndef RECV_BUF_SIZE
#define RECV_BUF_SIZE 10240 
#endif // !RECV_BUF_SIZE
#define PORT 8010
//#define SERVER_IP "202.118.19.190"//win ws
#define SERVER_IP "202.114.7.16"//win kzj
//#define SERVER_IP "222.20.79.232"//linux
//#define SERVER_IP "127.0.0.1"
//#define SERVER_IP "192.168.153.129"

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_UER_JOIN,
	CMD_ERROR
};

struct Header
{
	Header()
	{
		data_length = sizeof(Header);
		cmd = CMD_ERROR;
	}
	short cmd;
	short data_length;
};


struct Login :public Header
{
	Login()
	{
		data_length = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char name[32];
	char password[32];
	char data[932];
};

struct LoginResult :public Header
{
	LoginResult()
	{
		data_length = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
	char data[992];
};

struct Logout :public Header
{
	Logout()
	{
		data_length = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char name[32];
};

struct LogoutResult :public Header
{
	LogoutResult()
	{
		cmd = CMD_LOGOUT_RESULT;
		data_length = sizeof(LogoutResult);
		result = 0;
	}
	int result;
};

struct NewUserJoin :public Header
{
	NewUserJoin()
	{
		data_length = sizeof(NewUserJoin);
		cmd = CMD_NEW_UER_JOIN;
		sock = 0;
	}
	int sock;
};

#endif // !COMMON_H
