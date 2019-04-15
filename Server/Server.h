#ifndef SERVER_H_
#define SERVER_H_
#ifdef _WIN32
#include"../Test/Message.h"
#include"../Test/TimeStamp.h"
#else
#include"Message.h"
#include"TimeStamp.h"
#endif // _WIN32
#include<vector>
#include"ClientSocket.h"

class Server
{
public:
	Server(SOCKET server_sock = INVALID_SOCKET) :_server_sock(server_sock)
	{
		InitServer();
	}
	~Server();
	void InitServer();//初始化服务器
	void CloseServer();//关闭服务器
	void Bind(const char* ip, const short port);//绑定ip地址和端口
	void Listen(int n);//监听客户端
	void Accept();//接收客户端
	bool OnRun();//select
	void SendToAll(Header* header);//向所有客户端发送消息
	void SendToOne(SOCKET client_sock, Header* header);//向某个客户端发送消息
	int RecvMessages(ClientSock* client);//接收消息
	void HandleMessages(SOCKET client_sock, Header* header);//处理消息
private:
	SOCKET _server_sock;
	std::vector<ClientSock*> _client_groups;//存储所有的客户端
	char _recv_buf[RECV_BUF_SIZE];//接收缓冲区
	TimeStamp _tTime;
	int _recv_count;
};
#endif // !SERVER_H_
