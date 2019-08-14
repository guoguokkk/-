#ifndef CELL_FDSET_H_
#define CELL_FDSET_H_
#include"../comm/Common.h"

#define CELL_MAX_FD 10240

//描述符集合类
class CellFDSet
{
public:
	CellFDSet()
	{
		int nSockNum = CELL_MAX_FD;

#ifdef _WIN32
		_nfdSize = sizeof(u_int) + sizeof(SOCKET) * nSockNum;
#else
		_nfdSize = nSockNum / (8 * sizeof(char));
#endif // _WIN32

		_pfdset = (fd_set*)new char[_nfdSize];
		memset(_pfdset, 0, _nfdSize);
	}

	~CellFDSet()
	{
		if (_pfdset)
		{
			delete[] _pfdset;
			_pfdset = nullptr;
		}
	}

	//在文件描述符集合中增加一个新的文件描述符
	inline void Add(SOCKET s)
	{
#ifdef _WIN32
		FD_SET(s, _pfdset);
#else
		if (s < CELL_MAX_FD)
		{
			FD_SET(s, _pfdset);
		}
		else
		{
			CELLLOG_ERROR("CellFDSet::add sock<%d>, CELL_MAX_FD<%d>", (int)s, CELL_MAX_FD);
		}
#endif // _WIN32
	}

	//在文件描述符集合中删除一个文件描述符
	inline void del(SOCKET s)
	{
		FD_CLR(s, _pfdset);
	}

	//清零
	inline void Zero()
	{
#ifdef _WIN32
		FD_ZERO(_pfdset);
#else
		memset(_pfdset, 0, _nfdSize);
#endif // _WIN32
	}

	//判断描述符是否在集合中
	inline bool Has(SOCKET s)
	{
		return FD_ISSET(s, _pfdset);
	}

	//获取描述符集合
	inline fd_set* GetFdSet()
	{
		return _pfdset;
	}

	//拷贝描述符集合
	void Copy(CellFDSet& set)
	{
		memcpy(_pfdset, set.GetFdSet(), set._nfdSize);
	}
private:
	fd_set* _pfdset = nullptr;//描述符集合
	size_t _nfdSize = 0;//描述符集合的大小
};

#endif // !CELL_FDSET_H_

