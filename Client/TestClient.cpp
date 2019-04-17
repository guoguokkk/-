#include<iostream>
#include"Client.h"
#include<thread>
#include<chrono>
#include<stdio.h>
#define CLIENT_COUNT 10000
bool run = true;
void cmd_thread()
{
	while (true)
	{
		char cmd_buf[256];
		scanf("%s", cmd_buf);
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

const int client_count = CLIENT_COUNT;//客户端数量
Client* client[client_count];////客户端数组
const int thread_count = 4;//发送线程的数量

void send_thread(int id)//0-3，四个线程
{
	printf("thread<%d>,start\n", id);
	int c = client_count / thread_count;
	int begin = id * c;
	int end = (id + 1) * c;
	for (int i = begin; i < end; ++i)
	{
		client[i] = new Client();
	}

	for (int i = begin; i < end; ++i)
	{
		client[i]->Connect(IP, PORT);
	}

	printf("thread<%d>,Connect<begin=%d, end=%d>\n", id, begin, end);

	std::chrono::milliseconds t(300);//3000毫秒
	std::this_thread::sleep_for(t);

	Login login[10];//提高发送频率，每次发送十个消息包
	for (int i = 0; i < 10; ++i)
	{
		strcpy(login[i].userName, "kzj");
		strcpy(login[i].passWord, "12345");
	}

	const int len = sizeof(login);
	while (run)
	{
		for (int i = begin; i < end; ++i)
		{
			client[i]->SendData(login, len);
			client[i]->OnRun();
		}
	}

	for (int i = begin; i < end; ++i)
	{
		client[i]->CloseClient();
		delete client[i];
	}

	printf("thread<%d>,exit\n", id);
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