#ifndef C2_EVENTDEF_H_
#define C2_EVENTDEF_H_

#include"./c2PreDefined.h"

////////////////////////////////////////////////////////////////////////////////
#define C2EVTMSG_MAXSIZE	131072	//Ϊ����eventȡ���ݵĻ�����������С
#define C2EVTQUEUE_INITSIZE	131072	//ʹ��Memqueue��Ϊ�¼����У��ڲ����������Ľ�������event������˵����

////////////////////////////////////////////////////////////////////////////////
/*c2IEventû�ж�̬�����Ի��ڶ�̬�����Ե�virtual�����Ͳ����У����籣��size��size������act�*/
#pragma pack(push, 1)
struct c2IEvent {
	Uint32	_esTypeAddChunkOffset;
	mutable	Uint64	_esFixFrameStamp;	//Logic Frame Stamp.TODO��ʹ��64λ����Ϊ�˹��󣬵��Ծ��ܱ���ô����
protected://����ֱ��ʵ����ʹ�ã�ֻ�Ǹ�����
	c2IEvent() : _esTypeAddChunkOffset(0), _esFixFrameStamp(0) {
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

//evt_namespace������evttype_namespace��ͬ��evttype_nameʹ���¼�ö��chunk�ж�����¼�����
#define C2DefOneEvtBegin(evttype_namespace, evt_namespace, evttype_name)	\
namespace evt_namespace {\
struct evttype_name : public c2IEvent {\
	evttype_name(Uint32 ETChunkOffset) : c2IEvent() {\
		_esTypeAddChunkOffset = static_cast<Uint32>(evttype_namespace::evttype_name) + ETChunkOffset;\
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
C2DefOneEvtBegin(C2ET1, C2EVT1, EventTest)
Uint32	_esTest;
char	_s[128];
C2DefOneEvtEnd
#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
#endif//C2_EVENTDEF_H_
