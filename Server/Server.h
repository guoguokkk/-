#ifndef SERVER_H
#define SERVER_H
#ifdef _WIN32
#include"../Test/Common.h"
#else
#include"Common.h"
#endif // _WIN32
#include<vector>
class Server
{
public:
	Server();
	virtual ~Server();
	void InitServer();//初始化服务器
	int Bind(const char* ip, const unsigned short port);//绑定地址和端口
	void Listen(int n);//监听
	void Accept();//接收客户端
	void CloseServer();//关闭服务器
	bool OnRun();//执行
	bool IsRun();//是否正常执行
	int RecvData(int client_sock);//接受数据
	void OnNetMsg(int client_sock,Header* header, char* recv_buf);//处理消息
	int SendData(int client_sock, Header* header);//发送消息
	void SendData2All(Header* header);//广播消息
private:
	int _server_sock;
	std::vector<int> _group_clients;
};

#endif // !SERVER_H
