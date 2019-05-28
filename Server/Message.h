#ifndef MESSAGE_H_
#define MESSAGE_H_

#ifdef _WIN32
#define FD_SETSIZE 10056
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

#define IP "202.114.7.16"
#define PORT 8080

//缓冲区最小单元大小
#ifndef RECV_BUF_SIZE
#define RECV_BUF_SIZE 10240
#define SEND_BUF_SIZE 1024000
#endif // !RECV_BUF_SIZE

//命令
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR,
	CMD_NEW_USER_JOIN,
	CMD_C2S_HEART,
	CMD_S2C_HEART
};

//消息头
struct netmsg_Header
{
	netmsg_Header()
	{
		dataLength = sizeof(netmsg_Header);
		cmd = CMD_ERROR;
	}
	unsigned short dataLength;//消息的长度
	unsigned short cmd;//命令	
};

//登录消息
struct  netmsg_Login :public netmsg_Header
{
	netmsg_Login()
	{
		dataLength = sizeof(netmsg_Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
	char data[32];
};

//登录结果消息
struct  netmsg_LoginResult :public netmsg_Header
{
	netmsg_LoginResult()
	{
		dataLength = sizeof(netmsg_LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
	char data[92];
};

//登出消息
struct  netmsg_Logout :public netmsg_Header
{
	netmsg_Logout()
	{
		dataLength = sizeof(netmsg_Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

//登出结果消息
struct  netmsg_LogoutResult :public netmsg_Header
{
	netmsg_LogoutResult()
	{
		dataLength = sizeof(netmsg_LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

//新用户加入
struct  netmsg_NewUserJoin :public netmsg_Header
{
	netmsg_NewUserJoin()
	{
		dataLength = sizeof(netmsg_NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};

//心跳消息，客户端发送给服务器
struct  netmsg_c2s_Heart :public netmsg_Header
{
	netmsg_c2s_Heart()
	{
		dataLength = sizeof(netmsg_c2s_Heart);
		cmd = CMD_C2S_HEART;
	}
};

//心跳消息，服务器发送给客户端
struct  netmsg_s2c_Heart :public netmsg_Header
{
	netmsg_s2c_Heart()
	{
		dataLength = sizeof(netmsg_s2c_Heart);
		cmd = CMD_S2C_HEART;
	}
};

#endif // !MESSAGE_H_
