#ifndef CLIENT_H_
#define CLIENT_H_
#include"../tool/Common.h"
#include"../client/CellClient.h"
#include"../tool/CellFDSet.h"

class Client {
public:
	Client();
	virtual ~Client();

	SOCKET initClient(int af, int sendSize = SEND_BUF_SIZE, int recvSize = RECV_BUF_SIZE);//初始化客户端,af表示ipv4或者ipv6
	int connectToServer(const char* ip, unsigned short port);//连接服务器
	void closeClient();//关闭客户端	
	bool onRun(int microseconds = 1);//处理网络消息	

	int recvData(SOCKET clientSock);//接收消息
	virtual void onNetMsg(netmsg_DataHeader* header) = 0;//处理消息
	int sendData(netmsg_DataHeader* header);//发送消息
	int sendData(const char* pData, int len);//发送消息
	bool isRun();

protected:
	CellFDSet _fdRead;
	CellFDSet _fdWrite;
	CellClient* _pClient = nullptr;
	bool _isConnect = false;
	int _addressFamily;//ipv4或者ipv6
};

#endif // !CLIENT_H_
