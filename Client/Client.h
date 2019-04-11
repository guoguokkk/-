#ifndef CLIENT_H
#define CLIENT_H
#ifdef _WIN32
#include"../Test/Common.h"
#else
#include"Common.h"
#endif // _WIN32
#include<vector>
class Client {
public:
	Client();
	virtual ~Client();
	int Connect(const char* ip, const unsigned short port);//连接服务器
	void CloseClient();//关闭客户端
	void InitClient();//初始化客户端	
	bool OnRun();//select
	int RecvData(SOCKET client_sock);//接收数据,处理粘包,拆分包
	int SendData(Header* header);//发送	
private:
	SOCKET _client_sock;
	char _recv_buf[RECV_BUF_SIZE];//接收缓冲区
	char _msg_buf[RECV_BUF_SIZE * 10];//消息缓冲区
	int _last_pos = 0;//上次存放的位置（消息缓冲区）	
	void OnNetMsg(Header* header);//处理网络消息
	bool IsRun();//判断是否可以正常运行	
};

#endif // !CLIENT_H
