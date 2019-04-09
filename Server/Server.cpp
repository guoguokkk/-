#include"Server.h"
Server::Server()
{

}

Server::~Server()
{

}

void Server::CloseServer()
{
	closesocket(_server_sock);
}

void Server::InitServer()
{
	//建立一个socket,ipv4，面向连接的，tcp协议
	_server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_sock < 0)
		HandleError("Server socket error.");
	else
		HandleSuccess("Server socket success.");

	//绑定接受客户端连接的端口
	sockaddr_in _server_addr;
	_server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	_server_addr.sin_port = htons(PORT);
	_server_addr.sin_family = AF_INET;
	if (-1 == bind(_server_sock, (sockaddr*)& _server_addr, sizeof(_server_addr)))
		HandleError("Server bind error.");
	else
		HandleSuccess("Server bind success.");

	//监听网络端口
	if (-1 == listen(_server_sock, 5))
		HandleError("Server listen error.");
	else
		HandleSuccess("Server listen success.");
}

//接收请求，返回请求结果
int Server::processor(SOCKET _client_sock)
{
	char recv_buf[4096];
	int len = recv(_client_sock, recv_buf, sizeof(Header), 0);

	//强制类型转换
	Header* header = (Header*)recv_buf;
	if (len < 0)
	{
		std::cout << "client is exit." << std::endl;
		return -1;
	}
	else
	{
		if (header->cmd == CMD_LOGIN)
		{
			//接收客户端发送的数据
			recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);
			LOGIN* login = (LOGIN*)recv_buf;
			std::cout << login->_user_name << " : " << login->_user_password << std::endl;

			//返回数据
			LOGIN_RESULT login_result;
			send(_client_sock, (char*)& login_result, sizeof(login_result), 0);
		}
		else if (header->cmd == CMD_LOGOUT)
		{
			//接收客户端发送的数据
			recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);
			LOGOUT* logout = (LOGOUT*)recv_buf;
			std::cout << logout->_user_name << " : " << logout->_user_password << std::endl;

			//返回数据
			LOGOUT_RESULT logout_result;
			send(_client_sock, (char*)& logout_result, sizeof(logout_result), 0);
		}
		else
		{
			std::cout << "client is exit." << std::endl;
			return -1;
		}
	}
}

void Server::HandleClientRequest()
{

	//处理多个客户端的请求
	while (true)
	{
		FD_SET _fd_read;
		FD_ZERO(&_fd_read);//select()会修改字符集，如果在一个循环中，则描述符集必须被重新赋值
		FD_SET(_server_sock, &_fd_read);//将服务器fd加入set集合

		//将客户端fd加入set集合
		for (int i = 0; i < _group_clients.size(); ++i)
		{
			FD_SET(_group_clients[i], &_fd_read);
		}
		timeval _time_val;
		_time_val.tv_sec = 0;
		_time_val.tv_usec = 0;

		//监视老socketfd的状态是否改变，一旦改变说明有新的服务器连接进来
		int ret = select(_server_sock, &_fd_read, NULL, NULL, NULL);
		if (ret < 0)
		{
			HandleError("Server select end.");
			break;
		}
		else
			HandleSuccess("Server select success.");

		//检查在select函数返回后，某个描述符是否准备好
		if (FD_ISSET(_server_sock, &_fd_read))
		{
			FD_CLR(_server_sock, &_fd_read);

			//等待接受客户端连接
			SOCKET _client_sock;
			sockaddr_in _client_addr;
			int _client_addr_size = sizeof(_client_addr);
			_client_sock = accept(_server_sock, (sockaddr*)& _client_addr, &_client_addr_size);
			if (-1 == _client_sock)
				HandleError("Server accept error.");//无效客户端
			else
			{//连接客户端成功
				std::cout << "New Client " << _client_sock << ", ip: "
					<< inet_ntoa(_client_addr.sin_addr) << std::endl;//inet_ntoa
				_group_clients.push_back(_client_sock);//保存该客户端
			}
		}

		//依次处理所有的fd
		for (int i = 0; i < _fd_read.fd_count; ++i)
		{
			int ret = processor(_fd_read.fd_array[i]);
			if (ret == -1)
			{//如果某个客户端结束，则将它从数组删除
				auto iter = find(_group_clients.begin(), _group_clients.end(), _group_clients[i]);
				if (iter != _group_clients.end())
				{
					_group_clients.erase(iter);
				}
			}
		}

		std::cout << "do other things." << std::endl;
	}

	for (int i = 0; i < _group_clients.size(); ++i)
	{
		closesocket(_group_clients[i]);
	}
}
