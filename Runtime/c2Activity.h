#ifndef C2_EVENT_H_
#define C2_EVENT_H_

#include"./Metas/PreDefined.h"
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
C2Interface void c2WaitEvent(c2Event *pEvent);
C2Interface void c2PumpEvent(c2Event *pEvent);
C2Interface void c2ProcessEventQueue(c2Event *pEvent);

/******************************************************************************/
class c2::Part;
using c2ListenerList= std::list<const c2::Part*>;
using c2ListenMap	= stdext::hash_map<c2EventType, c2ListenerList>;
C2Interface void c2SubscribeEvent(const c2::Part::ARPart &AR, c2EventType EType);
C2Interface void c2UnsubscribeEvent(const c2::Part::ARPart &AR, c2EventType EType);


////////////////////////////////////////////////////////////////////////////////
C2Interface c2::Part::ARPart c2CreatePart(const char *sClass, const char *sName = nullptr);
C2Interface bool _c2RegistPartClass(const char *sClass, c2::Part::CreationFunc C);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create);

////////////////////////////////////////////////////////////////////////////////
#endif//C2_EVENT_H_
