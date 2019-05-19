#include "CellClient.h"

CellClient::CellClient(SOCKET sockfd)
{
	_sockfd = sockfd;
	memset(_msgBuf, 0, RECV_BUF_SIZE);//��ʼ����Ϣ������
	_lastMsgPos = 0;

	memset(_sendBuf, 0, SEND_BUF_SIZE);//��ʼ�����ͻ�����
	_lastSendPos = 0;

	resetDTHeart();//����������ʱ��ʼ��
	resetDTSend();//�����ϴη�����Ϣ��ʱ��
}

CellClient::~CellClient()
{
}

/**
 * @brief	��������ÿͻ��˷�������
 * @param	header	����������
 * @return	�Ƿ�ɹ�
 */
int CellClient::sendData(std::shared_ptr<netmsg_Header> header)
{
	int ret = SOCKET_ERROR;
	int nSendLen = header->dataLength;//���������ݵĳ���
	const char* pSendData = (const char*)header.get();//����������

	while (true)
	{
		//�������ݵ����������ͻ�������
		//�жϻ��������������ݺʹ����������ܳ���
		if (_lastSendPos + nSendLen >= SEND_BUF_SIZE)
		{
			int nCopyLen = SEND_BUF_SIZE - _lastSendPos;//���ͻ�����ʣ���С
			memcpy(_sendBuf + _lastSendPos, pSendData, nCopyLen);//����������
			pSendData = pSendData + nCopyLen;//δ�ܷ��뷢�ͻ�����������
			nSendLen = nSendLen - nCopyLen;//���³���
			ret = send(_sockfd, _sendBuf, SEND_BUF_SIZE, 0);

			resetDTSend();//���ͳɹ�����Ҫ�����ϴη��ͳɹ���ʱ��
			_lastSendPos = 0;
			if (ret == SOCKET_ERROR)
			{
				return ret;
			}
		}
		else
		{
			memcpy(_sendBuf + _lastSendPos, pSendData, nSendLen);//�����ݷ��뷢�ͻ�����
			_lastSendPos = _lastSendPos + nSendLen;//���·��ͻ�����β��λ��
			break;
		}
	}

	return ret;
}

//�����������������ݷ��͸��ͻ���
int CellClient::sendDataDirect()
{
	int ret = SOCKET_ERROR;
	//������������
	if (_lastSendPos > 0&&_sockfd!=SOCKET_ERROR)
	{		
		ret = send(_sockfd, _sendBuf, _lastSendPos, 0);//�����ͻ����������ݷ��ͳ�ȥ
		_lastSendPos = 0;//���ͻ�����β������
		resetDTSend();//���÷���ʱ��
	}
	return ret;
}

int CellClient::sendDataDirect(std::shared_ptr<netmsg_Header> header)
{
	sendData(header);
	sendDataDirect();
	return 0;
}
