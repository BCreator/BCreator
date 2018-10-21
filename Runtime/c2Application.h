#ifndef C2_APPLICATION_H_
#define C2_APPLICATION_H_

#include<boost/signals2/signal.hpp>
#include"./Metas/PreDefined.h"

////////////////////////////////////////////////////////////////////////////////
/*
Part
*/
#include"Metas/Part.h"
using c2APart = c2::Part::ARPart;
C2Interface c2APart c2CreatePart(const char *sClass,
	const char *sName = nullptr);
C2Interface bool _c2RegistPartClass(const char *sClass, c2::Part::CreationFunc C);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create);

////////////////////////////////////////////////////////////////////////////////
/*
Event system
*/
using c2EventType = int;//FIXME
struct c2IEvent {//TODO��GLFWȱ���ƶ��豸�ϵ�һЩINPUT��Ϣ��������Ļ��ת��������
	c2EventType	_nType;
	int			_nLFStamp;	//Logic Frame Stamp.
};

/*============================================================================*/
struct c2IAction {
	enum class Status
	{
		Invalid,
		Success,
		Failure,
		Running,
	};
	int		_Predicate;
	int		_SubjectID;
	//blackboard;ֻ�����ڲ�״̬�����ܼ�¼�κ�����״̬��
	explicit c2IAction();
	//TODO��������ֵ��RUNNING��Ϊ�˺����OneRounte��
	Status doItNow(const c2EventType &EventType);
};

/******************************************************************************/
//Consumer subscribe event And Producer publish event��ͨ�ö��ĺ�������û�б�¶
//����ʵ����Ҫ������ֻ�Ǳ�¶����Part�ġ�
using c2EvtSigConsumer	= boost::signals2::signal<
							c2IAction::Status(const c2EventType &EventType) >;
using c2EvtConsumer		= c2EvtSigConsumer::slot_type;
C2Interface c2EventType& c2SubEvt(const c2EvtConsumer &Consumer,
									const c2EventType &EventType);
C2Interface void c2UnsubEvt(const c2EvtConsumer &Consumer,
									const c2EventType &EventType);
C2Interface void c2PubEvt(const c2IEvent &Event);

////////////////////////////////////////////////////////////////////////////////
/*
Driving framework of the whole application
*/
C2Interface void c2WaitEvent(c2IEvent *pEvent);
C2Interface void c2PumpEvent(c2IEvent *pEvent);
C2Interface void c2UpdateLogicFrame(int nLogicFrameStamp);

//FIXME: ���º���ʱ����ע���ڴˣ��Ժ���ø��ִ����ķ�ʽ�����ⷽ����¡���������������
//ֻ��Ϊ�˱��ں����Դ˺�Ϊ����������޸���صĴ��롣
//#define C2_CHECK_MEM

////////////////////////////////////////////////////////////////////////////////
#endif//C2_APPLICATION_H_
