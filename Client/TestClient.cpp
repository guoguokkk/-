#include"Client.h"
#include<thread>
#include<iostream>
using std::cout;
using std::endl;
using std::cin;

bool g_bRun = true;

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

const int client_count = 4000;
Client* client[client_count];
const int t_count = 4;//发送线程数量

void send_thread(int id)//0-3，四个线程
{
	int c = client_count / t_count;
	int begin = id * c;
	int end = (id + 1) * c;
	for (int i = begin; i < end; ++i)
	{		
		client[i] = new Client();
	}

	for (int i = begin; i < end; ++i)
	{
		client[i]->Connect(SERVER_IP, PORT);
		cout << "Client " << i << endl;
	}

	Login login;
	strcpy(login.name, "ws");
	strcpy(login.password, "111");
	while (g_bRun)
	{
		for (int i = begin; i < end; ++i)
		{
			client[i]->SendData(&login);
			//client[i]->OnRun();
		}
	}

	for (int i = begin; i < end; ++i)
		client[i]->CloseClient();
}

int main()
{
	//启动ui线程 
	std::thread t(cmd_thread);
	t.detach();
	
	//启动发送线程
	for (int i = 0; i < t_count; ++i)
	{
		std::thread t(send_thread,i);//传递的是线程的编号
		t.detach();
	}		
	while (g_bRun)
	{
		Sleep(100);
	}
	cout << "EXIT...." << endl;
	return 0;
}