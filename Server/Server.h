#ifndef SERVER_H
#define SERVER_H
#include"../Test/Common.h"
#include<vector>
class Server
{
public:
	Server();
	~Server();
	void CloseServer();
	void InitServer();
	SOCKET GetSock() { return _server_sock; }
	int processor(SOCKET server_sock);
	void HandleClientRequest();
private:
	SOCKET _server_sock;
	std::vector<int> _group_clients;
};

#endif // !SERVER_H
