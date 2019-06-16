#ifndef CELL_NETWORK_H_
#define CELL_NETWORK_H_
#include"Message.h"
#include<signal.h>

//��������������رգ����������������������ر�ʱ�����쳣
class CellNetwork
{
private:
	CellNetwork()
	{
#ifdef _WIN32
		//���� windows ����
		WORD version = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(version, &data);
#endif // _WIN32

#ifndef _WIN32
		//if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		//	return (1);
		//�����쳣�źţ�Ĭ������ᵼ�½�����ֹ
		signal(SIGPIPE, SIG_IGN);
#endif // !_WIN32
	}

	~CellNetwork()
	{
#ifdef _WIN32
		WSACleanup();//�ر� windows ����
#endif // _WIN32
	}

public:
	static void Init()
	{
		static CellNetwork obj;
	}
};

#endif // !CELL_NETWORK_H_

