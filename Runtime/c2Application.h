#ifndef C2_EVENT_H_
#define C2_EVENT_H_

#include"./Metas/PreDefined.h"

////////////////////////////////////////////////////////////////////////////////
/*
Command
*/
struct c2Command {
	enum class Status
	{
		Invalid,
		Success,
		Failure,
		Running,
	};
	int		_Predicate;
	int		_SubjectID;
	explicit c2Command();
	Status action();
};

////////////////////////////////////////////////////////////////////////////////
/*
Subscriber
*/
using c2EventType = int;//FIXME
struct c2Event {
	c2EventType	_nType;
	int			_nLFStamp;	//Logic Frame Stamp.
};
C2Interface c2EventType c2SubscribeEvent(c2Command &Com);
C2Interface void c2UnsubscribeEvent(c2EventType EventType, const c2Command &Com);

////////////////////////////////////////////////////////////////////////////////
/*
Publisher
*/
C2Interface void c2PublishEvent(const c2Event &EventType);

////////////////////////////////////////////////////////////////////////////////
/*
Main loop of Application
*/
C2Interface void c2WaitEvent(c2Event *pEvent);
C2Interface void c2PumpEvent(c2Event *pEvent);
C2Interface void c2UpdateLogicFrame(int nLogicFrameStamp);

////////////////////////////////////////////////////////////////////////////////
/*
Part
*/
#include"Metas/Part.h"
C2Interface c2::Part::ARPart c2CreatePart(const char *sClass,
											const char *sName = nullptr);
C2Interface bool _c2RegistPartClass(const char *sClass, c2::Part::CreationFunc C);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create);

////////////////////////////////////////////////////////////////////////////////
#endif//C2_EVENT_H_
