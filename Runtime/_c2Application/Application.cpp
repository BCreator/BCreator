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

#if 0//����ʹ��signal2�ķ�ʽ
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
#if 0//����ʹ��signal2�ķ�ʽ��bind�����Ǳ������޷�connect�������ڲ���ȷ����Ա�������õ�ַ�Ķ�̬action
using SigActByEvt = boost::signals2::signal<c2IAction::Status (const c2IEvent &Evt)>;
static std::vector<SigActByEvt>	g_EvtSignals;
//����ʹ�ó�Ա����ָ�������bind�ķ�ʽ�������÷�ʽ�в�ͨ�����¼�fire��ʱ���Ծ���Ҫaction
using SigActByEvt = c2IAction::ActionFun;
#endif
//static std::vector<c2IAction*>	g_EvtSignals;
#include<set>
//��ʹ��multiset��ʵ������boost::signal2��slot�����ܿأ�TODO���Ժ�������
using Signal = std::set<c2IAction*>;	//��ʱ�������ظ�������set
static std::vector<Signal>	g_EvtSignals;

/*
����ֵΪ�Զ����¼�����Chunk��ƫ��ֵ��ÿһ��Ӧ�ó�������ǰ��Ӧ����ȷ�ģ�����������Բ��
������event chunk����Ϊ��ᵼ��ÿ��chunk��offect�仯��ͬ������ƥ����ϵ�����������϶�
�������л�������¼��undo/redo�ȣ��������ݶ����������ݡ�
*/
Uint32 c2AppendEvtTypesChunk(Uint32 nNewChunkSize) {
	Uint32 ret = g_EvtSignals.size();
	g_EvtSignals.resize(ret + nNewChunkSize);
	return ret;
}

#if 0//����ʹ�ó�Ա����ָ��ķ�ʽ�������÷�ʽ�в�ͨ
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
#if 0//����ʹ��signal2�ķ�ʽ
	g_EvtSignals[EvtType].connect(boost::bind(&c2IAction::doItNow, Act, _1));
	//����ʹ�ó�Ա����ָ�������bind�ķ�ʽ�������÷�ʽ�в�ͨ�����¼�fire��ʱ���Ծ���Ҫaction
	c2IAction::ActionFun f = &c2IAction::doItNow;
	(Act.*f)(Evt);//test
//	g_EvtSignals[EvtType]= f;
	g_EvtSignals[EvtType].connect(f);
#endif
}
C2Interface void c2UnsubEvt(Uint32 ETChunkOffset, Uint32 EvtType, const c2IAction &Act) {
	//	g_EvtSignals[EvtType].disconnect(Com);
}
/*ֻ����Ͷ���Ƴ�������ʵ���Ƴ�����ѭ����Ϣ����֡����������У���Ϊ�˺������������
��ʱ����ã������ƻ�sig���set��sigs��*/
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
	const Uint64 esLogicalFrameStamp) {	//FIXME: esLogicalFrameStamp��64��es��Ϊ�˹��󣬵���Ȼ�ܱ����⣿
	Event._esLogicalFrameStamp = esLogicalFrameStamp;
	g_EventQueue.push(&Event, EventSize);
}

static char g_pTempEventBuffer4UpdateLogicalFrame[C2EVTMSG_MAXSIZE];
C2Interface void c2UpdateLogicFrame(Uint64 esLogicalFrameStamp) {
	//�ַ���Ϣ
	static size_t st_evtsize;
	while (!g_EventQueue.isEmpty()) {
		st_evtsize= g_EventQueue.pop(g_pTempEventBuffer4UpdateLogicalFrame, C2EVTMSG_MAXSIZE);
		c2IEvent &evt = *((c2IEvent*)g_pTempEventBuffer4UpdateLogicalFrame);
#if 0//����ʹ��signal2�ķ�ʽ
		g_EvtSignals[evt._esType](evt);//ÿ��signal��ص����slots��
#endif
		for (c2IAction *tp_act : g_EvtSignals[evt._esType]) {
		//for each (auto tp_act in sig) {
			BOOST_ASSERT(tp_act);
			tp_act->doItNow(evt, st_evtsize);
		}
	}
	//ʵ���˶���Ϣ������ɾ��
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
c2::Part::CreationDict		c2::Part::_CreationDict;	//FIXME: ��ʱ����

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
	return c2::Part::_CreationDict.insert(	//����Ѵ���ͬ������ע�ᣬ�򷵻�false��
		c2::Part::CreationDict::value_type(sClass, C)).second;
}
