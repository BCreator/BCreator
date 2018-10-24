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
	mutable	Uint64	_esFixFrameStamp;	//Logic Frame Stamp.TODO：使用64位数是为了够大，但仍旧跑爆怎么处理？
protected://不能直接实例化使用，只是个类型
	c2IEvent() : _esType(0), _esFixFrameStamp(0) {
	}
};

/******************************************************************************/
C2API Uint32 c2AppendEvtTypesChunk(Uint32 nNewChunkSize);
#pragma pack(pop)
#define C2EvtTypeChunkBegin(evttype_namespace)	\
						namespace evttype_namespace {\
						enum {//c2EvtType
#define C2EvtTypeChunkEnd				\
						};\
						}//namespace

//evt_namespace不能与evttype_namespace相同。evttype_name使用事件枚举chunk中定义的事件名。
#define C2DefOneEvtBegin(evttype_namespace, evt_namespace, evttype_name)	\
namespace evt_namespace {\
struct evttype_name : public c2IEvent {\
	evttype_name(Uint32 ETChunkOffset) : c2IEvent() {\
		_esType = static_cast<Uint32>(evttype_namespace::evttype_name) + ETChunkOffset;\
	}
#define C2DefOneEvtEnd	\
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
//C2DefOneEvtBegin(C2ET_, EventTest)
C2DefOneEvtBegin(C2ET1, C2EVT1, EventTest)
Uint32	_esTest;
char	_s[128];
C2DefOneEvtEnd
#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
#endif//C2_EVENTDEF_H_
