#include<iostream>
#include"Client.h"
#include<thread>
#include<chrono>
#include<stdio.h>
#define CLIENT_COUNT 10000
bool g_bRun = true;
void cmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (strcmp(cmdBuf, "exit") == 0)
		{
			g_bRun = false;
			printf("Client exit.\n");
			break;
		}
		else
		{
			printf("Invalid input, please re-enter.\n");
		}
	}
}

const int client_count = CLIENT_COUNT;//�ͻ�������
Client* client[client_count];//�ͻ�������
const int thread_count = 4;//�����̵߳�����

void sendThread(int id)//1-4���ĸ��߳�
{
	printf("thread<%d>,start\n", id);
	int c = client_count / thread_count;
	int begin = (id - 1) * c;
	int end = id * c;
	for (int i = begin; i < end; ++i)
	{
		client[i] = new Client();
	}

	for (int i = begin; i < end; ++i)
	{
		client[i]->connectToServer(IP, PORT);
	}

	printf("thread<%d>,Connect<begin=%d, end=%d>\n", id, begin, end);

	std::chrono::milliseconds t(3000);//3000����
	std::this_thread::sleep_for(t);

	Login login[10];//��߷���Ƶ�ʣ�ÿ�η���ʮ����Ϣ��
	for (int i = 0; i < 10; ++i)
	{
		strcpy(login[i].userName, "kzj");
		strcpy(login[i].passWord, "12345");
	}

	const int nLen = sizeof(login);
	while (g_bRun)
	{
		for (int i = begin; i < end; ++i)
		{
			client[i]->sendData(login, nLen);
			client[i]->onRun();
		}
	}

	for (int i = begin; i < end; ++i)
	{
		client[i]->closeClient();
		delete client[i];
	}

	printf("thread<%d>,exit\n", id);
}

int main()
{
	//�����߳�
	std::thread cmd_t(cmdThread);
	cmd_t.detach();

	//���������߳�
	for (int i = 0; i < thread_count; ++i)
	{
		std::thread t(sendThread, i + 1);//���ݵ����̵߳ı��
		t.detach();
	}
	while (g_bRun)
	{

#ifdef _WIN32
		Sleep(100);
#else
		sleep(100);
#endif // _WIN32
	}
	std::cout << "EXIT...." << std::endl;
	return 0;
}