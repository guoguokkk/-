#ifndef CELL_MSG_STREAM_H_
#define CELL_MSG_STREAM_H_
#include"../tool/CellStream.h"
#include"../tool/Message.h"
#include<string>

//接收消息数据字节流
class CellReadStream :public CellStream
{
public:
	CellReadStream(char* pData, int nSize = 1024, bool bDelete = false) :
		CellStream(pData, nSize, false)
	{
		//push(nSize);//写入消息长度个字节，我感觉不需要写，而且也能正确运行
		uint16_t n;
		read<uint16_t>(n);//读取消息的长度
		getNetCmd();
	}

	CellReadStream(netmsg_Header* header) :
		CellReadStream((char*)header, header->dataLength, false)
	{
	}

	uint16_t getNetCmd()
	{
		uint16_t cmd = CMD_ERROR;
		read<uint16_t>(cmd);
		return cmd;
	}
};

//发送消息数据字节流
class CellWriteStream :public CellStream
{
public:
	CellWriteStream(char* pData, int nSize = 1024, bool bDelete = false) :
		CellStream(pData, nSize, bDelete)
	{
		write<int16_t>(0);//short
	}

	CellWriteStream(int nSize = 1024) :
		CellStream(nSize)
	{
		write<uint16_t>(0);//预先占有消息长度所需要的空间
	}

	void setNetCmd(uint16_t cmd) { write<uint16_t>(cmd); }

	bool writeString(const char* str, int len)
	{
		return writeArray(str, len);
	}

	bool writeString(const char* str)
	{
		return writeArray(str, strlen(str));
	}

	bool writeString(std::string& str)
	{
		return writeArray(str.c_str(), str.size());
	}

	//在首部位置写入消息的长度，写完以后再去调用判断到底消息有多长
	void finish()
	{
		//因为之前把缓冲区的一段位置给提前占下了，所以写的时候需要先回到缓冲区头部，然后把消息长度写入，再回到之前的位置
		int pos = getWritePos();//获取当前缓冲区最后一次写的位置
		setWritePos(0);//将尾部位置置为0，相当于回到缓冲区头部
		write<uint16_t>(pos);//unsigned short 把消息的长度写入到缓冲区
		setWritePos(pos);
	}
};

#endif // !CELL_MSG_STREAM_H_

