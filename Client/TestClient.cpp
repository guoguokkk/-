#include<iostream>
#include"Client.h"
#include<thread>
bool run = true;

void cmd_thread()
{
	while (true)
	{
		char cmd_buf[128];
		std::cin >> cmd_buf;
		if (strcmp(cmd_buf, "exit") == 0)
		{
			run = false;
			std::cout << "Client exit" << std::endl;
			break;
		}
		else
		{
			std::cout << "Invalid input, please re-enter." << std::endl;
		}
	}
}

const int client_count = 100;//客户端数量
Client* client[client_count];//所有的客户端
const int thread_count = 4;//发送线程的数量

void send_thread(int id)//0-3，四个线程
{
	int start = id * client_count / thread_count;
	int end = (id + 1) * client_count / thread_count;
	for (int i = start; i < end; ++i)
	{
		client[i] = new Client();
	}

	for (int i = start; i < end; ++i)
	{
		client[i]->Connect(IP,PORT);
		std::cout << "Client " << i << std::endl;
	}

	Login login;
	strcpy(login.name, "kzj");
	strcpy(login.password, "12345");

	while (run)
	{
		for (int i = start; i < end; ++i)
		{
			client[i]->Send(&login);
			//client[i]->OnRun();;
		}
	}
	for (int i = start; i < end; ++i)
	{
		client[i]->CloseClient();
	}
}

int main()
{
	//输入线程
	std::thread cmd_t(cmd_thread);
	cmd_t.detach();

	//启动发送线程
	for (int i = 0; i < thread_count; ++i)
	{
		std::thread t(send_thread, i);//传递的是线程的编号
		t.detach();
	}
	while (run)
	{
		Sleep(100);
	}
	std::cout << "EXIT...." << std::endl;
	return 0;
}