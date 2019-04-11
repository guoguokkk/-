#include"Server.h"
#include<thread>
#include<iostream>
using std::cout;
using std::endl;
using std::cin;

bool g_bRun = true;
//��һ���̸߳���������
void cmd_thread(Server* server)
{
	while (true)
	{
		char send_buf[128];
		cin >> send_buf;
		if (strcmp(send_buf, "exit") == 0)
		{
			cout << "Server exit." << endl;
			g_bRun = false;
			break;
		}
		else
		{
			cout << "Invalid input, please re-enter." << endl;
		}
	}
}

int main()
{
	Server server;
	server.Bind(SERVER_IP, PORT);
	server.Listen(5);
	//�����߳�
	std::thread t(cmd_thread, &server);
	t.detach();
	while (g_bRun)
	{
		server.OnRun();
	}
	server.CloseServer();
	return 0;
}