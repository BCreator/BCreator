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

/*暂时不可有重复，故用set。若使用multiset能实现类似boost::signal2对slot的灵活管控。
TODO：以后再完善*/
static std::vector<std::unordered_set<c2IAction*>> g_Evt2ActsetVector;

/*返回值为自定义事件类型Chunk的偏移值。每一个应用程序运行前就应该明确的，软件不能所以插拔
进来的event chunk，因为这会导致每个chunk的offect变化，同程序所匹配配合的其他网络配合端
或者序列化（例如录像，undo/redo等）所存数据而产生不兼容。*/
Uint32 c2AppendEvtTypesChunk(Uint32 nNewChunkSize) {
	Uint32 ret = g_Evt2ActsetVector.size();
	g_Evt2ActsetVector.resize(ret + nNewChunkSize);
	return ret;
}

/*============================================================================*/
/*只是先投递操作记录，实际操作在主循环消息处理帧主函数里进行，因为订阅或退订可能会在任
何时候（并不含有暗示改变我们多线程态度）被调用，不能破坏sig里的set及sigs。
TODO:还需要严密测试一下，例如使用订阅了某事件的ACT，里面再订阅或退订，并pub事件触发
ACT里再订阅或退订，等等。*/
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
		const Uint64 esFixFrameStamp) {//FIXME: 用64的es是为了够大，但仍然跑爆问题？
	Event._nFixFrameStamp = esFixFrameStamp;
	g_EventQueue.push(&Event, EventSize);
}

void ProcessEvts() {
	/*处理上一帧实际订阅消息。*/
	for (std::pair<c2IAction*, Uint32>& v : g_ActSubEvtList) {
		BOOST_ASSERT(v.first);
		g_Evt2ActsetVector[v.second].insert(v.first);
	}
	g_ActSubEvtList.clear();
	/*处理上一帧实际退订消息，进行删除。*/
	for (std::pair<c2IAction*, Uint32>& v : g_ActUnsubEvtList) {
		BOOST_ASSERT(v.first);
		g_Evt2ActsetVector[v.second].erase(v.first);
	}
	g_ActUnsubEvtList.clear();
	/*分发消息*/
	static char temp_evtbuf[C2EVTMSG_MAXSIZE];
	while (!g_EventQueue.isEmpty()) {
		g_EventQueue.pop(temp_evtbuf, C2EVTMSG_MAXSIZE);
		c2IEvent &evt = *((c2IEvent*)temp_evtbuf);
		for (c2IAction *tp_act : g_Evt2ActsetVector[evt.getTypeAddChunkOffset()]) {
			//for each (auto tp_act in sig) {
			BOOST_ASSERT(tp_act);
			tp_act->_pEvt = &evt;//传递此event作为参数。主要是BrainTree的update无参数，所以我们也不方便通过update函数传入参数。
			tp_act->update();
			tp_act->_pEvt = nullptr;//处理完后，则event指向重置。
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
