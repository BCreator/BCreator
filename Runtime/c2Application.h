#ifndef C2_APPLICATION_H_
#define C2_APPLICATION_H_

#include<boost/signals2/signal.hpp>
#include"./c2PreDefined.h"
#include"./c2DefEvent.h"

//FIXME: 以下宏暂时定义注释于此，以后会用更现代化的方式来做这方面的事。把这个宏放在这里
//只是为了便于后面以此宏为线索清除和修改相关的代码。
//#define C2_CHECK_MEM

////////////////////////////////////////////////////////////////////////////////
/*Action体系*/
struct c2IAction {
	//TODO：？返回值有RUNNING是为了后面的OneRounte。
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
	explicit c2IAction() {}
	virtual Status doItNow(const c2IEvent &Evt, size_t EvtSize) {
		std::cout << "EvType: " << Evt._esType << " -> "
			<< typeid(*this).name() << "::doItNow: success..." << std::endl;
		return Status::Success;
	}
};

////////////////////////////////////////////////////////////////////////////////
/*Driving framework of the whole application*/
C2API void c2AppRun(bool isBlocked, int SwapInterval);

/******************************************************************************/
/*Consumer subscribe event And Producer publish event.*/
C2API void c2SubEvt(const c2IEvent &Evt, c2IAction &Act);
C2API void c2UnsubEvt(const c2IEvent &Evt, c2IAction &Act);
C2API void c2PublishEvt(const c2IEvent &Event, const size_t EventSize,
								const Uint64 esFixFrameStamp);

/******************************************************************************/
/*System events of C2 Application
 注意通过系统事件进行用户自定义操作是假回调，我们的事件体系是基于事件队列，实质用户只有
 异步处理的机会，这导致用户可用功能并不灵活，但符合我们理念，特意不给用户太多选择。*/
C2EvtTypeChunkBegin(c2SysET)
	initialized = 0,
	AMMOUT,
C2EvtTypeChunkEnd

#pragma pack(push, 1)
C2DefOneEvtBegin(c2SysET, c2SysEvt, initialized)
C2DefOneEvtEnd
#pragma pack(pop)

C2API c2SysEvt::initialized& c2GetSysEvtInitialized();

////////////////////////////////////////////////////////////////////////////////
/*Part & Factory*/
#include"./Metas/Part.h"
using c2APart = c2::Part::ARPart;
C2API c2APart c2CreatePart(const char *sClass, const char *sName = nullptr);
C2API bool _c2RegistPartClass(const char *sClass, c2::Part::CreationFunc C);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create);

////////////////////////////////////////////////////////////////////////////////
#endif//C2_APPLICATION_H_
