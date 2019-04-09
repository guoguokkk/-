#ifndef CLIENT_H
#define CLIENT_H

#ifdef _WIN32
#include"../Test/Common.h"
#else
#include"Common.h"
#endif // _WIN32

class Client
{
public:
	Client();
	~Client();
	void InitClient();
	void SendRequenst();
	void CloseClient();
	int Processor(SOCKET _client_sock);
	static void CmdThread();
private:
	static SOCKET _client_sock;
	static bool g_bRun;
};

#endif // !CLIENT_H
