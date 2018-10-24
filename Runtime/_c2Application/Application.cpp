#include<vector>
#include<queue>
#include<iostream>
#include<boost/assert.hpp>
#include<boost/log/trivial.hpp>
#include"../c2Application.h"
#include"./tsMemQueue.h"

////////////////////////////////////////////////////////////////////////////////
/*
Action system
*/
c2IAction::c2IAction() : _SubjectID(0) {
}

#if 0//尝试使用signal2的方式
c2IAction::Status c2IAction::operator()(const c2IEvent &Evt) {
#endif
	c2IAction::Status c2IAction::doItNow(const c2IEvent &Evt, size_t EvtSize) {
	//	const EventTest &evt = (const EventTest&)Evt;
	const C2EVT2::Mouse &evt = (const C2EVT2::Mouse&)Evt;
	std::cout << typeid(*this).name() << "::doItNow| success..." << std::endl;
	return Status::Success;
}

////////////////////////////////////////////////////////////////////////////////
/*
Consumer subscribe event And Producer publish event
*/
#if 0//尝试使用signal2的方式。bind可能是编译期无法connect到运行期才能确定成员函数调用地址的多态action
using SigActByEvt = boost::signals2::signal<c2IAction::Status (const c2IEvent &Evt)>;
static std::vector<SigActByEvt>	g_EvtSignals;
//尝试使用成员函数指针而不用bind的方式。但调用方式行不通，在事件fire的时候仍旧需要action
using SigActByEvt = c2IAction::ActionFun;
#endif
//static std::vector<c2IAction*>	g_EvtSignals;
#include<set>
//若使用multiset能实现类似boost::signal2对slot的灵活管控，TODO：以后再完善
using Signal = std::set<c2IAction*>;	//暂时不可有重复，故用set
static std::vector<Signal>	g_EvtSignals;

/*
返回值为自定义事件类型Chunk的偏移值。每一个应用程序运行前就应该明确的，软件不能所以插拔
进来的event chunk，因为这会导致每个chunk的offect变化，同程序所匹配配合的其他网络配合端
或者序列化（例如录像，undo/redo等）所存数据而产生不兼容。
*/
Uint32 c2AppendEvtTypesChunk(Uint32 nNewChunkSize) {
	Uint32 ret = g_EvtSignals.size();
	g_EvtSignals.resize(ret + nNewChunkSize);
	return ret;
}

#if 0//尝试使用成员函数指针的方式。但调用方式行不通
//C2Interface void c2SubEvt(const c2IEvent &Evt, c2IAction::ActionFun pFunAct) {
#endif
C2Interface void c2SubEvt(const c2IEvent &Evt, c2IAction &Act) {
	Signal &sig = g_EvtSignals[Evt._esType];
	sig.insert(&Act);
}
C2Interface void c2SubEvt(Uint32 ETChunkOffset, Uint32 EvtType, c2IAction &Act) {
	//g_EvtSignals[EvtType] = &Act;
	Signal &sig = g_EvtSignals[ETChunkOffset+EvtType];
	sig.insert(&Act);
#if 0//尝试使用signal2的方式
	g_EvtSignals[EvtType].connect(boost::bind(&c2IAction::doItNow, Act, _1));
	//尝试使用成员函数指针而不用bind的方式。但调用方式行不通，在事件fire的时候仍旧需要action
	c2IAction::ActionFun f = &c2IAction::doItNow;
	(Act.*f)(Evt);//test
//	g_EvtSignals[EvtType]= f;
	g_EvtSignals[EvtType].connect(f);
#endif
}
C2Interface void c2UnsubEvt(Uint32 ETChunkOffset, Uint32 EvtType, const c2IAction &Act) {
	//	g_EvtSignals[EvtType].disconnect(Com);
}
/*只是先投递移除操作，实际移除在主循环消息处理帧主函数里进行，因为此函数会可能在任
何时候调用，不能破坏sig里的set及sigs。*/
#include<utility>
static std::list<std::pair<Uint32, c2IAction*>>	g_UnsubEvtList;
C2Interface void c2UnsubEvt(const c2IEvent &Evt, c2IAction &Act) {
	g_UnsubEvtList.push_back(std::make_pair(Evt._esType, &Act));
}

////////////////////////////////////////////////////////////////////////////////
/*
 Driving framework of the whole application
*/
static c2::tsMemQueue	g_EventQueue(C2EVTQUEUE_INITSIZE);
C2Interface void c2PublishEvt(const c2IEvent &Event, const size_t EventSize,
	const Uint64 esLogicalFrameStamp) {	//FIXME: esLogicalFrameStamp用64的es是为了够大，但仍然跑爆问题？
	Event._esLogicalFrameStamp = esLogicalFrameStamp;
	g_EventQueue.push(&Event, EventSize);
}

static char g_pTempEventBuffer4UpdateLogicalFrame[C2EVTMSG_MAXSIZE];
C2Interface void c2UpdateLogicFrame(Uint64 esLogicalFrameStamp) {
	//分发消息
	static size_t st_evtsize;
	while (!g_EventQueue.isEmpty()) {
		st_evtsize= g_EventQueue.pop(g_pTempEventBuffer4UpdateLogicalFrame, C2EVTMSG_MAXSIZE);
		c2IEvent &evt = *((c2IEvent*)g_pTempEventBuffer4UpdateLogicalFrame);
#if 0//尝试使用signal2的方式
		g_EvtSignals[evt._esType](evt);//每个signal会回调多个slots。
#endif
		for (c2IAction *tp_act : g_EvtSignals[evt._esType]) {
		//for each (auto tp_act in sig) {
			BOOST_ASSERT(tp_act);
			tp_act->doItNow(evt, st_evtsize);
		}
	}
	//实际退订消息，进行删除
	//for (std::pair<Uint32, c2IAction*> &tp : g_UnsubEvtList) {
	for (std::pair<Uint32, c2IAction*> tp : g_UnsubEvtList) {
			g_EvtSignals[tp.first].erase(tp.second);
	}
	g_UnsubEvtList.clear();
}

/*============================================================================*/
C2Interface void c2WaitEvent() {
	glfwWaitEvents();
}

C2Interface void c2PumpEvent() {
	glfwPollEvents();
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
