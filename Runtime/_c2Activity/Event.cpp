#include<vector>
#include<queue>
#include<boost/assert.hpp>
#include<boost/signals2/signal.hpp>

#include"../c2Application.h"
#include<iostream>

////////////////////////////////////////////////////////////////////////////////
/*
Command
*/
c2Command::c2Command() : _SubjectID(0) {
}
c2Command::Status c2Command::action() {
	std::cout << "action...." << std::endl;
	return Status::Failure;
}

////////////////////////////////////////////////////////////////////////////////
/*
Subscriber
*/
using EventHandler = boost::signals2::signal<c2Command::Status()>;
static std::vector<EventHandler>	s_EventHandlerVec;
//using c2EventType = std::vector<EventHandler>::size_type;
C2Interface c2EventType c2SubscribeEvent(c2Command &Com) {
	s_EventHandlerVec.push_back(EventHandler());
	c2EventType et = s_EventHandlerVec.size() - 1;
	BOOST_ASSERT(et >= 0);
	s_EventHandlerVec[et].connect(boost::bind(&c2Command::action, Com));
	return et;
}

/******************************************************************************/
C2Interface void c2UnsubscribeEvent(c2EventType EventType, const c2Command &Com) {
//	s_EventHandlerVec[EventType].disconnect(Com);
}

////////////////////////////////////////////////////////////////////////////////
/*
Publisher
*/
static std::queue<c2Event>	s_EventQueue;
C2Interface void c2PublishEvent(const c2Event &Event) {
	s_EventQueue.push(Event);
}

////////////////////////////////////////////////////////////////////////////////
/*
Main loop of Application
*/
static void FireEvent(c2EventType EventType) {
	s_EventHandlerVec[EventType]();
}
C2Interface void c2UpdateLogicFrame(int nLogicFrameStamp) {
	while (!s_EventQueue.empty()) {
		const c2Event &e = s_EventQueue.front();
		FireEvent(e._nType);
		s_EventQueue.pop();
	}
}

C2Interface void c2WaitEvent(c2Event *pEvent) {
	if (!pEvent)
		return;
	/*------------------------------------------------------------------------*/
//	EventQueue.get
}

C2Interface void c2PumpEvent(c2Event *pEvent) {
	if (!pEvent)
		return;
	/*------------------------------------------------------------------------*/
}

////////////////////////////////////////////////////////////////////////////////
