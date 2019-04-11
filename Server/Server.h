#ifndef SERVER_H
#define SERVER_H
#ifdef _WIN32
#include"../Test/Common.h"
#else
#include"Common.h"
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif // _WIN32
#include<vector>
class ClientSocket {
public:
	ClientSocket(int sockfd = INVALID_SOCKET)
	{
		_sockfd = sockfd;
		memset(_msg_buf, 0, sizeof(_msg_buf));//清空消息缓冲区
		_last_pos = 0;
	}
	int GetSockfd() const { return _sockfd; }
	char* GetMsgBuf() { return _msg_buf; }
	int GetLastPos() const { return _last_pos; }
	void SetLastPos(int pos) { _last_pos = pos; }
private:
	int _sockfd;//文件描述符
	char _msg_buf[RECV_BUF_SIZE * 10];//消息缓冲区
	int _last_pos = 0;//上次存放的位置（消息缓冲区）
};

//new 堆内存，否则在栈空间（很小，1-2M）
class Server {
public:
	Server(SOCKET server_sock = INVALID_SOCKET) :_server_sock(server_sock) { InitServer(); }
	virtual ~Server() 
	{ 
		CloseServer();
		_server_sock = INVALID_SOCKET;
	}

	int InitServer();//初始化服务器
	int Bind(const char* ip, const unsigned short port);//绑定地址和端口
	void Listen(int n);//监听	
	void CloseServer();//关闭服务器
	bool OnRun();//处理网络消息
	bool IsRun();//是否正常执行
	int RecvData(ClientSocket* client);//接收数据 处理粘包 拆分包
	
	int SendData(int client_sock, Header* header);//发送消息
	void SendData2All(Header* header);//广播消息
private:
	int _server_sock;
	std::vector<ClientSocket*> _clients;//new申请空间，不会爆栈
	char _recv_buf[RECV_BUF_SIZE];//接收缓冲区
	SOCKET Accept();//接收客户端
	void OnNetMsg(int client_sock, Header* header);//处理消息
};

#endif // !SERVER_H
