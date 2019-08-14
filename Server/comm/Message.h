#ifndef MESSAGE_H_
#define MESSAGE_H_

//命令类型
enum CMD
{
	CMD_LOGIN,//登录
	CMD_LOGIN_RESULT,//登录结果
	CMD_LOGOUT,//登出
	CMD_LOGOUT_RESULT,//登出结果
	CMD_ERROR,//错误
	CMD_NEW_USER_JOIN,//新用户加入
	CMD_C2S_HEART,//客户端发给服务器端的心跳消息
	CMD_S2C_HEART//服务器端发给客户端的心跳消息
};

//消息头(消息总长度、命令类型)
struct netmsg_DataHeader
{
	//构造函数
	netmsg_DataHeader()
	{
		dataLength = sizeof(netmsg_DataHeader);
		cmd = CMD_ERROR;
	}
	unsigned short dataLength;//消息总长度
	unsigned short cmd;//命令类型	
};

//登录消息-100字节：消息头(消息总长度、命令类型)、用户名、密码、数据、消息ID
struct netmsg_Login :public netmsg_DataHeader
{
	//构造函数
	netmsg_Login()
	{
		dataLength = sizeof(netmsg_Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
	char getData[28];
	int msgID;
};

//登录结果消息-100字节：消息头(消息总长度、命令类型)、登录结果、数据、消息ID
struct netmsg_LoginResult :public netmsg_DataHeader
{
	//构造函数
	netmsg_LoginResult()
	{
		dataLength = sizeof(netmsg_LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
	char getData[88];
	int msgID;
};

//登出消息-40字节：消息头(消息总长度、命令类型)、用户名
struct netmsg_Logout :public netmsg_DataHeader
{
	//构造函数
	netmsg_Logout()
	{
		dataLength = sizeof(netmsg_Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
	int msgID;
};

//登出结果消息-8字节：消息头(消息总长度、命令类型)、结果
struct  netmsg_LogoutResult :public netmsg_DataHeader
{
	//构造函数
	netmsg_LogoutResult()
	{
		dataLength = sizeof(netmsg_LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

//新用户加入消息-8字节：消息头(消息总长度、命令类型)、新加入用户的描述符
struct  netmsg_NewUserJoin :public netmsg_DataHeader
{
	//构造函数
	netmsg_NewUserJoin()
	{
		dataLength = sizeof(netmsg_NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};

//客户端发给服务器端的心跳消息-4字节：消息头(消息总长度、命令类型)
struct  netmsg_c2s_Heart :public netmsg_DataHeader
{
	//构造函数
	netmsg_c2s_Heart()
	{
		dataLength = sizeof(netmsg_c2s_Heart);
		cmd = CMD_C2S_HEART;
	}
};

//服务器端发给客户端的心跳消息-4字节：消息头(消息总长度、命令类型)
struct  netmsg_s2c_Heart :public netmsg_DataHeader
{
	//构造函数
	netmsg_s2c_Heart()
	{
		dataLength = sizeof(netmsg_s2c_Heart);
		cmd = CMD_S2C_HEART;
	}
};

#endif // !MESSAGE_H_
