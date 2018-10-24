#ifndef C2_EVENTDEF_H_
#define C2_EVENTDEF_H_

#include"./c2PreDefined.h"

////////////////////////////////////////////////////////////////////////////////
#define C2EVTMSG_MAXSIZE	131072	//为单个event取数据的缓冲容器最大大小
#define C2EVTQUEUE_INITSIZE	131072	//使用Memqueue作为事件队列，内部可自增。改进意见请见event框架相关说明。

////////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)
struct c2IEvent {//TODO：GLFW缺少移动设备上的一些INPUT消息，例如屏幕翻转、重力等
	Uint32			_esType;
	mutable	Uint64	_esLogicalFrameStamp;	//Logic Frame Stamp.TODO：使用64位数是为了够大，但仍旧跑爆怎么处理？
protected://不能直接实例化使用，只是个类型
	c2IEvent() : _esType(0), _esLogicalFrameStamp(0) {
	}
};

/*============================================================================*/
Uint32 c2AppendEvtTypesChunk(Uint32 nNewChunkSize);
#pragma pack(pop)
#define C2EvtTypeChunkBegin(evttype_namespace)	\
						namespace evttype_namespace {\
						enum {//c2EvtType
#define C2EvtTypeChunkEnd				\
						};\
						}//namespace

//evt_namespace不能与evttype_namespace相同。evttype_name使用事件枚举chunk中定义的事件名。
#define C2DefEvtBegin(evttype_namespace, evt_namespace, evttype_name)	\
namespace evt_namespace {\
struct evttype_name : public c2IEvent {\
	evttype_name(Uint32 ETChunkOffset) : c2IEvent() {\
		_esType = static_cast<Uint32>(evttype_namespace::evttype_name) + ETChunkOffset;\
	}
#define C2DefEvtEnd	\
};\
}//namespace

////////////////////////////////////////////////////////////////////////////////
/*event types chunk 1 for test*/
C2EvtTypeChunkBegin(C2ET1)
	Unknown = 0,
	EventTest,
	EVENTTYPE_AMMOUT,
C2EvtTypeChunkEnd

#pragma pack(push, 1)
//C2DefEvtBegin(C2ET_, EventTest)
C2DefEvtBegin(C2ET1, C2EVT1, EventTest)
Uint32	_esTest;
char	_s[128];
C2DefEvtEnd
#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
/*event types chunk 2 for test*/
C2EvtTypeChunkBegin(C2ET2)
	Keyboard = 0,
	Mouse,
	EVENTTYPE_AMMOUT,
C2EvtTypeChunkEnd

#pragma pack(push, 1)
C2DefEvtBegin(C2ET2, C2EVT2, Keyboard)
Uint32	_esTest;
char	_s[128];
C2DefEvtEnd

C2DefEvtBegin(C2ET2, C2EVT2, Mouse)
Uint32	_esTest;
char	_s[128];
C2DefEvtEnd
#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
/*
glfw.gleq events。GLEQ内的事件长度其实并不足够明确，并且没有明确的字节对齐。暂时又不想
直接修改gleq.h文件。
*/
C2EvtTypeChunkBegin(c2gleqet)
c2GLEQevent = 0,
EVENTTYPE_AMMOUT,
C2EvtTypeChunkEnd

#include"./_c2Application/gleq.h"
#pragma pack(push, 1)
/*可以考虑把GLEQ整个当一个消息类型，然后都交给他处理*/
C2DefEvtBegin(c2gleqet, c2gleqevts, c2GLEQevent)
GLEQevent	_GLEQevent;
C2DefEvtEnd
#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
#endif//C2_EVENTDEF_H_
