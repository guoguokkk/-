#include"Client.h"
#include<iostream>
using std::cout;
using std::endl;
using std::cin;
Client::Client()
{
	_client_sock = -1;
}

Client::~Client()
{
	CloseClient();
}

//��ʼ���ͻ��ˣ���ʼ�������������׽���
void Client::InitClient()
{
#ifdef _WIN32
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);//LPWSADATA��һ��ָ��WSADATA�ṹ��ָ��
#endif // _WIN32

	//��ֹ�ظ���ʼ��
	if (_client_sock < 0)
	{
		_client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_client_sock < 0)
			cout << "Client socket error." << endl;
		else
			cout << "Client socket success." << endl;
	}
	else
	{
		CloseClient();//������Ҫ�ȹر�
		cout << "Client " << _client_sock << " close old connections." << endl;
	}
}

//���ӵ�������
int Client::Connect(const char* ip, const unsigned short port)
{
	//û�г�ʼ����Ҫ�ȳ�ʼ��
	if (_client_sock <= 0)
	{
		InitClient();
	}

	sockaddr_in server_addr;
#ifdef _WIN32
	server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_serv_addr.sin_addr.s_addr = inet_addr(ip);
#endif // _WIN32
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	int ret = connect(_client_sock, (sockaddr*)& server_addr, sizeof(sockaddr_in));
	if (ret < 0)
		cout << "Client " << _client_sock << " connect error." << endl;
	else
		cout << "Client " << _client_sock << " connect success." << endl;
	return ret;
}

//�رտͻ���
void Client::CloseClient()
{
	//��ֹ�ظ��ر�
	if (_client_sock < 0)
	{
		return;
	}
#ifdef _WIN32
	closesocket(_client_sock);
	WSACleanup();
#else
	close(_client_sock);
#endif // _WIN32
	_client_sock = -1;
}

//��ѯ
bool Client::OnRun()
{
	//�ж��Ƿ���������
	if (!IsRun())
		return false;

	fd_set fds_read;//���������ϣ��Ƿ���Դ���Щ�ļ��ж�ȡ����
	FD_ZERO(&fds_read);//��������������
	FD_SET(_client_sock, &fds_read);//�ͻ��˼���ɶ�����

	//����ʱ��
	timeval time_val;
	time_val.tv_sec = 1;//��
	time_val.tv_usec = 0;//����
	int ret = select(_client_sock + 1, &fds_read, nullptr, nullptr, &time_val);
	if (ret < 0)
	{
		cout << "Client " << _client_sock << "select task end 1." << endl;
		return false;
	}

	if (FD_ISSET(_client_sock, &fds_read))
	{
		FD_CLR(_client_sock, &fds_read);//����ÿͻ���

		//�ͻ����˳�
		int ret = RecvData();//��������
		if (ret == -1)
		{
			cout << "Client " << _client_sock << " select task end 2." << endl;
			CloseClient();
			return false;
		}
	}
	cout << "do other things" << endl;
	return true;
}

//�ж��Ƿ������������
bool Client::IsRun()
{
	return _client_sock >= 0;
}

//��������
int Client::RecvData()
{
	char recv_buf[4096];//���ջ�����
	int len = (int)recv(_client_sock, recv_buf, sizeof(Header), 0);//�������ݵ�ͷ��	
	if (len <= 0)
	{
		cout << "Disconnect from server." << endl;
		return -1;
	}
	Header* header = (Header*)recv_buf;//ͷ��
	recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);//���ܳ�ͷ�������������
	OnNetMsg(header, recv_buf);
	return 0;
}

int Client::SendData(Header* header)
{
	//�ж���Ϣ�Ƿ�Ϊ��
	if (IsRun() && header)
	{
		send(_client_sock, (const char*)header, header->data_length, 0);
		return 0;
	}
	return -1;
}

//����������Ϣ
void Client::OnNetMsg(Header* header, char* recv_buf)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* login_result = (LoginResult*)recv_buf;
		cout << "Login result is " << login_result->result << " ,data length is " << login_result->data_length << endl;
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* logout_result = (LogoutResult*)recv_buf;
		cout << "Logout result is " << logout_result->result << " ,data length is " << logout_result->data_length << endl;
	}
	break;
	case CMD_NEW_UER_JOIN:
	{
		NewUserJoin* new_user_join = (NewUserJoin*)recv_buf;
		cout << "New User is " << new_user_join->sock << endl;
	}
	default:
		break;
	}
}
