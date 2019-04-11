#ifndef SERVER_H
#define SERVER_H
#ifdef _WIN32
#include"../Test/Common.h"
#else
#include"Common.h"
#endif // _WIN32
#include<vector>
#include"ClientSocket.h"

//new 堆内存，否则在栈空间（很小，1-2M）
class Server {
public:
	Server(SOCKET server_sock = INVALID_SOCKET) :_server_sock(server_sock) { InitServer(); }
	virtual ~Server() 
	{ 
		CloseServer();
		_server_sock = INVALID_SOCKET;
	}
	int Bind(const char* ip, const unsigned short port);//绑定地址和端口
	int Listen(int n);//监听	
	void CloseServer();//关闭服务器
	bool OnRun();//select accept
	int RecvData(ClientSocket* client);//接收数据 处理粘包 拆分包	
	int SendData(SOCKET client_sock, Header* header);//发送消息
	void SendData2All(Header* header);//广播消息
private:
	int _server_sock;
	std::vector<ClientSocket*> _clients;//new申请空间，不会爆栈
	char _recv_buf[RECV_BUF_SIZE];//接收缓冲区
	SOCKET InitServer();//初始化服务器
	SOCKET Accept();//接收客户端
	virtual void OnNetMsg(SOCKET client_sock, Header* header);//处理消息	
	bool IsRun();//是否正常执行
};

#endif // !SERVER_H
