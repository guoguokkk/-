#include"Server.h"
int main()
{
	Server server;
	server.Bind(SERVER_IP, PORT);
	server.Listen(5);
	
	while (true)
	{
		server.OnRun();
	}
	
	return 0;
}