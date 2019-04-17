#ifndef MESSAGE_H_
#define MESSAGE_H_

#ifdef _WIN32
#define FD_SETSIZE 2056
#define WIN32_LEAN_AND_MEAN//解决 Windows.h 和 WinSock2.h 冲突
#include<Windows.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#else
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR -1
#endif // _WIN32

#define IP "127.0.0.1"
#define PORT 8080

//缓冲区最小单元大小
#ifndef RECV_BUF_SIZE
#define RECV_BUF_SIZE 10240
#endif // !RECV_BUF_SIZE

//命令
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR,
	CMD_NEW_USER_JOIN
};

//消息头
struct Header
{
	Header() 
	{
		data_length = sizeof(Header);
		cmd = CMD_ERROR;		
	}
	short data_length;//消息的长度
	short cmd;//命令	
};

//登录消息
struct  Login :public Header
{
	Login()
	{
		data_length = sizeof(Login);
		cmd = CMD_LOGIN;		
	}
	char userName[32];
	char passWord[32];
	char data[32];
};

//登录结果消息
struct  LoginResult :public Header
{
	LoginResult()
	{
		data_length = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;		
		result = 0;
	}
	int result;
	char data[92];
};

//登出消息
struct  Logout :public Header
{
	Logout()
	{		
		data_length = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

//登出结果消息
struct  LogoutResult :public Header
{
	LogoutResult()
	{		
		data_length = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

//新用户加入
struct  NewUserJoin :public Header
{
	NewUserJoin()
	{
		data_length = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;		
		sock = 0;
	}
	int sock;
};
#endif // !MESSAGE_H_
