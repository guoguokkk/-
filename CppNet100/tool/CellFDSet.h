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
		_pFdset = (fd_set*)new char[_nfdSize];
		memset(_pFdset, 0, _nfdSize);
	}

	~CellFDSet()
	{
		if (_pFdset)
		{
			delete[] _pFdset;
			_pFdset = nullptr;
		}
	}

	inline void add(SOCKET s)
	{
#ifdef _WIN32
		FD_SET(s, _pFdset);
#else
		if (s < CELL_MAX_FD)
		{
			FD_SET(s, _pFdset);
		}
		else {
			CELLLOG_ERROR("CellFDSet::add sock<%d>, CELL_MAX_FD<%d>", (int)s, CELL_MAX_FD);
		}
#endif // _WIN32
	}

	inline void del(SOCKET s)
	{
		FD_CLR(s, _pFdset);
	}

	inline void zero()
	{
#ifdef _WIN32
		FD_ZERO(_pFdset);
#else
		memset(_pFdset, 0, _nfdSize);
#endif // _WIN32
	}

	inline bool has(SOCKET s)
	{
		return FD_ISSET(s, _pFdset);
	}

	inline fd_set* fdset()
	{
		return _pFdset;
	}

	void copy(CellFDSet& set)
	{
		memcpy(_pFdset, set.fdset(), set._nfdSize);
	}
private:
	fd_set* _pFdset = nullptr;
	size_t _nfdSize = 0;
};

#endif // !CELL_FDSET_H_

