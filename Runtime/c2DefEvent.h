#ifndef C2_EVENTDEF_H_
#define C2_EVENTDEF_H_

#include"./c2PreDefined.h"

////////////////////////////////////////////////////////////////////////////////
#define C2EVTBUF_MAXSIZE	131072	//为单个event取数据的缓冲容器最大大小
#define C2EVTQUEUE_INITSIZE	131072	//使用Memqueue作为事件队列，内部可自增。改进意见请见event框架相关说明。
enum c2EvtType {
	C2T_Unknown= 0,
	C2T_c2EventTest,
	C2EVT_ACCOUNT,
};

////////////////////////////////////////////////////////////////////////////////
#define C2DefEvtBegin(classname)	\
struct classname : public c2IEvent {\
	classname() : c2IEvent() {\
		_esType = C2T_##classname;\
	}
#define C2DefEvtEnd	\
};

#pragma pack(push, 1)

struct c2IEvent {//TODO：GLFW缺少移动设备上的一些INPUT消息，例如屏幕翻转、重力等
	c2EvtType		_esType;
	mutable	Uint64	_esLogicalFrameStamp;	//Logic Frame Stamp.TODO：使用64位数是为了够大，但仍旧跑爆怎么处理？
	c2IEvent() : _esType(C2T_Unknown),
				_esLogicalFrameStamp(0) {
	}
};

C2DefEvtBegin(c2EventTest)
Uint32	_esTest;
char	_s[128];
C2DefEvtEnd

//struct _test_c2IEvent : public c2IEvent {
//};

#pragma pack(pop)


////////////////////////////////////////////////////////////////////////////////
#endif//C2_EVENTDEF_H_
