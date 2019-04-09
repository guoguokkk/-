#ifdef _WIN32
#include"../Test/Common.h"
#else
#include"Common.h"
#endif // _WIN32

#include"Client.h"
int main()
{
#ifdef _WIN32
	WORD _version = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_version, &_data);
#endif

	Client client;
	client.InitClient();
	client.SendRequenst();
	client.CloseClient();

#ifdef _WIN32
	getchar();
	WSACleanup();
#endif
	return 0;
}