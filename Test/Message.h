#ifndef MESSAGE_H_
#define MESSAGE_H_
#ifdef _WIN32
#define FD_SETSIZE 4024
#define WIN32_LEAN_AND_MEAN//解决 Windows.h 和 WinSock2.h 冲突
#include<Windows.h>
#include<WinSock2.h>
#else
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR -1
#endif // _WIN32
#define IP "127.0.0.1"
#define PORT 8800
#ifndef BUF_SIZE
#define RECV_BUF_SIZE 10240
#endif // !BUF_SIZE
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
	Header() :cmd(CMD_ERROR), data_length(sizeof(Header)) {}
	short cmd;//命令
	short data_length;//整个的长度
};

//登录消息
struct  Login :public Header
{
	Login()
	{
		cmd = CMD_LOGIN;
		data_length = sizeof(Login);
	}
	char name[32];
	char password[32];
	char data[932];
};

//登录结果消息
struct  LoginResult :public Header
{
	LoginResult()
	{
		cmd = CMD_LOGIN_RESULT;
		data_length = sizeof(LoginResult);
	}
	short result = 0;
};

//登出消息
struct  Logout :public Header
{
	Logout()
	{
		cmd = CMD_LOGOUT;
		data_length = sizeof(Logout);
	}
	char name[32];
	char data[964];
};

//登出结果消息
struct  LogoutResult :public Header
{
	LogoutResult()
	{
		cmd = CMD_LOGOUT_RESULT;
		data_length = sizeof(LogoutResult);
	}
	short result = 0;
};

//新用户加入
struct  NewUserJoin :public Header
{
	NewUserJoin()
	{
		cmd = CMD_NEW_USER_JOIN;
		data_length = sizeof(NewUserJoin);
	}
	short sock = 0;
};
#endif // !MESSAGE_H_
