//得完善线程安全。

#ifndef _RAY_KERNEL_LOGGER_NEW_H_
#define _RAY_KERNEL_LOGGER_NEW_H_
//============================================================================================

//============================================================================================
class IAppender {
	virtual bool init()		= 0;
	virtual void outputMsg(const char* msg) = 0;
};

//============================================================================================
class LOGGER {
public:
	enum LOGGERMODE
	{
		LOGGERMODE_INFO		= (1<<0),
		LOGGERMODE_DEBUG	= (1<<1),
		LOGGERMODE_WARN		= (1<<2),
		LOGGERMODE_ERROR	= (1<<3),
		LOGGERMODE_FATAL	= (1<<4),

		LOGGERMODE_ALL		= (1<<0) + (1<<1) + (1<<2) + (1<<3) + (1<<4),
	};
	static IAppender*	APPENDER_FILE;
	static IAppender*	APPENDER_OUTPUTDEBUG;
	static IAppender*	APPENDER_CONSOLE;
	static IAppender*	APPENDER_MDEBUG;
	//----------------------------------------------------------------------------------------
public:
	bool			addAppender(IAppender* appender, unsigned mode, const char* pFileName= NULL);
	bool			removeAppender(IAppender* appender);
	bool			setAppenderMode(IAppender* appender, unsigned mode);
	bool			getAppenderMode(IAppender* appender, unsigned& mode);

	//----------------------------------------------------------------------------------------	
	static void		Info(const char *sFormat, ...);
	static void		Log(const char *sFormat, ...);
	static void		Debug(const char *sFormat, ...);
	static void		Warn(const char *sFormat, ...);
	static void		Error(const char *sFormat, ...);
	static void		Fatal(const char *sFormat, ...);
	static void		Print(const char *sText);
private:

	typedef std::map<IAppender*, unsigned> AppenderList;
	AppenderList    _AppenderList;

	char*			_pMsgBuffer;
	int				_nMsgBufferLen;

	time_t			_tmStart;
	unsigned		_nTickStart, _nTickPrev;
private:
	LOGGER();
	~LOGGER();

	void			line(const char* prefix, unsigned appendermode, const char* format, va_list args);
	void			line_(const char *sText);
public:
	static LOGGER&  getInstance();
};

//============================================================================================
#endif//_RAY_KERNEL_LOGGER_NEW_H_
