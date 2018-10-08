
#include <string.h>
#ifdef WIN32
	#include <windows.h>
	#ifdef M_DEBUG
		#include <m_debug.h>
	#endif
#elif defined(__APPLE__)
extern void output_debug_string(const char *str);
#elif defined(ANDROID)
#include <android/log.h>
#include <typeinfo>
#endif

#include "../c2Debugger.h"

////////////////////////////////////////////////////////////////////////////////
extern unsigned gettick();
AppenderBase*	LOGGER::APPENDER_FILE = NULL;
AppenderBase*	LOGGER::APPENDER_CONSOLE = Appender_Console::getInstance();
AppenderBase*   LOGGER::APPENDER_MDEBUG = Appender_MDebug::getInstance();

LOGGER::LOGGER()
	: _pMsgBuffer(NULL), _nMsgBufferLen(0)
{
	APPENDER_OUTPUTDEBUG = Appender_OutputDebug::getInstance();
	APPENDER_CONSOLE = Appender_Console::getInstance();
	APPENDER_MDEBUG = Appender_MDebug::getInstance();

	addAppender(APPENDER_OUTPUTDEBUG, LOGGERMODE_ALL);
#ifndef __IOS__
	addAppender(APPENDER_CONSOLE, LOGGERMODE_ALL);
#endif
	addAppender(APPENDER_MDEBUG, LOGGERMODE_ALL);

	//time(&_tmStart);
	_nTickStart = _nTickPrev = gettick();
}

LOGGER::~LOGGER()
{
	for (AppenderList::iterator i = _AppenderList.begin(); i != _AppenderList.end(); ++i)
	{
		i->first->uninit();
	}

	if (_pMsgBuffer)
	{
		delete[] _pMsgBuffer;
		_pMsgBuffer = NULL;
	}
}

LOGGER& LOGGER::getInstance()
{
	static LOGGER p;// p= new LOGGER;
	return p;
}

//============================================================================================
bool LOGGER::addAppender(AppenderBase* appender, unsigned mode, const char* pFileName)
{
	if (appender == 0)
		return false;

	AUTOLOCK(this);
	AppenderList::iterator i = _AppenderList.find(appender);
	if (i != _AppenderList.end())
		return false;	//already added to appender list.

	if (!appender->init(pFileName))
		return false;	//init appender failed.

	_AppenderList[appender] = mode;
	return true;
}
bool LOGGER::removeAppender(AppenderBase* appender)
{
	AUTOLOCK(this);
	AppenderList::iterator i = _AppenderList.find(appender);
	if (i == _AppenderList.end())
		return false;

	appender->uninit();

	_AppenderList.erase(i);
	return true;
}
bool LOGGER::setAppenderMode(AppenderBase* appender, unsigned mode)
{
	AUTOLOCK(this);
	AppenderList::iterator i = _AppenderList.find(appender);
	if (i == _AppenderList.end())
		return false;

	i->second = mode;
	return true;
}
bool LOGGER::getAppenderMode(AppenderBase* appender, unsigned& mode)
{
	AUTOLOCK(this);
	AppenderList::iterator i = _AppenderList.find(appender);
	if (i == _AppenderList.end())
		return false;

	mode = i->second;
	return true;
}

//============================================================================================
void LOGGER::Info(const char *sFormat, ...)
{
	va_list arg;
	va_start(arg, sFormat);
	getInstance().line("<Info>", LOGGERMODE_INFO, sFormat, arg);
	va_end(arg);
}

void LOGGER::Log(const char *sFormat, ...)	//same as LOGGER::Info
{
	va_list arg;
	va_start(arg, sFormat);
	getInstance().line("<Info>", LOGGERMODE_INFO, sFormat, arg);
	va_end(arg);
};

