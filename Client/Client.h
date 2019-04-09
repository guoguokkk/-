#ifndef CLIENT_H
#define CLIENT_H
#include"../Test/Common.h"
class Client
{
public:
	Client();
	~Client();
	void InitClient();
	void SendRequenst();
	void CloseClient();
	int processor(SOCKET _client_sock);
private:
	SOCKET _client_sock;
};

#endif // !CLIENT_H
