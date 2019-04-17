#include"Server.h"
#include"MyServer.h"
#include<iostream>
#include<thread>

bool g_bRun = true;
void cmdThread()
{
	while (true)
	{
		char cmd_buf[256];
		std::cin >> cmd_buf;
		if (strcmp(cmd_buf, "exit") == 0)
		{
			g_bRun = false;
			std::cout << "Server exit" << std::endl;
			break;
		}
		else
		{
			std::cout << "Invalid input, please re-enter." << std::endl;
		}
	}
}
int main()
{
	MyServer server;
	server.initServer();
	server.Bind(IP, PORT);
	server.Listen(5);
	server.startServer(4);

	//Æô¶¯UIÏß³Ì
	std::thread cmd_t(cmdThread);
	cmd_t.detach();
		
	while (g_bRun)
	{
		server.onRun();
	}
	server.closeServer();
	std::cout << "EXIT...." << std::endl;
	return 0;
}