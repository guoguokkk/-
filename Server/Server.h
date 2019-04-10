#ifndef SERVER_H
#define SERVER_H
#include"../Test/Common.h"
#include<vector>
class Server
{
public:
	Server();
	virtual ~Server();
	void InitServer();//初始化服务器
	int Bind(const char* ip, const unsigned short port);//绑定ip和端口号
	void Listen(int n);//监听端口
	void Accept();//接收客户端连接
	void CloseServer();//关闭客户端
	bool OnRun();//查询
	bool IsRun();//判断是否可以正常运行
	int RecvData(int client_sock);//接收数据
	void OnNetMsg(int client_sock,Header* header, char* recv_buf);//处理网络消息
	int SendData2All(int client_sock, Header* header);//单发
	void SendData2All(Header* header);//群发
private:
	int _server_sock;
	std::vector<int> _group_clients;
};

#endif // !SERVER_H
