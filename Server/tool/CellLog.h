#ifndef CELL_LOG_H_
#define CELL_LOG_H_
#include<stdio.h>
#include"CellTask.h"
#include"CellTimeStamp.h"
#include<ctime>

//日志：单例模式
class CellLog
{
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
			Info("CellLog::fclose()\n");
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

	void setLogPath(const char* logPath, const char* mode)
	{
		if (_logFile)
		{
			Info("CellLog::fclose()\n");
			fclose(_logFile);
			_logFile = nullptr;
		}

		_logFile = fopen(logPath, mode);
		if (_logFile)
		{
			Info("CellLog::setLogPath() success, <%s, %s>\n", logPath, mode);
		}
		else
		{
			Info("CellLog::setLogPath() failed, <%s, %s>\n", logPath, mode);
		}
	}

	static void Info(const char* pStr)
	{
		CellLog* pLog = &Instance();
		pLog->_taskSever.addTask([=]() {
			if (pLog->_logFile)
			{
				//写文件
				auto t = system_clock::now();
				auto tNow = system_clock::to_time_t(t);
				//fprintf(pLog->_logFile, "%s", ctime(&tNow));
				std::tm* now = std::gmtime(&tNow);
				fprintf(pLog->_logFile, "%s", "Info ");
				fprintf(pLog->_logFile, "[%d-%d-%d %d:%d:%d]",
					now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
				fprintf(pLog->_logFile, "%s", pStr);
				fflush(pLog->_logFile);
			}
			printf(pStr);
			});
	}

	template<typename ...Args>
	static void Info(const char* pformat, Args... args)
	{
		CellLog* pLog = &Instance();
		pLog->_taskSever.addTask([=]() {
			if (pLog->_logFile)
			{
				//写文件
				auto t = system_clock::now();
				auto tNow = system_clock::to_time_t(t);
				//fprintf(pLog->_logFile, "%s", ctime(&tNow));
				std::tm* now = std::gmtime(&tNow);
				fprintf(pLog->_logFile, "%s", "Info ");
				fprintf(pLog->_logFile, "[%d-%d-%d %d:%d:%d] ",
					now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
				fprintf(pLog->_logFile, pformat, args...);
				fflush(pLog->_logFile);
			}
			printf(pformat, args...);
			});


	}

private:
	FILE* _logFile = nullptr;
	CellTaskServer _taskSever;
};


#endif // !CELL_LOG_H_
