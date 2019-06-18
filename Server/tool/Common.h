#ifndef COMMON_H_
#define COMMON_H_

#ifdef _WIN32
#define FD_SETSIZE 65535//FD_SETSIZE，一个进程中select所能操作的文件描述符的最大数目
#define WIN32_LEAN_AND_MEAN//解决 Windows.h 和 WinSock2.h 冲突
#include<Windows.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/socket.h>
#include<signal.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR -1
#endif // _WIN32

#include<stdio.h>

#include"../tool/CellConfig.h" //读取配置数据
#include"../tool/Message.h" //结构化消息定义
#include"../tool/CellTimeStamp.h" //获取时间、高精度计时器
#include"../tool/CellSemaphore.h" //信号量
#include"../tool/CellThread.h" //线程控制类
#include"../tool/CellLog.h" //日志记录
#include"../tool/CellMsgStream.h" //收发字节流

//缓冲区最小单元大小
#ifndef RECV_BUF_SIZE
#define RECV_BUF_SIZE 8192
#define SEND_BUF_SIZE 10240
//#define RECV_BUF_SIZE 10240
//#define SEND_BUF_SIZE 1024000
#endif // !RECV_BUF_SIZE

#define CLIENT_HEART_DEAD_TIME 60000//客户端心跳检测死亡计时，60秒
#define CLIENT_SEND_BUF_TIME 200//定时发送数据的最大时间间隔，0.2秒

#endif // !COMMON_H_
