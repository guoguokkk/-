#ifndef CLIENT_SOCK_H_
#define CLIENT_SOCK_H_

#ifdef _WIN32
#include"../Test/Message.h"
#else
#include"Message.h"
#endif // _WIN32

//�ͻ�����������
class ClientSock {
public:
	ClientSock(SOCKET clientSock = INVALID_SOCKET)
	{
		_sockfd = clientSock;
		memset(_msgBuf, 0, RECV_BUF_SIZE);//��ʼ����Ϣ������
		_lastMsgPos = 0;

		memset(_sendBuf, 0, SEND_BUF_SIZE);//��ʼ����Ϣ������
		_lastSendPos = 0;
	}
	SOCKET getSock() { return _sockfd; }
	char* getMsgBuf() { return _msgBuf; }
	int getLastPos() { return _lastMsgPos; }
	void setLastPos(int pos) { _lastMsgPos = pos; }

	//�������ݣ���ʱ��������
	int sendData(Header* header)
	{
		int ret = SOCKET_ERROR;
		int nSendLen = header->data_length;//��Ҫ�������ݵĳ���
		const char* pSendData = (const char*)header;//��Ҫ���͵����ݣ��������޸�

		//������С
		while (true)
		{
			if (_lastSendPos + nSendLen >= SEND_BUF_SIZE)
			{
				int nCopyLen = SEND_BUF_SIZE - _lastSendPos;//���Կ��������ݳ���
				memcpy(_sendBuf + _lastSendPos, pSendData, nCopyLen);//��������
				pSendData = pSendData + nCopyLen;
				nSendLen = nSendLen - nCopyLen;
				//return send(_client_sock, (char*) &header, header->data_length, 0);//!!!�������㣬һֱ����ʶ������
				//ret = send(_sockfd, (const char*)header, header->data_length, 0);
				ret = send(_sockfd, _sendBuf, SEND_BUF_SIZE, 0);
				_lastSendPos = 0;
				if (ret == SOCKET_ERROR)
				{
					return ret;
				}
			}
			else
			{
				memcpy(_sendBuf + _lastSendPos, pSendData, nSendLen);//��������
				_lastSendPos = _lastSendPos + nSendLen;
				break;//������㹻��һ�ε�ʱ�򣬾�����ѭ��
			}
		}

		return ret;
	}

private:
	char _msgBuf[RECV_BUF_SIZE];//��Ϣ��������!!!һ��Ҫ���ڽ��ջ�����
	int _lastMsgPos;//��Ϣ���������һ��λ��
	SOCKET _sockfd;//�ͻ���socket
	char _sendBuf[SEND_BUF_SIZE];//���ͻ�����
	int _lastSendPos;//���ͻ��������һ��λ��
};
#endif // !CLIENT_SOCK_H_
