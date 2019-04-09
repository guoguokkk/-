#ifndef COMMOC_H
#define COMMOC_H
#define WIN32_LEAN_AND_MEAN//解决<Windows.h>和<WinSock2.h>矛盾
#include<iostream>
#include<Windows.h>
#include<WinSock2.h>
#include<string>
#define PORT 8082
#define SERVER_IP "127.0.0.1"
#define CLIENT_IP "127.0.0.1"

enum CMD_LINE
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR,
	CMD_NEW_USER_JOIN//其他用户加入
};

//数据报头部
struct Header
{
	int data_length;//数据长度
	int cmd;//命令
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

struct NEW_USER_JOIN :public Header
{
	NEW_USER_JOIN()
	{
		data_length = sizeof(NEW_USER_JOIN);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;//新用户id
};

static void HandleError(std::string _error_s)
{
	std::cout << _error_s << std::endl;
}

static void HandleSuccess(std::string _success_s)
{
	std::cout << _success_s << std::endl;
}
#endif // !COMMOC_H
