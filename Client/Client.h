#ifndef CLIENT_H
#define CLIENT_H
#ifdef _WIN32
#include"../Test/Common.h"
#else
#include"Common.h"
#endif // _WIN32
#include<vector>
class Client
{
public:
	Client();
	virtual ~Client();
	void InitClient();//初始化客户端
	int Connect(const char* ip, const unsigned short port);//连接服务器
	void CloseClient();//关闭客户端
	bool OnRun();//查询
	bool IsRun();//判断是否可以正常运行
	int RecvData();//接收数据
	int SendData(Header* header);//发送
	void OnNetMsg(Header* header, char* recv_buf);//处理网络消息
private:
	int _client_sock;
};

#endif // !CLIENT_H
