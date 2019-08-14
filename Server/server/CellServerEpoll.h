#ifndef CELL_SERVEREPOLL_H_
#define CELL_SERVEREPOLL_H_
#include"../server/CellServer.h"

class CellServerEpoll :public CellServer {
	//计算可读、可写集合，并处理
	bool DoNetEvents()
	{

	}
};

#endif // !CELL_SERVEREPOLL_H_
