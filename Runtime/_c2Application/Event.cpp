#include<vector>
#include<queue>
#include<boost/assert.hpp>
#include"../c2Application.h"
#include<iostream>

////////////////////////////////////////////////////////////////////////////////
/*
 Event system
*/
c2IAction::c2IAction() : _SubjectID(0) {
}
c2IAction::Status c2IAction::doItNow(const c2EventType &EventType) {
	std::cout << "action...." << std::endl;
	return Status::Failure;
}

/******************************************************************************/
// Consumer subscribe event And Producer publish event
static std::vector<c2EvtSigConsumer>	g_EventHandlers;
C2Interface c2EventType& c2SubEvt(const c2EvtConsumer &Consumer,
									const c2EventType &EventType) {
	g_EventHandlers.push_back(c2EvtSigConsumer());
	c2EventType et = g_EventHandlers.size() - 1;
	BOOST_ASSERT(et >= 0);
	g_EventHandlers[et].connect(Consumer);
	return et;
}
C2Interface void c2UnsubEvt(const c2EvtConsumer &Consumer,
							const c2EventType &EventType) {
	//	g_EventHandlers[EventType].disconnect(Com);
}

/*============================================================================*/
static std::queue<c2IEvent>	s_EventQueue;
C2Interface void c2PubEvt(const c2IEvent &Event) {
	s_EventQueue.push(Event);
}

////////////////////////////////////////////////////////////////////////////////
/*
 Driving framework of the whole application
*/
C2Interface void c2UpdateLogicFrame(int nLogicFrameStamp) {
	while (!s_EventQueue.empty()) {
		const c2IEvent &e = s_EventQueue.front();
		g_EventHandlers[e._nType](1000);
		s_EventQueue.pop();
	}
}

C2Interface void c2WaitEvent(c2IEvent *pEvent) {
	if (!pEvent)
		return;
	/*------------------------------------------------------------------------*/
//	EventQueue.get
}

C2Interface void c2PumpEvent(c2IEvent *pEvent) {
	if (!pEvent)
		return;
	/*------------------------------------------------------------------------*/
}
