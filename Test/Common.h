#ifndef COMMON_H
#define COMMON_H
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<WinSock2.h>
#else
#include<unistd>
#include<arpr/inet.h>
#include<string.h>
#endif // _WIN32
#include<string>

#define PORT 8087
#define SERVER_IP "202.114.7.16"//win 台式机
//#define SERVER_IP "222.20.79.232"//linux
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
	int cmd;//命令
	int data_length;//数据长度
};

struct Login :public Header
{
	Login()
	{
		cmd = CMD_LOGIN;
		data_length = sizeof(Login);
	}
	char name[32];//用户名
	char password[32];//密码
};

struct LoginResult :public Header
{
	LoginResult()
	{
		cmd = CMD_LOGIN_RESULT;
		data_length = sizeof(LoginResult);
	}
	int result = 0;//登录结果
};

struct Logout :public Header
{
	Logout()
	{
		cmd = CMD_LOGOUT;
		data_length = sizeof(Logout);
	}
	char name[32];//用户名
};

struct LogoutResult :public Header
{
	LogoutResult()
	{
		cmd = CMD_LOGOUT_RESULT;
		data_length = sizeof(LogoutResult);
	}
	int result = 0;//登出结果
};

struct NewUserJoin :public Header
{
	NewUserJoin()
	{
		cmd = CMD_NEW_UER_JOIN;
		data_length = sizeof(NewUserJoin);
	}
	int sock = 0;//新用户
};

#endif // !COMMON_H
