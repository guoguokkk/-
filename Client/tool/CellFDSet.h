#ifndef CELL_FDSET_H_
#define CELL_FDSET_H_
#include"../tool/Common.h"

#define CELL_MAX_FD 10240

class CellFDSet
{
public:
	CellFDSet()
	{
		int nSockNum = CELL_MAX_FD;
#ifdef _WIN32
		_nfdSize = sizeof(u_int) + (sizeof(SOCKET) * nSockNum);
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

	inline void del(SOCKET s)
	{
		FD_CLR(s, _pfdset);
	}

	inline void Zero()
	{
#ifdef _WIN32
		FD_ZERO(_pfdset);
#else
		memset(_pfdset, 0, _nfdSize);
#endif // _WIN32
	}

	inline bool Has(SOCKET s)
	{
		return FD_ISSET(s, _pfdset);
	}

	inline fd_set* GetFdSet()
	{
		return _pfdset;
	}

	void Copy(CellFDSet& set)
	{
		memcpy(_pfdset, set.GetFdSet(), set._nfdSize);
	}
private:
	fd_set* _pfdset = nullptr;
	size_t _nfdSize = 0;
};

#endif // !CELL_FDSET_H_

