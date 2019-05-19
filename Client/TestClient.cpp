#include<iostream>
#include"Client.h"
#include<thread>
#include<chrono>
#include<stdio.h>
#include<atomic>
#include"TimeStamp.h"

#define CLIENT_COUNT 1000//�ͻ�������
#define THREAD_COUNT 8//�߳�����

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
const int thread_count = THREAD_COUNT;//�����̵߳�����
std::atomic_int sendCount(0);
std::atomic_int readyCount(0);

void recvThread(int begin, int end)//1-4���ĸ��߳�
{
	while (g_bRun)
	{
		for (int i = begin; i < end; ++i)
		{
			client[i]->onRun();
		}
	}
}

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

	//�ȴ������߳�׼���÷���
	++readyCount;
	while (readyCount < thread_count)
	{
		std::chrono::milliseconds t(100);
		std::this_thread::sleep_for(t);
	}

	std::thread t1(recvThread, begin, end);
	t1.detach();

	netmsg_Login login[10];//��߷���Ƶ�ʣ�ÿ�η���ʮ����Ϣ��
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
			if (client[i]->sendData(login, nLen) != SOCKET_ERROR)
			{
				++sendCount;//���͵�����
			}
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

	CellTimeStamp tTime;

	while (g_bRun)
	{
		auto t = tTime.getElapsedSecond();
		if (t >= 1.0)
		{
			//!"std::atomic<int>::atomic(const std::atomic<int>&)": ����������ɾ���ĺ���
			//!���� "std::atomic<int>" ��Ϊ�ɱ���������Ĳ����ķǱ�׼�÷�
			/*printf("thread<%d>,clients<%d>,time<%lf>,send<%d>\n",
				thread_count, client_count, t, sendCount);*/

			printf("thread<%d>,clients<%d>,time<%lf>,send<%d>\n",
				thread_count, client_count, t, (int)(sendCount.load() / t));

			sendCount = 0;
			tTime.update();
		}

#ifdef _WIN32
		Sleep(1);
#else
		sleep(1);
#endif // _WIN32
	}

	std::cout << "EXIT...." << std::endl;
	return 0;
}