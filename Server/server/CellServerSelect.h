#ifndef CELL_SERVER_SELECT_H_
#define CELL_SERVER_SELECT_H_
#include"../server/CellServer.h"

//select-网络消息接收处理服务类
class CellServerSelect :public CellServer
{
public:
	//跨线程时，父类中有纯虚函数，子类中实现了，子类先释放，父类还没有释放，父类仍有可能执行纯虚函数，报错-需要虚析构函数
	~CellServerSelect()
	{
		CloseCellServer();
	}

	//select-计算可读、可写集合，并处理
	bool DoNetEvents()
	{
		///////////////////////////////////////////////////////////
		//可读集合
		if (_clientsChange)
		{
			//如果客户列表发生改变
			_clientsChange = false;
			_fdRead.Zero();//清理集合

			//将所有的描述符都添加到可读集合，并找到最大的描述符
			_maxSock = _clients.begin()->second->GetSockfd();
			for (auto client : _clients)
			{
				_fdRead.Add(client.second->GetSockfd());
				if (_maxSock < client.second->GetSockfd())
				{
					_maxSock = client.second->GetSockfd();
				}
			}

			//备份可读集合
			_fdReadBak.Copy(_fdRead);
		}
		else
		{
			//可读集合没有改变，直接拷贝内容
			_fdRead.Copy(_fdReadBak);
		}

		/////////////////////////////////////////////////////////
		//可写集合
		bool bNeedWrite = false;//是否需要写数据
		_fdWrite.Zero();
		for (auto client : _clients)
		{
			//需要向客户端发送数据,才加入fd_set检测是否可写
			if (client.second->NeedWrite()) {
				bNeedWrite = true;
				_fdWrite.Add(client.second->GetSockfd());
			}
		}

		/////////////////////////////////////////////////////////
		//计算可读、可写集合
		timeval time;
		time.tv_sec = 0;//秒
		time.tv_usec = 1;
		int ret = 0;
		if (bNeedWrite)
		{
			//返回发生可读事件的描述符和发生可写事件的描述符
			ret = select(_maxSock + 1, _fdRead.GetFdSet(), _fdWrite.GetFdSet(), nullptr, &time);
		}
		else
		{
			//返回发生可读事件的描述符
			ret = select(_maxSock + 1, _fdRead.GetFdSet(), nullptr, nullptr, &time);
		}

		//select返回值：超时返回0;失败返回-1；成功返回大于0的整数，这个整数表示就绪描述符的数目
		if (ret < 0)
		{
			CELLLOG_PERROR("CELLServer%d.OnRun.select Error exit:errno<%d>,errmsg<%s>",
				_id, errno, strerror(errno));
			return false;
		}
		else if (ret == 0)
		{
			return true;
		}

		//处理可读集合数据-接收消息
		ReadData();

		//处理可写集合数据-发送消息
		WriteData();
		return true;
	}


	//处理可读集合数据-接收消息
	void ReadData() {
#ifdef _WIN32
		auto pfdset = _fdRead.GetFdSet();

		//windows下记录了描述符数组的大小和描述符数组，遍历数组
		for (int i = 0; i < pfdset->fd_count; ++i)
		{
			auto iter = _clients.find(pfdset->fd_array[i]);
			if (iter != _clients.end())
			{
				int ret = RecvData(iter->second);//找到了，接收数据
				if (ret == SOCKET_ERROR)
				{
					//客户端离开
					OnClientLeave(iter->second);
					_clients.erase(iter);
				}
			}
		}

#else
		for (auto iter = _clients.begin(); iter != _clients.end();) {
			if (_fdRead.Has(iter->second->GetSockfd())) {
				int ret = RecvData(iter->second);//接收消息
				if (ret == SOCKET_ERROR) {
					//客户端离开
					OnClientLeave(iter->second);
					auto iterOld = iter;
					++iter;
					_clients.erase(iterOld);
					continue;
				}
			}
			++iter;
		}
#endif // _WIN32
	}

	//处理可写集合数据-发送消息
	void WriteData() {
#ifdef _WIN32
		auto pfdset = _fdWrite.GetFdSet();

		//windows下记录了描述符数组的大小和描述符数组，遍历数组
		for (int i = 0; i < pfdset->fd_count; ++i)
		{
			auto iter = _clients.find(pfdset->fd_array[i]);//在正式客户端队列中要到某个发生可写事件的客户端
			if (iter != _clients.end())
			{
				int ret = iter->second->SendDataReal();//找到了，就直接发送消息
				if (ret == SOCKET_ERROR)
				{
					OnClientLeave(iter->second);
					_clients.erase(iter);
				}
			}
		}

#else
		for (auto iter = _clients.begin(); iter != _clients.end();) {
			if (iter->second->NeedWrite() && _fdWrite.Has(iter->second->GetSockfd())) {
				int ret = iter->second->SendDataReal();//发送消息
				if (ret == -1) {
					OnClientLeave(iter->second);
					auto iterOld = iter;
					++iter;
					_clients.erase(iter);
					continue;
				}
			}
			++iter;
		}
#endif // _WIN32
	}

private:
	CellFDSet _fdRead;
	CellFDSet _fdWrite;
	CellFDSet _fdReadBak;//客户列表备份

	SOCKET _maxSock;
};

#endif // !CELL_SERVER_SELECT_H_
