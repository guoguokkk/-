#ifndef CELL_LOG_H_
#define CELL_LOG_H_
#include<stdio.h>
#include"CellTask.h"
#include"CellTimeStamp.h"
#include<ctime>

//日志：单例模式
class CellLog
{
	//Debug 调式信息，只在debug模式下起作用
#ifdef _DEBUG
#ifndef CELLLOG_DEBUG
#define CELLLOG_DEBUG(...) CellLog::Debug(__VA_ARGS__)
#endif // !CELLLOG_DEBUG
#else
#define CELLLOG_DEBUG(...)
#endif // _DEBUG

	//Info 普通信息
#define CELLLOG_INFO(...) CellLog::Info(__VA_ARGS__)

	//Error 错误信息
#define CELLLOG_ERROR(...) CellLog::Error(__VA_ARGS__)

	//Warring 警告信息
#define CELLLOG_WARRING(...) CellLog::Warring(__VA_ARGS__)

private:
	CellLog()
	{
		_taskSever.startTask();
	}

	~CellLog()
	{
		_taskSever.closeTask();
		if (_logFile)
		{
			Info("CellLog::fclose");
			fclose(_logFile);
			_logFile = nullptr;
		}
	}

public:
	static CellLog& Instance()
	{
		static CellLog sLog;
		return sLog;
	}

	void setLogPath(const char* logName, const char* mode)
	{
		if (_logFile)
		{
			Info("CellLog::fclose");
			fclose(_logFile);
			_logFile = nullptr;
		}

		static char logPath[256] = {};

		//时间
		auto t = system_clock::now();
		auto tNow = system_clock::to_time_t(t);
		std::tm* now = std::localtime(&tNow);

		//名字+年月日时分秒
		sprintf(logPath, "%s[%d-%d-%d_%d-%d-%d].txt",
			logName, now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

		_logFile = fopen(logPath, mode);
		if (_logFile)
		{
			CELLLOG_INFO("CellLog::setLogPath success, <%s, %s>", logPath, mode);
		}
		else
		{
			CELLLOG_INFO("CellLog::setLogPath failed, <%s, %s>", logPath, mode);
		}
	}

	//Info
	static void Info(const char* pStr)
	{
		Info("%s", pStr);
	}

	template<typename ...Args>
	static void Info(const char* pformat, Args... args)
	{
		Echo("Info, ", pformat, args...);
	}

	//Error
	static void Error(const char* pStr)
	{
		Error("%s", pStr);
	}

	template<typename ...Args>
	static void Error(const char* pformat, Args... args)
	{
		Echo("Error, ", pformat, args...);
	}

	//Debug
	static void Debug(const char* pStr)
	{
		Debug("%s", pStr);
	}

	template<typename ...Args>
	static void Debug(const char* pformat, Args... args)
	{
		Echo("Debug, ", pformat, args...);
	}

	//Warring
	static void Warring(const char* pStr)
	{
		Warring("%s", pStr);
	}

	template<typename ...Args>
	static void Warring(const char* pformat, Args... args)
	{
		Echo("Warring, ", pformat, args...);
	}

	template<typename ...Args>
	static void Echo(const char* type, const char* pformat, Args... args)
	{
		CellLog* pLog = &Instance();
		pLog->_taskSever.addTask([=]() {
			if (pLog->_logFile)
			{
				//写文件
				auto t = system_clock::now();
				auto tNow = system_clock::to_time_t(t);
				//fprintf(pLog->_logFile, "%r", ctime(&tNow));
				std::tm* now = std::localtime(&tNow);
				fprintf(pLog->_logFile, "%s", type);
				fprintf(pLog->_logFile, "[%d-%d-%d %d:%d:%d] ",
					now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
				fprintf(pLog->_logFile, pformat, args...);
				fprintf(pLog->_logFile, "%s", "\n");
				fflush(pLog->_logFile);
			}
			printf("%s", type);
			printf(pformat, args...);
			printf("%s", "\n");
			});
	}

private:
	FILE* _logFile = nullptr;
	CellTaskServer _taskSever;
};

#endif // !CELL_LOG_H_
