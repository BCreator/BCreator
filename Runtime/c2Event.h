#ifndef C2_EVENT_H_
#define C2_EVENT_H_

#include"./Metas/_Defined.h"
////////////////////////////////////////////////////////////////////////////////

enum c2EventType {
	c2ET_None = 0,

	/*------------------------------------------------------------------------*/
	c2ET_RuntimeLogic,	//
	c2ETRL_UPDATEFIX,

	/*------------------------------------------------------------------------*/
	c2ET_Keyboard,
	c2ETKB_ESC,

	/*------------------------------------------------------------------------*/
	c2ET_Mouse,

	/*------------------------------------------------------------------------*/
	c2ET_Joystick,

	/*------------------------------------------------------------------------*/
	c2ET_Touch,

	/*------------------------------------------------------------------------*/
	c2ET_UserDefinedBegin
};

/*============================================================================*/
struct c2Event {
	c2EventType	_nType;
	int			_nFrameStamp;
};

/******************************************************************************/
_C2Interface void c2WaitEvent(c2Event *pEvent);
_C2Interface void c2PumpEvent(c2Event *pEvent);
_C2Interface void c2ProcessEventQueue(c2Event *pEvent);

/******************************************************************************/
class c2::Part;
typedef std::list<const c2::Part*>						c2ListenerList;
typedef stdext::hash_map<c2EventType, c2ListenerList>	c2ListenMap;
_C2Interface void c2ListenEvent(const c2::ARPart &AR, c2EventType EType);
_C2Interface void c2DislistenEvent(const c2::ARPart &AR, c2EventType EType);

////////////////////////////////////////////////////////////////////////////////
#endif//C2_EVENT_H_
