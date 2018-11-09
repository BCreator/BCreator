#pragma once

#include<boost/signals2/signal.hpp>
#include"./ThirdParty/imgui/imgui.h"

//FIXME: ���º���ʱ����ע���ڴˣ��Ժ���ø��ִ����ķ�ʽ�����ⷽ����¡���������������
//ֻ��Ϊ�˱��ں����Դ˺�Ϊ����������޸���صĴ��롣
//#define C2_CHECK_MEM

////////////////////////////////////////////////////////////////////////////////
/*Action��ϵ*/
#include"./_c2Application/BrainTree.h"
struct c2IAction : public BrainTree::BehaviorTree {
	////TODO��������ֵ��RUNNING��Ϊ�˺����OneRounte��
	//enum class Status
	//{
	//	Invalid,
	//	Success,
	//	Failure,
	//	Running,
	//};
	////int		_Predicate;
	////int		_SubjectID;
	////blackboard;ֻ�����ڲ�״̬�����ܼ�¼�κ�����״̬��
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
 ע��ͨ��ϵͳ�¼������û��Զ�������Ǽٻص������ǵ��¼���ϵ�ǻ����¼����У�ʵ���û�ֻ��
 �첽����Ļ��ᣬ�⵼���û����ù��ܲ���������������������ⲻ���û�̫��ѡ��*/
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
