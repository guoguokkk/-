#include"../client/Client.h"
#include"../tool/Common.h"
#include"../tool/CellThread.h"
#include"../tool/CellConfig.h"
#include<thread>
#include<chrono>
#include<stdio.h>
#include<atomic>
#include<iostream>
#include<vector>

const char* strIP = "127.0.0.1";//服务器IP地址
uint16_t nPort = 8099;//服务器端口
int nThread = 1;//发送线程的数量
int nClient = 1;//客户端数量

int nMsg = 1;//客户端每次发几条消息
int nSendSleep = 1;//写入消息到缓冲区的间隔时间
int nWorkSleep = 1;//工作休眠时间
int nSendBufSize = SEND_BUF_SIZE;//客户端发送缓冲区大小
int nRecvBufSize = RECV_BUF_SIZE;//客户端接收缓冲区大小

std::atomic_int sendCount(0);
std::atomic_int readyCount(0);
std::atomic_int connectCount(0);

class MyClient :public Client
{
public:
	MyClient()
	{
		_bCheckMsgID = CellConfig::Instance().hasKey("-checkMsgID");
	}

	//处理消息
	void onNetMsg(netmsg_DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			netmsg_LoginResult* login_result = (netmsg_LoginResult*)header;

			if (_bCheckMsgID)
			{
				//检测id是否连续
				if (login_result->msgID != _nRecvMsgID)
				{
					//当前消息ID和本地收消息次数不匹配
					CELLLOG_ERROR("OnNetMsg socket<%d> msgID<%d> _nRecvMsgID<%d> %d",
						_pClient->getSockfd(), login_result->msgID, _nRecvMsgID, login_result->msgID - _nRecvMsgID);
				}
				++_nRecvMsgID;
			}

			/*CELLLOG_INFO("netmsg_Login result : socket = %d , data length= %d , result= %d",
				(int)_clientSock, login_result->dataLength, login_result->result);*/
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			CellReadStream r(header);
			int8_t n1;
			r.readInt8(n1);
			int16_t n2;
			r.readInt16(n2);
			int32_t n3;
			r.readInt32(n3);
			float n4;
			r.readFloat(n4);
			double n5;
			r.readDouble(n5);
			char s[20] = {};
			int a = r.readArray(s, 20);//返回的是数组元素个数
			char name[20] = {};
			int b = r.readArray(name, 20);//返回的是数组元素个数
			int password[20] = {};
			int c = r.readArray(password, 20);//返回的是数组元素个数

			netmsg_LogoutResult* logout_result = (netmsg_LogoutResult*)header;
			/*CELLLOG_INFO("netmsg_Logout result : socket = %d , data length= %d , result= %d",
				(int)_client_sock, logout_result->dataLength, logout_result->result);*/
		}
		break;
		case CMD_ERROR:
		{
			CELLLOG_INFO("error : socket = %d , data length= %d",
				(int)_pClient->getSockfd(), header->dataLength);
		}
		break;
		default:
		{
			CELLLOG_INFO("Undefined data : socket = %d , data length=  %d",
				(int)_pClient->getSockfd(), header->dataLength);
		}
		break;
		}
	}

	int sendTest(netmsg_Login* login)
	{
		int ret = 0;

		//如果剩余发送次数大于0
		if (_nSendCount > 0)
		{
			login->msgID = _nSendMsgID;
			ret = sendData(login);
			if (ret != SOCKET_ERROR)
			{
				++_nSendMsgID;
				--_nSendCount;
			}
		}
		return ret;
	}

	bool checkSend(time_t dt)
	{
		_tRestTime += dt;
		//每经过nSendSleep毫秒
		if (_tRestTime >= nSendSleep)
		{
			_tRestTime -= nSendSleep;//重置计时			
			_nSendCount = nMsg;//重置发送计数
		}
		return _nSendCount > 0;
	}
private:
	int _nRecvMsgID = 1;//接收消息id计数	
	int _nSendMsgID = 1;//发送消息id计数	
	time_t _tRestTime = 0;//发送时间计数	
	int _nSendCount = 0;//发送条数计数	
	bool _bCheckMsgID = false;//检查接收到的服务端消息ID是否连续
};

