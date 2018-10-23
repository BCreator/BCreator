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
	c2IAction::Status c2IAction::doItNow(const c2IEvent &Evt) {
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
static std::vector<c2IAction*>	g_EvtSignals;

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
	std::cout	<< "EvtType= " << Evt._esType
				<< "  Act Type: " << typeid(Act).name() << std::endl;
		g_EvtSignals[Evt._esType] = &Act;
#if 0//����ʹ��signal2�ķ�ʽ
	g_EvtSignals[Evt._esType].connect(boost::bind(&c2IAction::doItNow, Act, _1));
	//����ʹ�ó�Ա����ָ�������bind�ķ�ʽ�������÷�ʽ�в�ͨ�����¼�fire��ʱ���Ծ���Ҫaction
	c2IAction::ActionFun f = &c2IAction::doItNow;
	(Act.*f)(Evt);//test
//	g_EvtSignals[Evt._esType]= f;
	g_EvtSignals[Evt._esType].connect(f);
#endif
}
C2Interface void c2UnsubEvt(const c2IEvent &Evt, const c2IAction &Act) {
	//	g_EvtSignals[EventType].disconnect(Com);
}

/*============================================================================*/
static c2::tsMemQueue	g_EventQueue(C2EVTQUEUE_INITSIZE);
C2Interface void c2PubEvt(const c2IEvent &Event, const size_t EventSize,
				const Uint64 esLogicalFrameStamp) {	//FIXME: esLogicalFrameStamp��64��es��Ϊ�˹��󣬵���Ȼ�ܱ����⣿
	Event._esLogicalFrameStamp = esLogicalFrameStamp;
	g_EventQueue.push(&Event, EventSize);
}

////////////////////////////////////////////////////////////////////////////////
/*
 Driving framework of the whole application
*/
static char g_pTempEventBuffer4UpdateLogicalFrame[C2EVTMSG_MAXSIZE];
C2Interface void c2UpdateLogicFrame(Uint64 esLogicalFrameStamp) {
	//�ַ���Ϣ
	g_EventQueue.pop(g_pTempEventBuffer4UpdateLogicalFrame, C2EVTMSG_MAXSIZE);
	c2IEvent &evt= *((c2IEvent*)g_pTempEventBuffer4UpdateLogicalFrame);
#if 0//����ʹ��signal2�ķ�ʽ
	g_EvtSignals[evt._esType](evt);//ÿ��signal��ص����slots��
#endif
	BOOST_ASSERT(g_EvtSignals[evt._esType]);
	g_EvtSignals[evt._esType]->doItNow(evt);//��ʱ��֧��һ����Ϣ�����action������
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
