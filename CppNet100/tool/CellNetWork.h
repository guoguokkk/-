#ifndef CELL_NETWORK_H_
#define CELL_NETWORK_H_
#include"../tool/Common.h"
#ifndef _WIN32
#include<fcntl.h>
#include<stdlib.h>
#endif // !_WIN32

//��������������رգ����������������������ر�ʱ�����쳣
class CellNetwork
{
private:
	CellNetwork()
	{
#ifdef _WIN32
		//����Windows socket 2.x����
		WORD version = MAKEWORD(2, 2);
		WSADATA getData;
		WSAStartup(version, &getData);
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
		WSACleanup();//���Windows socket����
#endif // _WIN32
	}

public:
	static void Init()
	{
		static CellNetwork obj;
	}

	static int make_nonblocking(SOCKET fd)
	{
#ifdef _WIN32
		{
			unsigned long nonblocking = 1;

			//һ����ʶ�׽ӿڵ������֡����׽ӿ�s�Ĳ������ָ��cmd��������������ָ��
			//FIONBIO��������ֹ�׽ӿ�s�ķ�����ģʽ
			if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR)
			{
				CELLLOG_WARRING("fcntl(%d, F_GETFL)", (int)fd);
				return -1;
			}
		}
#else
		{
			int flags;
			if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) 
			{
				CELLLOG_WARRING("fcntl(%d, F_GETFL)", fd);
				return -1;
			}
			if (!(flags & O_NONBLOCK))
			{
				if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
				{
					CELLLOG_WARRING("fcntl(%d, F_SETFL)", fd);
					return -1;
				}
			}
		}
#endif
		return 0;
	}

	static int make_reuseaddr(SOCKET fd)
	{
		int flag = 1;

		//��ʶһ���׽ӿڵ������֡�ѡ���Ĳ�Ρ������õ�ѡ�ָ�룬ָ����ѡ������õ���ֵ�Ļ�������optval���������ȡ�
		//SO_REUSEADDR���򿪻�رյ�ַ���ù��ܣ��˴�Ϊ��
		//�����Ҫ���׽��ּ���������ѡ��ͱ����level����Ϊ SOL_SOCKET
		if (SOCKET_ERROR == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)& flag, sizeof(flag)))
		{
			CELLLOG_WARRING("setsockopt socket<%d> SO_REUSEADDR fail", (int)fd);
			return SOCKET_ERROR;
		}
		return 0;
	}
};

#endif // !CELL_NETWORK_H_
