#ifndef CELL_STREAM_H_
#define CELL_STREAM_H_
#include<stdint.h>
#include<memory>
#include"../tool/CellLog.h"

//字节流数据
class CellStream
{
public:
	//构造函数-字节流数据，参数为要转换的数据 pData，数据长度 nSize，是否需要删除数据缓冲区 bDelete=false
	CellStream(char* pData, int nSize, bool bDelete = false) :
		_pBuf(pData), _nSize(nSize), _bDelete(bDelete)
	{
		_nWritePos = 0;//最后写入的位置
		_nReadPos = 0;//最后读取的位置
	}

	//构造函数，参数为长度 nSize
	CellStream(int nSize = 1024) :
		_nSize(nSize)
	{
		_nWritePos = 0;//最后写入的位置
		_nReadPos = 0;//最后读取的位置
		_pBuf = new char[nSize];//新建数据缓冲区
		_bDelete = true;//数据缓冲区需要删除
	}

	virtual ~CellStream()
	{
		if (_bDelete && _pBuf)
		{
			delete _pBuf;
			_pBuf = nullptr;
		}
	}

	//数据缓冲区是否还可以读取一段数据，参数为要读取的字节数 n	
	inline bool canRead(int n) { return _nSize - _nReadPos >= n; }

	//数据缓冲区是否还可以写入一段数据，参数为要写入的字节数 n	
	inline bool canWrite(int n) { return _nSize - _nWritePos >= n; }

	//更新写入一段数据之后的写位置，参数为写入的字节数 n
	inline void push(int n) { _nWritePos += n; }

	//更新读取一段数据之后的读位置，参数为读取的字节数 n
	inline void pop(int n) { _nReadPos += n; }

	//获取数据缓冲区
	char* getData() { return _pBuf; }

	//获取数据缓冲区长度
	int getLength() { return _nWritePos; }

	//获取最后一次写入的位置
	int getWritePos() { return _nWritePos; }

	//设置最后一次写入的位置，参数为新的最后一次写入的位置 newPos
	void setWritePos(int newPos) { _nWritePos = newPos; }

	////////////////////////////////////////////////////////////////////////////////////////
	//读取数据，参数为要读取数据 data，是否要更新最后一次读取的位置 bOffset=true
	template<typename T>
	bool read(T& getData, bool bOffset = true)
	{
		int nLen = sizeof(T);//计算要读取数据的字节长度

		//判断是否还可以读取nLen个字节
		if (canRead(nLen))
		{
			memcpy(&getData, _pBuf + _nReadPos, nLen);//将数据加入数据缓冲区
			if (bOffset)
				pop(nLen);//更新最后一次读取的位置
			return true;
		}
		CELLLOG_ERROR("CellStream: read failed");
		return false;
	}

	//只读数据-不更新最后读取的位置，参数为要读取数据 data
	template<typename T>
	bool onlyRead(T& getData)
	{
		return read(getData, false);
	}

	//读取数组-返回值为数组的字节长度或者0，参数为数组数据 pArr，给定的数组长度(实际读取的长度应该不大于len) len
	template<typename T>
	uint32_t readArray(T* pArr, uint32_t len)
	{
		//数组先用uint32_t记录了数组长度，然后存储数据
		uint32_t len1 = 0;
		read(len1, false);//先读取数组的长度，不偏移尾部位置
		if (len1 <= len)
		{
			size_t nLen = sizeof(T) * len1;//计算数组的字节长度

			//判断能否读取
			if (canRead(nLen + sizeof(uint32_t)))
			{
				pop(sizeof(uint32_t));
				memcpy(pArr, _pBuf + _nReadPos, nLen);
				pop(nLen);
				return len1;
			}
		}

		CELLLOG_ERROR("CellStream: read failed");
		return 0;
	}

	//读取字符串，参数为要返回的字符串
	bool readString(std::string& str)
	{
		uint32_t nLen = 0;
		read(nLen, false);//先读取数组的长度，不偏移尾部位置
		if (nLen > 0)
		{
			//判断能否读取
			if (canRead(nLen + sizeof(uint32_t)))
			{
				pop(sizeof(uint32_t));
				str.insert(0, _pBuf + _nReadPos, nLen);//在0位置插入字符串_pBuf + _nReadPos的前nLen个字符
				pop(nLen);
				return true;
			}
		}
		return false;
	}

	bool readInt8(int8_t& getData) { return read<int8_t>(getData); }//typedef signed char int8_t;	
	bool readInt16(int16_t& getData) { return read<int16_t>(getData); }//typedef short int16_t;	
	bool readInt32(int32_t& getData) { return read<int32_t>(getData); }//typedef int int32_t;
	bool readInt64(int64_t& getData) { return read<int64_t>(getData); }//typedef long long int64_t;

	bool readUInt8(uint8_t& getData) { return read<uint8_t>(getData); }//typedef unsigned char uint8_t;	
	bool readUInt16(uint16_t& getData) { return read<uint16_t>(getData); }//typedef unsigned short uint16_t;
	bool readUInt32(uint32_t& getData) { return read<uint32_t>(getData); }//typedef unsigned int uint32_t;	
	bool readUInt64(uint64_t& getData) { return read<uint64_t>(getData); }//typedef unsigned long long uint64_t;

	bool readFloat(float& getData) { return read<float>(getData); }
	bool readDouble(double& getData) { return read<double>(getData); }

	////////////////////////////////////////////////////////////////////////////////////////
	//写入数据，参数为要写入数据 data
	template<typename T>
	bool write(T getData)
	{
		size_t nLen = sizeof(T);//计算要写入数据的字节长度

		//判断能否写入
		if (canWrite(nLen))
		{
			memcpy(_pBuf + _nWritePos, &getData, nLen);
			push(nLen);
			return true;
		}

		CELLLOG_ERROR("CellStream: write failed");
		return false;
	}

	//写入数组，参数为数组数据 pArr，给定的数组元素个数 num
	template<typename T>
	bool writeArray(T* pArr, uint32_t num)
	{
		size_t nLen = sizeof(T) * num;//要写入数据的大小

		//判断能否写入
		if (canWrite(sizeof(uint32_t) + nLen))
		{
			writeInt32(num);//先写入数组的元素数量
			memcpy(_pBuf + _nWritePos, pArr, nLen);
			push(nLen);
			return true;
		}

		CELLLOG_ERROR("CellStream: write failed");
		return false;
	}

	bool writeInt8(int8_t getData) { return write<int8_t>(getData); }
	bool writeInt16(int16_t getData) { return write<int16_t>(getData); }
	bool writeInt32(int32_t getData) { return write<int32_t>(getData); }
	bool writeInt64(int64_t getData) { return write<int64_t>(getData); }

	bool writeUInt8(uint8_t getData) { return write<uint8_t>(getData); }
	bool writeUInt16(uint16_t getData) { return write<uint16_t>(getData); }
	bool writeUInt32(uint32_t getData) { return write<uint32_t>(getData); }
	bool writeUInt64(uint64_t getData) { return write<uint64_t>(getData); }

	bool writeFloat(float getData) { return write<float>(getData); }
	bool writeDouble(double getData) { return write<double>(getData); }
	
private:
	char* _pBuf = nullptr;//数据缓冲区
	int _nWritePos;//缓冲区已写入数据的尾部
	int _nReadPos;//缓冲区已读取数据的尾部
	int _nSize;//缓冲区总大小
	int _fullCount = 0;//缓冲区写满次数
	bool _bDelete = false;//_pBuf是外部数据块，是否应该释放
};

#endif // !CELL_STREAM_H_
