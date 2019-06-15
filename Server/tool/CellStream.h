#ifndef CELL_STREAM_H_
#define CELL_STREAM_H_
#include<stdint.h>
#include<memory>
#include"../tool/CellLog.h"

//字节流数据
class CellStream
{
public:
	CellStream(char* pData, int nSize = 1024, bool bDelete = false) :
		_pBuf(pData), _nSize(nSize), _bDelete(bDelete)
	{
		_nWritePos = 0;
		_nReadPos = 0;
	}

	CellStream(int nSize = 1024) :
		_nSize(nSize)
	{
		_nWritePos = 0;
		_nReadPos = 0;
		_pBuf = new char[nSize];
		_bDelete = true;
	}

	virtual ~CellStream()
	{
		if (_bDelete && _pBuf)
		{
			delete _pBuf;
			_pBuf = nullptr;
		}
	}

	inline bool canRead(int n) { return _nSize - _nReadPos >= n; }//是否可读n个字节	
	inline bool canWrite(int n) { return _nSize - _nWritePos >= n; }//是否可写n个字节	
	inline void push(int n) { _nWritePos += n; }//写入n字节数据	
	inline void pop(int n) { _nReadPos += n; }//读出n字节数据

	//两端数据格式不一致时需要处理，C# char 2字节 : c++ char 1字节
	//读取，bOffset是否偏移
	template<typename T>
	bool read(T& n, bool bOffset = true)
	{
		int nLen = sizeof(T);
		if (canRead(nLen))
		{
			memcpy(&n, _pBuf + _nReadPos, nLen);
			if (bOffset)
			{
				pop(nLen);
			}
			return n;
		}

		//错误日志
		CELLLOG_ERROR(" CellStream: read failed\n");
		return 0;
	}

	template<typename T>
	bool onlyRead(T& n)
	{
		return read(n, false);
	}
	
	bool readInt8(int8_t& n) { return read<int8_t>(n); }//typedef signed char int8_t;	
	bool readInt16(int16_t& n) { return read<int16_t>(n); }//typedef short int16_t;	
	bool readInt32(int32_t& n) { return read<int32_t>(n); }//typedef int int32_t;
	bool readInt64(int64_t& n) { return read<int64_t>(n); }//typedef long long int64_t;

	bool readUInt8(uint8_t& n) { return read<uint8_t>(n); }//typedef unsigned char uint8_t;	
	bool readUInt16(uint16_t& n) { return read<uint16_t>(n); }//typedef unsigned short uint16_t;
	bool readUInt32(uint32_t& n) { return read<uint32_t>(n); }//typedef unsigned int uint32_t;	
	bool readUInt64(uint64_t& n) { return read<uint64_t>(n); }//typedef unsigned long long uint64_t;

	bool readFloat(float& n) { return read<float>(n); }
	bool readDouble(double& n) { return read<double>(n); }

	//读取数组，len是给定的数组长度，实际读取的长度应该不大于len
	template<typename T>
	uint32_t readArray(T* pData, uint32_t len)
	{
		uint32_t len1 = 0;
		read(len1, false);//先读取数组的长度，不偏移尾部位置
		if (len1 < len)
		{
			size_t nLen = sizeof(T) * len1;//要读取数据的大小
			//判断能否读取
			if (canRead(nLen + sizeof(uint32_t)))
			{
				pop(sizeof(uint32_t));
				memcpy(pData, _pBuf + _nReadPos, nLen);
				pop(nLen);
				return len1;
			}
		}

		//错误日志
		CELLLOG_ERROR(" CellStream: read failed\n");
		return 0;
	}

	//写入
	template<typename T>
	bool write(T n)
	{
		size_t nLen = sizeof(T);//要写入数据的大小

		//判断能否写入
		if (canWrite(nLen))
		{
			memcpy(_pBuf + _nWritePos, &n, nLen);
			push(nLen);
			return true;
		}

		//错误日志
		CELLLOG_ERROR(" CellStream: write failed\n");
		return false;
	}

	bool writeInt8(int8_t n) { return write<int8_t>(n); }
	bool writeInt16(int16_t n) { return write<int16_t>(n); }
	bool writeInt32(int32_t n) { return write<int32_t>(n); }
	bool writeInt64(int64_t n) { return write<int64_t>(n); }

	bool writeUInt8(uint8_t n) { return write<uint8_t>(n); }
	bool writeUInt16(uint16_t n) { return write<uint16_t>(n); }
	bool writeUInt32(uint32_t n) { return write<uint32_t>(n); }
	bool writeUInt64(uint64_t n) { return write<uint64_t>(n); }

	bool writeFloat(float n) { return write<float>(n); }
	bool writeDouble(double n) { return write<double>(n); }

	//写入数组
	template<typename T>
	bool writeArray(T* pData, uint32_t len)
	{
		size_t nLen = sizeof(T) * len;//要写入数据的大小

		//判断能否写入
		if (canWrite(sizeof(uint32_t) + nLen))
		{
			writeInt32(len);//先记录数组的长度
			memcpy(_pBuf + _nWritePos, pData, nLen);
			push(nLen);
			return true;
		}

		//错误日志
		CELLLOG_ERROR(" CellStream: write failed\n");
		return false;
	}

	char* getData() { return _pBuf; }
	int getWritePos() { return _nWritePos; }
	void setWritePos(int n) { _nWritePos = n; }

private:
	char* _pBuf = nullptr;//数据缓冲区
	int _nWritePos;//缓冲区已写入数据的尾部
	int _nReadPos;//缓冲区已读取数据的尾部
	int _nSize;//缓冲区总大小
	int _fullCount = 0;//缓冲区写满次数
	bool _bDelete = false;//_pBuf是外部数据块，是否应该释放
};

#endif // !CELL_STREAM_H_

