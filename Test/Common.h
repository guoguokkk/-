#ifndef COMMON_H
#define COMMON_H
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define FD_SETSIZE 64
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#include<Windows.h>
#include<WinSock2.h>
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#endif // _WIN32
#include<string>
#ifndef RECV_BUF_SIZE
#define RECV_BUF_SIZE 10240 //缓冲区区域最小单元大小
#endif // !RECV_BUF_SIZE
#define PORT 8304
//#define SERVER_IP "202.118.19.190"//win ws
#define SERVER_IP "202.114.7.16"//win kzj
//#define SERVER_IP "222.20.79.232"//linux
//#define SERVER_IP "127.0.0.1"
//命令
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_UER_JOIN,
	CMD_ERROR
};

//数据头
struct Header
{
	Header()
	{
		data_length = sizeof(Header);
		cmd = CMD_ERROR;
	}
	short cmd;//cmd 命令
	short data_length;//数据总长度
};

//数据包
struct Login :public Header
{
	Login()
	{
		data_length = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char name[32];//姓名
	char password[32];//密码
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
	int result;//登录结果
	char data[992];
};

struct Logout :public Header
{
	Logout()
	{
		data_length = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char name[32];//姓名
};

struct LogoutResult :public Header
{
	LogoutResult()
	{
		cmd = CMD_LOGOUT_RESULT;
		data_length = sizeof(LogoutResult);
		result = 0;
	}
	int result;//登出结果
};

struct NewUserJoin :public Header
{
	NewUserJoin()
	{
		data_length = sizeof(NewUserJoin);
		cmd = CMD_NEW_UER_JOIN;
		sock = 0;
	}
	int sock;//新加入的用户
};

#endif // !COMMON_H
