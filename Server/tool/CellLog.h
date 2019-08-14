#ifndef CELL_LOG_H_
#define CELL_LOG_H_
#include"../tool/CellTask.h"
#include"../tool/CellTimeStamp.h"
#include<ctime>
#include<stdio.h>

//日志记录
class CellLog
{
#ifdef _DEBUG
#ifndef CELLLOG_DEBUG
#define CELLLOG_DEBUG(...) CellLog::Debug(__VA_ARGS__)//Debug 调式信息，只在debug模式下起作用
#endif // !CELLLOG_DEBUG
#else
#define CELLLOG_DEBUG(...)
#endif // _DEBUG


#define CELLLOG_INFO(...) CellLog::Info(__VA_ARGS__)//Info 普通信息	
#define CELLLOG_ERROR(...) CellLog::Error(__VA_ARGS__)//Error 错误信息	
#define CELLLOG_WARRING(...) CellLog::Warring(__VA_ARGS__)//Warring 警告信息

private:
	CellLog()
	{
		_taskSever.startTask();//启动任务对应的工作线程
	}

	~CellLog()
	{
		_taskSever.closeTask();//关闭任务对应的工作线程
		if (_logFile)
		{
			Info("CellLog::fclose(_logFile)");
			fclose(_logFile);
			_logFile = nullptr;
		}
	}

public:
	//单例模式
	static CellLog& Instance()
	{
		static CellLog sLog;
		return sLog;
	}

	//设置日志文件的路径，参数为日志文件名称logName，操作方式(读、写...)mode，日志文件的名称是否添加当前日期时间hasDate
	void setLogPath(const char* logName, const char* mode, bool hasDate)
	{
		//文件已经存在，需要先关闭
		if (_logFile)
		{
			Info("CellLog::fclose(_logFile)");
			fclose(_logFile);
			_logFile = nullptr;
		}

		static char logPath[256] = {};//文件路径名

		//如果需要在文件名称中添加当前日期时间
		if (hasDate)
		{
			auto t = system_clock::now();
			auto tNow = system_clock::to_time_t(t);
			std::tm* now = std::localtime(&tNow);

			//名字+年月日时分秒
			sprintf(logPath, "%s[%d-%d-%d_%d-%d-%d].txt",
				logName, now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
		}
		else
		{
			//名字
			sprintf(logPath, "%s.txt", logName);
		}

		_logFile = fopen(logPath, mode);
		if (_logFile)
		{
			CELLLOG_INFO("CellLog::setLogPath success, <%s, %s>", logPath, mode);//成功
		}
		else
		{
			CELLLOG_INFO("CellLog::setLogPath failed, <%s, %s>", logPath, mode);//失败
		}
	}

	//Error
	static void Error(const char* pStr)
	{
		Error("%s", pStr);
	}

	template<typename ...Args>
	static void Error(const char* pformat, Args... args)
	{
		Echo("<Error>", pformat, args...);
	}
	   
	//Warring
	static void Warring(const char* pStr)
	{
		Warring("%s", pStr);
	}

	template<typename ...Args>
	static void Warring(const char* pformat, Args... args)
	{
		Echo("<Warring>", pformat, args...);
	}

	//Debug
	static void Debug(const char* pStr)
	{
		Debug("%s", pStr);
	}

	template<typename ...Args>
	static void Debug(const char* pformat, Args... args)
	{
		Echo("<Debug>", pformat, args...);
	}

	//Info
	static void Info(const char* pStr)
	{
		Info("%s", pStr);
	}

	template<typename ...Args>
	static void Info(const char* pformat, Args... args)
	{
		Echo("<Info>", pformat, args...);
	}

	//输出函数
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
	FILE* _logFile = nullptr;//保存日志的文件
	CellTaskServer _taskSever;//执行任务类
};

#endif // !CELL_LOG_H_
