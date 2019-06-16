#ifndef CELL_CONFIG_H_
#define CELL_CONFIG_H_
#include<string>
#include"../tool/CellLog.h"
#include<unordered_map>
#include<cstring>

//读取配置数据，目前配置数据都来源于main函数的args传入
class CellConfig
{
public:
	static CellConfig& Instance()
	{
		static CellConfig obj;
		return obj;
	}

	void Init(int argc, char* args[])
	{
		_exePath = args[0];//当前程序路径
		for (int i = 0; i < argc; ++i)
		{
			madeCmd(args[i]);
		}
	}

	void madeCmd(char* cmd)
	{
		//char *strchr(const char *str, int c) 在参数 str 所指向的字符串中搜索第一次出现字符 c（一个无符号字符）的位置
		//也就是包含 =和=之后的所有字符的字符串
		char* val = strchr(cmd, '=');
		if (val)
		{
			*val = '\0';//等号换为结束符，变成两个字符串
			val++;
			_kv[cmd] = val;
			CELLLOG_DEBUG("madeCmd %s|%s", cmd, val);
		}
		else
		{//没有"="
			_kv[cmd] = "";
			CELLLOG_DEBUG("madeCmd %s", cmd);
		}
	}

	//默认参数，要获取的参数名字
	const char* getStr(const char* def, const char* argName)
	{
		auto iter = _kv.find(argName);
		if (iter == _kv.end())
		{//没找到
			CELLLOG_ERROR("CellConfig::getStr, not found <%s>", argName);
		}
		else
		{
			def = iter->second.c_str();
		}
		CELLLOG_INFO("%s=%s", argName, def);
		return def;
	}

	//默认参数，要获取的参数名字
	int getInt(int def, const char* argName)
	{
		auto iter = _kv.find(argName);
		if (iter == _kv.end())
		{//没找到
			CELLLOG_ERROR("CellConfig::getStr, not found <%s>", argName);
		}
		else
		{
			def = stoi(iter->second);
		}
		CELLLOG_INFO("%s=%d", argName, def);
		return def;
	}

	//有没有某个关键字
	bool hasKey(const char* key)
	{
		auto iter = _kv.find(key);
		return iter != _kv.end();
	}

private:
	CellConfig() {}
	~CellConfig() {}

private:
	std::string _exePath;//当前程序路径
	std::unordered_map<std::string, std::string> _kv;
};

#endif // !CELL_CONFIG_H_
