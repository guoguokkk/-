#include"Client.h"
#include<thread>
#include<iostream>
using std::cout;
using std::endl;
using std::cin;

bool g_bRun = true;
//绑定一个线程负责发送请求
void cmd_thread()
{
	while (true)
	{
		char cmd_buf[256];
		cin >> cmd_buf;
		if (strcmp(cmd_buf, "exit") == 0)
		{
			g_bRun = false;
			cout << "Client exit." << endl;
			break;
		}
		else
			cout << "Invalid input, please re-enter." << endl;
	}
}

int main()
{
	//const int client_count = FD_SETSIZE - 1;
	const int client_count = 1;
	Client* client[client_count];
	for (int i = 0; i < client_count; ++i)
		client[i] = new Client();

	for (int i = 0; i < client_count; ++i)
		client[i]->Connect(SERVER_IP, PORT);

	//启动线程
	std::thread t(cmd_thread);
	t.detach();

	Login login;
	strcpy(login.name, "ws");
	strcpy(login.password, "111");
	while (g_bRun)
	{
		for (int i = 0; i < client_count; ++i)
		{
			client[i]->SendData(&login);
			client[i]->OnRun();
		}
	}

	for (int i = 0; i < client_count; ++i)
		client[i]->CloseClient();

	cout << "EXIT...." << endl;
	return 0;
}