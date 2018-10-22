#ifndef C2_APPLICATION_H_
#define C2_APPLICATION_H_

#include<boost/signals2/signal.hpp>
#include"./c2PreDefined.h"
#include"./c2DefEvent.h"

//FIXME: 以下宏暂时定义注释于此，以后会用更现代化的方式来做这方面的事。把这个宏放在这里
//只是为了便于后面以此宏为线索清除和修改相关的代码。
//#define C2_CHECK_MEM

////////////////////////////////////////////////////////////////////////////////
/*
Action体系
*/
struct c2IAction {
	enum class Status
	{
		Invalid,
		Success,
		Failure,
		Running,
	};
	int		_Predicate;
	int		_SubjectID;
	//blackboard;只能是内部状态，不能记录任何体外状态。
	explicit c2IAction();
	//TODO：？返回值有RUNNING是为了后面的OneRounte。
	virtual Status doItNow(const c2IEvent &Evt);
};

////////////////////////////////////////////////////////////////////////////////
/*
Driving framework of the whole application
*/
C2Interface void c2WaitEvent(c2IEvent &Event);
C2Interface void c2PumpEvent(c2IEvent &Event);
C2Interface void c2UpdateLogicFrame(int nLogicFrameStamp);

/******************************************************************************/
//Consumer subscribe event And Producer publish event。
C2Interface void c2SubEvt(const c2IEvent &Evt, const c2IAction &Act);
C2Interface void c2UnsubEvt(const c2IEvent &Evt, const c2IAction &Act);
C2Interface void c2PubEvt(const c2IEvent &Event, const size_t EventSize,
				const Uint64 esLogicalFrameStamp);

////////////////////////////////////////////////////////////////////////////////
/*
Part & Factory
*/
#include"./Metas/Part.h"
using c2APart = c2::Part::ARPart;
C2Interface c2APart c2CreatePart(const char *sClass, const char *sName = nullptr);
C2Interface bool _c2RegistPartClass(const char *sClass, c2::Part::CreationFunc C);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create);

////////////////////////////////////////////////////////////////////////////////
#endif//C2_APPLICATION_H_
