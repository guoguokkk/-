#include<iostream>
#include"../client/Client.h"
#include<thread>
#include<chrono>
#include<stdio.h>
#include<atomic>
#include"../tool/CellTimeStamp.h"
#include"MyClient.h"
#include"../tool/CellLog.h"
#include"../tool/CellThread.h"
#include"../tool/CellStream.h"
#include"../tool/CellMsgStream.h"

#define CLIENT_COUNT 8//客户端数量
#define THREAD_COUNT 4//线程数量

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
			CellLog::Info("Client exit.\n");
			break;
		}
		else
		{
			CellLog::Info("Invalid input, please re-enter.\n");
		}
	}
}

const int client_count = CLIENT_COUNT;//客户端数量
Client* client[client_count];//客户端数组
const int thread_count = THREAD_COUNT;//发送线程的数量
std::atomic_int sendCount(0);
std::atomic_int readyCount(0);

void recvThread(int begin, int end)//1-4，四个线程
{
	//CellTimeStamp t;
	while (g_bRun)
	{
		for (int i = begin; i < end; ++i)
		{
			/*if (t.getElapsedSecond() > 3.0 && i == begin)
				continue;*/
			client[i]->onRun();
		}
	}
}

void sendThread(int id)//1-4，四个线程
{
	CellLog::Info("thread<%d>,start\n", id);
	int c = client_count / thread_count;
	int begin = (id - 1) * c;
	int end = id * c;
	for (int i = begin; i < end; ++i)
	{
		client[i] = new MyClient();
	}

	for (int i = begin; i < end; ++i)
	{
		client[i]->connectToServer(IP, PORT);
	}

	CellLog::Info("thread<%d>,Connect<begin=%d, end=%d>\n", id, begin, end);

	//等待其他线程准备好发送
	++readyCount;
	while (readyCount < thread_count)
	{
		CellThread::sleepInThread(10);
	}

	std::thread t1(recvThread, begin, end);
	t1.detach();

	netmsg_Login login[10];//提高发送频率，每次发送十个消息包
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
			if (client[i]->sendData(login) != SOCKET_ERROR)
			{
				++sendCount;//发送的数量
			}
		}

		std::chrono::milliseconds t(100);
		std::this_thread::sleep_for(t);
	}

	for (int i = begin; i < end; ++i)
	{
		client[i]->closeClient();
		delete client[i];
	}

	CellLog::Info("thread<%d>,exit\n", id);
}

int main()
{
	CellLog::Instance().setLogPath("../../clientLog.txt", "w");

	CellSendStream s;
	s.setNetCmd(CMD_LOGOUT);
	s.writeInt8(1);
	s.writeInt16(2);
	s.writeInt32(3);
	s.writeFloat(4.5f);
	s.writeDouble(6.7);
	s.writeString("client");
	s.writeString("kzj");
	int arr[] = { 1,2,3,4,5,6, };
	s.writeArray(arr, 6);
	s.finish();

	MyClient client;
	client.connectToServer(IP, PORT);

	while (client.isRun())
	{
		client.onRun();
		client.sendData(s.getData(), s.getWritePos());
		CellThread::sleepInThread(1000);
	}

	//输入线程
	std::thread cmd_t(cmdThread);
	cmd_t.detach();

	//启动发送线程
	for (int i = 0; i < thread_count; ++i)
	{
		std::thread t(sendThread, i + 1);//传递的是线程的编号
		t.detach();
	}

	CellTimeStamp tTime;

	while (g_bRun)
	{
		auto t = tTime.getElapsedSecond();
		if (t >= 1.0)
		{
			//!"std::atomic<int>::atomic(const std::atomic<int>&)": 尝试引用已删除的函数
			//!将类 "std::atomic<int>" 作为可变参数函数的参数的非标准用法
			/*CellLog::Info("thread<%d>,clients<%d>,time<%lf>,send<%d>\n",
				thread_count, client_count, t, sendCount);*/

			CellLog::Info("thread<%d>,clients<%d>,time<%lf>,send<%d>\n",
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