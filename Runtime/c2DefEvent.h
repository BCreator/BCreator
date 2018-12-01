#ifndef C2_EVENTDEF_H_
#define C2_EVENTDEF_H_

#include"./c2PreDefined.h"

////////////////////////////////////////////////////////////////////////////////
#define C2EVTMSG_MAXSIZE	131072	//为单个event取数据的缓冲容器最大大小
#define C2EVTQUEUE_INITSIZE	131072	//使用Memqueue作为事件队列，内部可自增。改进意见请见event框架相关说明。

////////////////////////////////////////////////////////////////////////////////
/*c2IEvent没有多态，所以基于多态的特性的virtual操作就不能有（例如保存size，size放在了act里）*/
#pragma pack(push, 1)
struct c2IEvent {
	friend void c2PublishEvt(const c2IEvent &Event, size_t EventSize,
								const Uint64 esFixFrameStamp);
	inline Uint32 getTypeAddChunkOffset() const {
		return _nTypeAddChunkOffset;
	}
protected://不能直接实例化使用，只是个类型
	Uint32	_nTypeAddChunkOffset;
	mutable	Uint64	_nFixFrameStamp;	//Logic Frame Stamp.TODO：使用64位数是为了够大，但仍旧跑爆怎么处理？
	c2IEvent() : _nTypeAddChunkOffset(0), _nFixFrameStamp(0) {
	}
};
#pragma pack(pop)

/******************************************************************************/
C2API Uint32 c2AppendEvtTypesChunk(Uint32 nNewChunkSize);
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
		_nTypeAddChunkOffset = static_cast<Uint32>(evttype_namespace::evttype_name) + ETChunkOffset;\
	}
#define C2DefOneEvtEnd	\
};\
}//namespace

////////////////////////////////////////////////////////////////////////////////
/*System events of C2 Application
 注意通过系统事件进行用户自定义操作是假回调，我们的事件体系是基于事件队列，实质用户只有
 异步处理的机会，这导致用户可用功能并不灵活，但符合我们理念，刻意不给用户太多选择。*/
C2EvtTypeChunkBegin(c2SysET)
initialized = 0,
terminate,

mouse_button,
cursor_moved,
cursor_enter,
scrolled,
key,
char_input,
charmods_input,

/*TODO*/
window_maximized,
window_unmaximized,
window_moved,
window_resized,
window_closed,
window_refresh,
window_focused,
window_defocused,
window_iconified,
window_uniconified,
windowframebuffer_resized,

AMOUNT,
C2EvtTypeChunkEnd

/******************************************************************************/
struct GLFWwindow;
#pragma pack(push, 1)
C2DefOneEvtBegin(c2SysET, c2SysEvt, initialized)
GLFWwindow*	_pWnd;
C2DefOneEvtEnd

C2DefOneEvtBegin(c2SysET, c2SysEvt, terminate)
C2DefOneEvtEnd

/******************************************************************************/
C2DefOneEvtBegin(c2SysET, c2SysEvt, mouse_button)
GLFWwindow*	_pWnd;
//Uint8		_nButton : 4;
//Uint8		_nAction : 1;
//Uint8		_nModifier : 3;
Uint8		_nButton;
Uint8		_nAction;
Uint8		_nModifier;
C2DefOneEvtEnd

C2DefOneEvtBegin(c2SysET, c2SysEvt, cursor_moved)
GLFWwindow*	_pWnd;
double		_x;
double		_y;
C2DefOneEvtEnd

C2DefOneEvtBegin(c2SysET, c2SysEvt, cursor_enter)
GLFWwindow*	_pWnd;
Uint8		_bEnter;//false is left;
C2DefOneEvtEnd

C2DefOneEvtBegin(c2SysET, c2SysEvt, scrolled)
GLFWwindow*	_pWnd;
double		_x;
double		_y;
C2DefOneEvtEnd

C2DefOneEvtBegin(c2SysET, c2SysEvt, key)
GLFWwindow*	_pWnd;
Uint16		_nKey;
Uint16		_nScancode;
Uint8		_nAction;
Uint8		_nModifier;
C2DefOneEvtEnd

C2DefOneEvtBegin(c2SysET, c2SysEvt, char_input)
GLFWwindow*	_pWnd;
Uint32		_nCodePoint;
C2DefOneEvtEnd

C2DefOneEvtBegin(c2SysET, c2SysEvt, charmods_input)
GLFWwindow*	_pWnd;
Uint32		_nCodePoint;
Uint8		_nModifier;
C2DefOneEvtEnd

#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
#endif//C2_EVENTDEF_H_
