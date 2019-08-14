#ifndef CELL_MSG_STREAM_H_
#define CELL_MSG_STREAM_H_
#include"../server/CellStream.h"
#include"../comm/Message.h"
#include<string>

//读取字节流(接收数据时读取)
class CellReadStream :public CellStream
{
public:
	//构造函数-读取字节流(接收数据时读取)，参数为要转换的数据 pData，数据长度 nSize，是否需要删除数据缓冲区 bDelete=false
	CellReadStream(char* pData, int nSize, bool bDelete = false) :
		CellStream(pData, nSize, false)
	{
		push(nSize);//写入消息长度个字节，为修改读取到数据修改流出接口
		uint16_t n;
		read<uint16_t>(n);//读取消息的长度
		getNetCmd();
	}

	CellReadStream(netmsg_DataHeader* header) :
		CellReadStream((char*)header, header->dataLength)
	{
	}

	uint16_t getNetCmd()
	{
		uint16_t cmd = CMD_ERROR;
		read<uint16_t>(cmd);
		return cmd;
	}
};

//写入字节流(发送数据时写入)
class CellWriteStream :public CellStream
{
public:
	//构造函数-写入字节流(发送数据时写入)，参数为要转换的数据 pData，数据长度 nSize，是否需要删除数据缓冲区 bDelete=false
	CellWriteStream(char* pData, int nSize, bool bDelete = false) :
		CellStream(pData, nSize, bDelete)
	{
		write<int16_t>(0);//预先占领消息长度所需空间，数据真正写入之后在再更新
	}

	CellWriteStream(int nSize = 1024) :
		CellStream(nSize)
	{
		write<uint16_t>(0);//预先占领消息长度所需空间，数据真正写入之后在再更新
	}

	//更新消息的长度
	void finish()
	{
		//因为之前把缓冲区的一段位置给提前占下了，所以写的时候需要先回到缓冲区头部，然后把消息长度写入，再回到之前的位置
		int pos = getWritePos();//获取当前缓冲区最后一次写的位置
		setWritePos(0);//将尾部位置置为0，相当于回到缓冲区头部
		write<uint16_t>(pos);//unsigned short 把消息的长度写入到缓冲区
		setWritePos(pos);
	}

	//设置命令类型，参数为命令类型 cmd
	void setNetCmd(uint16_t cmd) { write<uint16_t>(cmd); }

	//写入字符串，参数为数据 str，给定的字符串元素个数 num
	bool writeString(const char* str, int num) { return writeArray(str, num); }
	//写入字符串，参数为数据 str
	bool writeString(const char* str) { return writeArray(str, strlen(str)); }
	//写入字符串，参数为数据 str
	bool writeString(std::string& str) { return writeArray(str.c_str(), str.size()); }
};

#endif // !CELL_MSG_STREAM_H_
