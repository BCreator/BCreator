#include<mutex>
#include<iostream>
#include<boost/assert.hpp>
#include<boost/log/trivial.hpp>

#include"../c2DefEvent.h"
#include"./tsMemQueue.h"

////////////////////////////////////////////////////////////////////////////////
/*
Consumer subscribe event And Producer publish event
*/
#include<unordered_set>

/*��ʱ�������ظ�������set����ʹ��multiset��ʵ������boost::signal2��slot�����ܿء�
TODO���Ժ�������*/
static std::vector<std::unordered_set<c2IAction*>> g_Evt2ActsetVector;

/*����ֵΪ�Զ����¼�����Chunk��ƫ��ֵ��ÿһ��Ӧ�ó�������ǰ��Ӧ����ȷ�ģ�����������Բ��
������event chunk����Ϊ��ᵼ��ÿ��chunk��offect�仯��ͬ������ƥ����ϵ�����������϶�
�������л�������¼��undo/redo�ȣ��������ݶ����������ݡ�*/
Uint32 c2AppendEvtTypesChunk(Uint32 nNewChunkSize) {
	Uint32 ret = g_Evt2ActsetVector.size();
	g_Evt2ActsetVector.resize(ret + nNewChunkSize);
	return ret;
}

/*============================================================================*/
/*ֻ����Ͷ�ݲ�����¼��ʵ�ʲ�������ѭ����Ϣ����֡����������У���Ϊ���Ļ��˶����ܻ�����
��ʱ�򣨲������а�ʾ�ı����Ƕ��߳�̬�ȣ������ã������ƻ�sig���set��sigs��
TODO:����Ҫ���ܲ���һ�£�����ʹ�ö�����ĳ�¼���ACT�������ٶ��Ļ��˶�����pub�¼�����
ACT���ٶ��Ļ��˶����ȵȡ�*/
static std::list<std::pair<c2IAction*, Uint32>>	g_ActSubEvtList;
C2API void c2asActSubEvt(c2IAction &Act,
						Uint32 esEvtTypeAddChunkOffset, size_t EvtSize) {
	g_ActSubEvtList.push_back(std::make_pair(&Act, esEvtTypeAddChunkOffset));
}
static std::list<std::pair<c2IAction*, Uint32>>	g_ActUnsubEvtList;
C2API void c2asActUnsubEvt(c2IAction &Act, Uint32 esEvtTypeAddChunkOffset) {
	g_ActUnsubEvtList.push_back(std::make_pair(&Act, esEvtTypeAddChunkOffset));
}

/*Driving framework of the whole application*/
static c2::tsMemQueue g_EventQueue(C2EVTQUEUE_INITSIZE);
C2API void c2PublishEvt(const c2IEvent &Event, size_t EventSize,
		const Uint64 esFixFrameStamp) {//FIXME: ��64��es��Ϊ�˹��󣬵���Ȼ�ܱ����⣿
	Event._nFixFrameStamp = esFixFrameStamp;
	g_EventQueue.push(&Event, EventSize);
}

void ProcessEvts() {
	/*������һ֡ʵ�ʶ�����Ϣ��*/
	for (std::pair<c2IAction*, Uint32>& v : g_ActSubEvtList) {
		BOOST_ASSERT(v.first);
		g_Evt2ActsetVector[v.second].insert(v.first);
	}
	g_ActSubEvtList.clear();
	/*������һ֡ʵ���˶���Ϣ������ɾ����*/
	for (std::pair<c2IAction*, Uint32>& v : g_ActUnsubEvtList) {
		BOOST_ASSERT(v.first);
		g_Evt2ActsetVector[v.second].erase(v.first);
	}
	g_ActUnsubEvtList.clear();
	/*�ַ���Ϣ*/
	static char temp_evtbuf[C2EVTMSG_MAXSIZE];
	while (!g_EventQueue.isEmpty()) {
		g_EventQueue.pop(temp_evtbuf, C2EVTMSG_MAXSIZE);
		c2IEvent &evt = *((c2IEvent*)temp_evtbuf);
		for (c2IAction *tp_act : g_Evt2ActsetVector[evt.getTypeAddChunkOffset()]) {
			//for each (auto tp_act in sig) {
			BOOST_ASSERT(tp_act);
			tp_act->_pEvt = &evt;//���ݴ�event��Ϊ��������Ҫ��BrainTree��update�޲�������������Ҳ������ͨ��update�������������
			tp_act->update();
			tp_act->_pEvt = nullptr;//���������eventָ�����á�
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
/*
Action
*/
c2IAction::Status c2IAction::update() {
	BOOST_ASSERT(_pEvt);
	std::cout << "EvType: " << _pEvt->getTypeAddChunkOffset() << " -> "
		<< typeid(*this).name() << "::update: success..." << std::endl;
	return Status::Success;
}