void LOGGER::Debug(const char *sFormat, ...)
{
#ifndef _OPTIMIZE
	va_list arg;
	va_start(arg, sFormat);
	getInstance().line("<Debug>", LOGGERMODE_DEBUG, sFormat, arg);
	va_end(arg);
#endif
}
void LOGGER::Warn(const char *sFormat, ...)
{
	va_list arg;
	va_start(arg, sFormat);
	getInstance().line("<Warn>", LOGGERMODE_WARN, sFormat, arg);
	va_end(arg);
}
void LOGGER::Error(const char *sFormat, ...)
{
	va_list arg;
	va_start(arg, sFormat);
	getInstance().line("<Error>", LOGGERMODE_ERROR, sFormat, arg);
	va_end(arg);
}
void LOGGER::Fatal(const char *sFormat, ...)
{
	va_list arg;
	va_start(arg, sFormat);
	getInstance().line("<Fatal>", LOGGERMODE_FATAL, sFormat, arg);
	va_end(arg);
	throw 0;
}

void LOGGER::Print(const char *sText)
{
	getInstance().line_(sText);
}

void LOGGER::line(const char* prefix, unsigned appendermode, const char* format, va_list args)
{
	if (appendermode == 0 || _AppenderList.empty())
		return;

	AUTOLOCK(this);
	//prefix
	int nDestLen = strlen(format) + 0x400;				//reserver 1k space additional
	nDestLen = nDestLen - (nDestLen & 0x3FF) + 0x400;	//1k align
	if (_nMsgBufferLen == 0)
		nDestLen = MAX(0x4000, nDestLen);

	if (_pMsgBuffer == NULL || nDestLen > _nMsgBufferLen)
	{
		if (_pMsgBuffer)
			delete[] _pMsgBuffer;
		_pMsgBuffer = new char[nDestLen];
		_nMsgBufferLen = nDestLen;
	}
	strcpy(_pMsgBuffer, prefix);

	//date&time
	unsigned tick_now = gettick();
	if (tick_now < _nTickPrev)	//如果tick_now比tick_prev小，说明tick溢出了，重置time/tick
	{
		_nTickStart = tick_now;
		//time(&_tmStart);		
	}
	_nTickPrev = tick_now;

	unsigned dur = tick_now - _nTickStart;
	unsigned sec = dur / 1000;
	//time_t t = _tmStart + sec;
	time_t t;
	time(&t);
	tm* tmTime = localtime(&t);
	unsigned ms = dur - sec * 1000;
	sprintf(&_pMsgBuffer[strlen(prefix)], "%02d%02d-%02d:%02d:%02d.%03d", tmTime->tm_mon + 1, tmTime->tm_mday, tmTime->tm_hour, tmTime->tm_min, tmTime->tm_sec, ms);

	//msg
	size_t nLen = strlen(_pMsgBuffer);
	_pMsgBuffer[nLen] = ' ';
	vsnprintf(&_pMsgBuffer[nLen + 1], _nMsgBufferLen - nLen - 2, format, args);

	for (AppenderList::iterator i = _AppenderList.begin(); i != _AppenderList.end(); ++i)
	{
		if (appendermode & i->second)
			i->first->outputMsg(_pMsgBuffer);
	}
}

void LOGGER::line_(const char *sText)
{
	for (AppenderList::iterator i = _AppenderList.begin(); i != _AppenderList.end(); ++i)
	{
		i->first->outputMsg(sText);
	}
}

