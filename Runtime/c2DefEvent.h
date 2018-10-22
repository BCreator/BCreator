#ifndef C2_EVENTDEF_H_
#define C2_EVENTDEF_H_

#include"./c2PreDefined.h"

////////////////////////////////////////////////////////////////////////////////
#define C2EVTBUF_MAXSIZE	131072	//Ϊ����eventȡ���ݵĻ�����������С
#define C2EVTQUEUE_INITSIZE	131072	//ʹ��Memqueue��Ϊ�¼����У��ڲ����������Ľ�������event������˵����
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

struct c2IEvent {//TODO��GLFWȱ���ƶ��豸�ϵ�һЩINPUT��Ϣ��������Ļ��ת��������
	c2EvtType		_esType;
	mutable	Uint64	_esLogicalFrameStamp;	//Logic Frame Stamp.TODO��ʹ��64λ����Ϊ�˹��󣬵��Ծ��ܱ���ô����
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
