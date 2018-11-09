#pragma once

#include<boost/signals2/signal.hpp>
#include"./ThirdParty/imgui/imgui.h"

//FIXME: 以下宏暂时定义注释于此，以后会用更现代化的方式来做这方面的事。把这个宏放在这里
//只是为了便于后面以此宏为线索清除和修改相关的代码。
//#define C2_CHECK_MEM

////////////////////////////////////////////////////////////////////////////////
/*Action体系*/
#include"./_c2Application/BrainTree.h"
struct c2IAction : public BrainTree::BehaviorTree {
	////TODO：？返回值有RUNNING是为了后面的OneRounte。
	//enum class Status
	//{
	//	Invalid,
	//	Success,
	//	Failure,
	//	Running,
	//};
	////int		_Predicate;
	////int		_SubjectID;
	////blackboard;只能是内部状态，不能记录任何体外状态。
	c2IAction() : _pEvt(nullptr) {}
	const c2IEvent*	_pEvt;
	virtual Status update() {
		BOOST_ASSERT(_pEvt );
		std::cout << "EvType: " << _pEvt->_esTypeAddChunkOffset << " -> "
			<< typeid(*this).name() << "::update: success..." << std::endl;
		return Status::Success;
	}
};

////////////////////////////////////////////////////////////////////////////////
/*Driving framework of the whole application*/
C2API void c2AppRun(bool isBlocked, int SwapInterval,
					int nWndWidth, int nWndHeight, const char *sWndCaption);

/******************************************************************************/
/*Consumer subscribe event And Producer publish event.*/
C2API void c2asActSubEvt(c2IAction &Act, Uint32 esEvtTypeAddChunkOffset, size_t EvtSize);
C2API void c2asActUnsubEvt(c2IAction &Act, Uint32 esEvtTypeAddChunkOffset);
C2API void c2PublishEvt(const c2IEvent &Event, const size_t EventSize,
								const Uint64 esFixFrameStamp);

/******************************************************************************/
/*System events of C2 Application
 注意通过系统事件进行用户自定义操作是假回调，我们的事件体系是基于事件队列，实质用户只有
 异步处理的机会，这导致用户可用功能并不灵活，但符合我们理念，刻意不给用户太多选择。*/
C2EvtTypeChunkBegin(c2SysET)
initialized = 0,
updatefixframe,
updateframe,
AMMOUT,
C2EvtTypeChunkEnd

#pragma pack(push, 1)
C2DefOneEvtBegin(c2SysET, c2SysEvt, initialized)
C2DefOneEvtEnd

C2DefOneEvtBegin(c2SysET, c2SysEvt, updatefixframe)
mutable Uint32 _esElapsed;
C2DefOneEvtEnd

C2DefOneEvtBegin(c2SysET, c2SysEvt, updateframe)
mutable Uint32 _esElapsed;
C2DefOneEvtEnd
#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
/*Part & Factory*/
#include"./c2Foundation/c2Part.h"
using c2APart = c2Part::ARPart;
C2API c2APart c2CreatePart(const char *sClass, const char *sName = nullptr);
C2API bool _c2RegistPartClass(const char *sClass, c2Part::CreationFunc C);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create);
