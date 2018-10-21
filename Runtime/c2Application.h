#ifndef C2_APPLICATION_H_
#define C2_APPLICATION_H_

#include<boost/signals2/signal.hpp>
#include"./Metas/PreDefined.h"

////////////////////////////////////////////////////////////////////////////////
/*
Part
*/
#include"Metas/Part.h"
using c2APart = c2::Part::ARPart;
C2Interface c2APart c2CreatePart(const char *sClass,
	const char *sName = nullptr);
C2Interface bool _c2RegistPartClass(const char *sClass, c2::Part::CreationFunc C);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create);

////////////////////////////////////////////////////////////////////////////////
/*
Event system
*/
using c2EventType = int;//FIXME
struct c2IEvent {//TODO：GLFW缺少移动设备上的一些INPUT消息，例如屏幕翻转、重力等
	c2EventType	_nType;
	int			_nLFStamp;	//Logic Frame Stamp.
};

/*============================================================================*/
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
	Status doItNow(const c2EventType &EventType);
};

/******************************************************************************/
//Consumer subscribe event And Producer publish event。通用订阅函数故意没有暴露
//根据实际需要，仅仅只是暴露操作Part的。
using c2EvtSigConsumer	= boost::signals2::signal<
							c2IAction::Status(const c2EventType &EventType) >;
using c2EvtConsumer		= c2EvtSigConsumer::slot_type;
C2Interface c2EventType& c2SubEvt(const c2EvtConsumer &Consumer,
									const c2EventType &EventType);
C2Interface void c2UnsubEvt(const c2EvtConsumer &Consumer,
									const c2EventType &EventType);
C2Interface void c2PubEvt(const c2IEvent &Event);

////////////////////////////////////////////////////////////////////////////////
/*
Driving framework of the whole application
*/
C2Interface void c2WaitEvent(c2IEvent *pEvent);
C2Interface void c2PumpEvent(c2IEvent *pEvent);
C2Interface void c2UpdateLogicFrame(int nLogicFrameStamp);

//FIXME: 以下宏暂时定义注释于此，以后会用更现代化的方式来做这方面的事。把这个宏放在这里
//只是为了便于后面以此宏为线索清除和修改相关的代码。
//#define C2_CHECK_MEM

////////////////////////////////////////////////////////////////////////////////
#endif//C2_APPLICATION_H_
