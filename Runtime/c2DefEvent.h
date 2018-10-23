#ifndef C2_EVENTDEF_H_
#define C2_EVENTDEF_H_

#include"./c2PreDefined.h"

////////////////////////////////////////////////////////////////////////////////
#define C2EVTMSG_MAXSIZE	131072	//Ϊ����eventȡ���ݵĻ�����������С
#define C2EVTQUEUE_INITSIZE	131072	//ʹ��Memqueue��Ϊ�¼����У��ڲ����������Ľ�������event������˵����

////////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)
struct c2IEvent {//TODO��GLFWȱ���ƶ��豸�ϵ�һЩINPUT��Ϣ��������Ļ��ת��������
	Uint32			_esType;
	mutable	Uint64	_esLogicalFrameStamp;	//Logic Frame Stamp.TODO��ʹ��64λ����Ϊ�˹��󣬵��Ծ��ܱ���ô����
protected://����ֱ��ʵ����ʹ�ã�ֻ�Ǹ�����
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

//evt_namespace������evttype_namespace��ͬ��evttype_nameʹ���¼�ö��chunk�ж�����¼�����
#define C2DefEvtBegin(evttype_namespace, evt_namespace, evttype_name)	\
namespace evt_namespace {\
struct evttype_name : public c2IEvent {\
	evttype_name(Uint32 ETOffset) : c2IEvent() {\
		_esType = static_cast<Uint32>(evttype_namespace::evttype_name) + ETOffset;\
	}
#define C2DefEvtEnd	\
};\
}//namespace

////////////////////////////////////////////////////////////////////////////////
// event types chunk 1 for test
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
// event types chunk 2 for test
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

////////////////////////////////////////////////////////////////////////////////
// glfw.gleq events
#include"./_c2Application/gleq.h"


#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
#endif//C2_EVENTDEF_H_