//工作线程
void workThread(CellThread* pThread, int id)
{
	CELLLOG_INFO("thread<%d>,start", id);//nThread个线程 id值为 1-nThread
	std::vector<MyClient*> clients(nClient);//客户端数组	

	int beginClients = 0;
	int endClients = nClient;

	/////////////////////////////////////////////////////////
	//新建客户端，nClient个
	for (int i = beginClients; i < endClients; ++i)
	{
		if (!pThread->isRun())
			break;//输入了exit
		clients[i] = new MyClient();
		CellThread::sleepInThread(0);//多线程时让下CPU，线程休眠会让出CPU
	}

	/////////////////////////////////////////////////////////
	//初始化客户端，连接服务器
	for (int i = beginClients; i < endClients; ++i)
	{
		if (!pThread->isRun())
			break;//输入了exit
		if (clients[i]->initClient(nSendBufSize, nRecvBufSize) == INVALID_SOCKET)
			break;//创建失败，端口用完
		if (clients[i]->connectToServer(strIP, nPort) == SOCKET_ERROR)
			break;
		++connectCount;
		CellThread::sleepInThread(0);
	}
	CELLLOG_INFO("thread<%d>, Connect<begin=%d, end=%d, connectCount=%d>", id, 0, nClient, (int)connectCount);

	++readyCount;
	while (readyCount < nThread && pThread->isRun())
	{
		//等待其他线程准备好发送
		CellThread::sleepInThread(10);
	}

	/////////////////////////////////////////////////////////
	//需要发送的数据
	netmsg_Login login;
	strcpy(login.userName, "kzj");
	strcpy(login.passWord, "12345");

	//收发数据都是通过onRun线程
	//sendData只是将数据写入发送缓冲区
	//等待select检测可写时才会发送数据	
	auto tOld = CellTime::getNowInMillSec();//旧的时间点
	auto tNew = tOld;//新的时间点
	auto dt = tNew;//经过的时间
	CellTimeStamp tTime;
	while (pThread->isRun())
	{
		tNew = CellTime::getNowInMillSec();
		dt = tNew - tOld;
		tOld = tNew;

		int count = 0;
		//每轮每个客户端发送nMsg条数据
		for (int m = 0; m < nMsg; ++m)
		{
			for (int j = beginClients; j < endClients; ++j)
			{
				if (clients[j]->isRun())
				{
					if (clients[j]->sendTest(&login) > 0)
						++sendCount;
				}
			}
		}

		for (int j = beginClients; j < endClients; ++j)
		{
			if (clients[j]->isRun())
			{
				//超时设置为0表示select检测状态后立即返回
				if (!clients[j]->onRun(0))
				{
					--connectCount;
					continue;
				}
				//检测发送计数 每1000毫秒只允许发送100次 可以少发，但不能多发
				clients[j]->checkSend(dt);
			}
		}

		CellThread::sleepInThread(nWorkSleep);
	}

	/////////////////////////////////////////////////////////
	//关闭客户端
	for (int i = beginClients; i < endClients; ++i)
	{
		clients[i]->closeClient();
		delete clients[i];
	}
	CELLLOG_INFO("thread<%d>,exit", id);
	--readyCount;
}

//字节流发送的代码
void testStreamClient()
{
	//字节流发送
	CellWriteStream s;
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
	client.connectToServer(strIP, nPort);

	while (client.isRun())
	{
		client.onRun();
		client.sendData(s.getData(), s.getWritePos());
		CellThread::sleepInThread(1000);
	}
}

void testClient(int argc, char* args[])
{
	/////////////////////////////////////////////////////////
	//处理配置信息
	CellConfig::Instance().Init(argc, args);
	strIP = CellConfig::Instance().getStr("strIP", "127.0.0.1");
	nPort = CellConfig::Instance().getInt("nPort", 8099);
	nThread = CellConfig::Instance().getInt("nThread", 1);
	nClient = CellConfig::Instance().getInt("nClient", 10000);
	nMsg = CellConfig::Instance().getInt("nMsg", 10);
	nSendSleep = CellConfig::Instance().getInt("nSendSleep", 100);
	nSendBufSize = CellConfig::Instance().getInt("nSendBufSize", SEND_BUF_SIZE);
	nRecvBufSize = CellConfig::Instance().getInt("nRecvBufSize", RECV_BUF_SIZE);

	/////////////////////////////////////////////////////////
	//启动终端命令线程，用于接收运行时用户输入的指令
	CellThread tCmd;
	tCmd.startThread(nullptr, [](CellThread* pThread) {
		while (true)
		{
			char cmdBuf[256] = {};
			scanf("%s", cmdBuf);
			if (strcmp(cmdBuf, "exit") == 0)
			{
				CELLLOG_INFO("Client exit.");
				break;
			}
			else
			{
				CELLLOG_ERROR("Invalid input, please re-enter.");
			}
		}
		},
		nullptr);

	/////////////////////////////////////////////////////////
	//启动模拟客户端线程
	std::vector<CellThread*> threads;
	for (int i = 0; i < nThread; ++i)
	{
		CellThread* t = new CellThread();
		t->startThread(nullptr, [i](CellThread* pThread) {
			workThread(pThread, i + 1);
			}, nullptr);
		threads.push_back(t);
	}

	/////////////////////////////////////////////////////////
	//每秒数据统计
	CellTimeStamp tTime;
	while (tCmd.isRun())
	{
		auto t = tTime.getElapsedSecond();
		if (t >= 1.0)
		{
			CELLLOG_INFO("thread<%d>,clients<%d>,connect<%d>,time<%lf>,send<%d>",
				nThread, nClient, (int)connectCount, t, (int)sendCount);
			sendCount = 0;
			tTime.update();
		}
		CellThread::sleepInThread(1);
	}

	/////////////////////////////////////////////////////////
	//关闭所有的线程
	for (auto t : threads)
	{
		t->closeThread();
		delete t;
	}
}

int main(int argc, char* args[])
{
	//文件名不加日期
	//CellLog::Instance().setLogPath("F:/AA/guoguokkk/log/clientLog", "w", false);
	CellLog::Instance().setLogPath("./clientLog", "w", false);

	////字节流发送测试
	//testStreamClient();

	//非字节流发送测试
	testClient(argc, args);

	std::cout << "EXIT...." << std::endl;
	return 0;
}