////////////////////////////////////////////////////////////////////////////////
class Appender_Console : public AppenderBase
{
	Appender_Console();
public:
	virtual void outputMsg(const char* msg);
public:
	static Appender_Console* getInstance();
};
Appender_Console::Appender_Console()
{
#if 0
	//#ifdef WIN32
	HANDLE g_hConsole; //创建句柄，详细句柄知识，请百度一下或查MSDN
	g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //实例化句柄
	SetConsoleTextAttribute(g_hConsole, FOREGROUND_RED);//设置字体颜色
	//printf("hello ");
	//SetConsoleTextAttribute(g_hConsole, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	//printf("world!\n");
	//SetConsoleTextAttribute(g_hConsole, BACKGROUND_INTENSITY | BACKGROUND_BLUE);
	//printf("It is really beautiful!\n");

	//------------------------------------------------------------------------------
	getch();
	PROCSETCONSOLEFONT SetConsoleFont;
	PROCGETCONSOLEFONTINFO GetConsoleFontInfo;
	PROCGETCONSOLEFONTSIZE GetConsoleFontSize;
	PROCGETNUMBEROFCONSOLEFONTS GetNumberOfConsoleFonts;
	PROCGETCURRENTCONSOLEFONT GetCurrentConsoleFont;
	HMODULE hKernel32 = GetModuleHandle("kernel32");
	SetConsoleFont = (PROCSETCONSOLEFONT)GetProcAddress(hKernel32, "SetConsoleFont");
	GetConsoleFontInfo = (PROCGETCONSOLEFONTINFO)GetProcAddress(hKernel32, "GetConsoleFontInfo");
	GetConsoleFontSize = (PROCGETCONSOLEFONTSIZE)GetProcAddress(hKernel32, "GetConsoleFontSize");
	GetNumberOfConsoleFonts = (PROCGETNUMBEROFCONSOLEFONTS)GetProcAddress(hKernel32, "GetNumberOfConsoleFonts");
	GetCurrentConsoleFont = (PROCGETCURRENTCONSOLEFONT)GetProcAddress(hKernel32, "GetCurrentConsoleFont");
	SetConsoleFont(g_hConsole, 3);
#endif//WIN32
}
void Appender_Console::outputMsg(const char* msg)
{
#ifdef ANDROID
	android_LogPriority t = ANDROID_LOG_INFO;
	if (strstr(msg, "<Error>")) {
		t = ANDROID_LOG_ERROR;
	}
	__android_log_print(t, "c2fun", msg);
#else
	printf("%s", msg);
#endif
}

////////////////////////////////////////////////////////////////////////////////
class Appender_OutputDebug : private IAppender {
	Appender_OutputDebug() {}
	virtual void outputMsg(const char* msg);
};
void Appender_OutputDebug::outputMsg(const char* msg)
{
#ifdef WIN32
	OutputDebugStringA(msg);
#elif defined(__APPLE__)
	output_debug_string(msg);
#endif
}

////////////////////////////////////////////////////////////////////////////////
class Appender_File : private IAppender {
	Appender_File() :_pLoggerFile(0) {
	}
public:
	virtual bool init(const char* pFileName);
	virtual bool deInit();
	virtual void outputMsg(const char* msg);
	const char*	 getFileName() const;
	void		 setFileName(const char* sFile);
protected:
	FILE*			_pLoggerFile;
	std::string		_sFilename;
};
Appender_File* Appender_File::getInstance()
{
	static Appender_File ins;
	return &ins;
}

bool Appender_File::init(const char* pFileName)
{
	uninit();

	if (pFileName) {
		_sFilename = pFileName;
	}
	else
	{
#ifndef LITE_KERNEL
		_sFilename = l3Kernel_GetExecPath();
		_sFilename += "/";
#endif
		_sFilename += pFileName;
	}

	for (unsigned i = 0; i < _sFilename.size(); ++i)
	{
#ifdef WIN32
		if (_sFilename[i] == '/')
			_sFilename[i] = '\\';
#else
		if (_sFilename[i] == '\\')
			_sFilename[i] = '/';
#endif
	}

	_pLoggerFile = fopen(_sFilename.c_str(), "w+");
	if (_pLoggerFile == 0) {
#ifdef ANDROID
		__android_log_print(ANDROID_LOG_VERBOSE, "c2fun", "write log %s failed\r\n", _sFilename.c_str());
#else
		printf("write log %s failed\r\n", _sFilename.c_str());
#endif
		return false;
	}

#ifndef LITE_KERNEL
	if (rvGetSysInteger("codepage_internal") == 65001) {
		unsigned char utf8_head[] = { 0xEF, 0xBB, 0xBF };
		fwrite(utf8_head, 1, 3, _pLoggerFile);
	}
#endif

	return true;
}

bool Appender_File::uninit()
{
	if (_pLoggerFile)
	{
		fclose(_pLoggerFile);
		return true;
	}
	return false;
}

void Appender_File::outputMsg(const char* msg)
{
	if (_pLoggerFile)
	{
		fprintf(_pLoggerFile, "%s", msg);
		fflush(_pLoggerFile);
	}
}
const char* Appender_File::getFileName() const
{
	return _sFilename.c_str();
}

void Appender_File::setFileName(const char* sFile)
{
	if (sFile && strcmp(_sFilename.c_str(), sFile))
	{
		uninit();
		init(sFile);
	}
}
