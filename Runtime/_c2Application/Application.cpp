#include<vector>
#include<queue>
#include<iostream>
#include<boost/assert.hpp>
#include<boost/log/trivial.hpp>
#include"../c2Application.h"
#include"./tsMemQueue.h"

////////////////////////////////////////////////////////////////////////////////
/*
Consumer subscribe event And Producer publish event
*/
using SigActByEvt = boost::signals2::signal<c2IAction::Status(const c2IEvent &Evt)>;
static std::vector<SigActByEvt>	g_EvtSignals(C2EVT_ACCOUNT);
C2Interface void c2SubEvt(const c2IEvent &Evt, const c2IAction &Act) {
	g_EvtSignals[Evt._esType].connect(boost::bind(&c2IAction::doItNow, Act, _1));
}
C2Interface void c2UnsubEvt(const c2IEvent &Evt, const c2IAction &Act) {
	//	g_EvtSignals[EventType].disconnect(Com);
}

/*============================================================================*/
static c2::tsMemQueue	g_EventQueue(C2EVTQUEUE_INITSIZE);
C2Interface void c2PubEvt(	const c2IEvent &Event, const size_t EventSize, 
							const Uint64 esLogicalFrameStamp) {//用64的es是为了够大，但仍然跑爆问题？
	Event._esLogicalFrameStamp = esLogicalFrameStamp;
	g_EventQueue.push(&Event, EventSize);
}

////////////////////////////////////////////////////////////////////////////////
/*
 Driving framework of the whole application
*/
static char g_pTempEventBuffer4UpdateLogicalFrame[C2EVTBUF_MAXSIZE];
C2Interface void c2UpdateLogicFrame(int nLogicFrameStamp) {
	g_EventQueue.pop(g_pTempEventBuffer4UpdateLogicalFrame, C2EVTBUF_MAXSIZE);
	c2IEvent &evt= *((c2IEvent*)g_pTempEventBuffer4UpdateLogicalFrame);
	g_EvtSignals[evt._esType](evt);
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

////////////////////////////////////////////////////////////////////////////////
/*
Action system
*/
c2IAction::c2IAction() : _SubjectID(0) {
}
c2IAction::Status c2IAction::doItNow(const c2IEvent &Evt) {
	c2EventTest &evt = (c2EventTest&)Evt;
	std::cout << "[c2IAction::doItNow] Testing Success..." << evt._s << std::endl;
	return Status::Success;
}

////////////////////////////////////////////////////////////////////////////////
/*
Part & Factory
*/
#include"../Metas/Part.h"
c2::Part::CreationDict		c2::Part::_CreationDict;	//FIXME: 暂时放这

C2Interface c2APart c2CreatePart(const char *sClass, const char *sName) {
	if (!sClass)
		return nullptr;
	c2::Part::CreationDict::iterator ci = c2::Part::_CreationDict.find(sClass);
	if (ci == c2::Part::_CreationDict.end())
		return nullptr;
	c2::Part::CreationFunc	create = ci->second;
	BOOST_ASSERT(create);
	return create();
}

C2Interface bool _c2RegistPartClass(const char *sClass, c2::Part::CreationFunc C) {
	if (!sClass || !C)
		return false;
	return c2::Part::_CreationDict.insert(	//如果已存在同样类名注册，则返回false。
		c2::Part::CreationDict::value_type(sClass, C)).second;
}
