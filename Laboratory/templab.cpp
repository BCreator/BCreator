#include<iostream>
#include<conio.h>

////////////////////////////////////////////////////////////////////////////////
static void test_application();
static void test_signalbind();
static void test_memqueue();

int main_templab() {
	test_application();
//	test_memqueue();
//	test_signalbind();

	std::cout << "Press any key to quit." << std::endl;
	_getch();
	return 0;
}

#include"../Runtime/c2PreDefined.h"
#include"../Runtime/c2DefEvent.h"
#include"../Runtime/c2Application.h"
class onSysInitialized : public c2IAction {
	class btAct : public BrainTree::Node {
		Status update() override {
			std::cout << "  -> BT PRINTed." << std::endl;
			return Node::Status::Success;
		}
	};
	BrainTree::BehaviorTree _BTree;
public:
	onSysInitialized() {
		auto repeater = std::make_shared<BrainTree::Repeater>(5);
		repeater->setChild(std::make_shared<btAct>());
		_BTree.setRoot(repeater);
	}
	virtual Status update() {
		std::cout << "I can plugin my extensions here." << std::endl;
		_BTree.update();
		return c2IAction::update();
	}
};
static void test_application() {
	std::cout << "test_application begin......" << std::endl;
	onSysInitialized osi;
	c2asActSubEvt(osi, 0+c2SysEvtType::initialized, sizeof(c2SysEvt::initialized));
	c2AppRun(1, 640, 480, "Temp Lab", false);
	std::cout << "......test_application end" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
/*test_signalbind
���������command����һ��ʱ��������do stack��ʵ��undo redo�����Ӱ����������״̬��
����״̬��Ӧ���ܻ��ˡ������ȫ��������״̬��ƣ�������һ�����ס�����ʵ�����۾���transaction
�������ˡ�*/
struct testCommand {
	enum class Status
	{
		Invalid,
		Success,
		Failure,
		Running,
	};
	int		_Predicate;
	int		_SubjectID;
	explicit testCommand() : _SubjectID(0) {
	}
	Status act() {
		std::cout << "act...." << std::endl;
		return Status::Failure;
	}
};
#include<vector>
#include<queue>
#include<boost/assert.hpp>
#include<boost/signals2/signal.hpp>
using EventHandler = boost::signals2::signal<testCommand::Status()>;
static std::vector<EventHandler>	EventHandlerVec;
using EventType = std::vector<EventHandler>::size_type;
static void test_signalbind() {
	std::cout << "test_signalbind begin......" << std::endl;
	EventHandlerVec.push_back(EventHandler());
	EventType evttype = EventHandlerVec.size() - 1;
	BOOST_ASSERT(evttype >= 0);
	testCommand act;
	EventHandlerVec[evttype].connect(boost::bind(&testCommand::act, act));
	EventHandlerVec[evttype].connect(boost::bind(&testCommand::act, act));
	EventHandlerVec[evttype].connect(boost::bind(&testCommand::act, act));
	EventHandlerVec[evttype]();
#if 0
	std::cout << ">>> learn queue >>>" << std::endl;
	std::queue<int> mq;
	mq.push(1);
	std::cout << mq.front() << std::endl;
	std::cout << mq.front() << std::endl;
	std::cout << mq.front() << std::endl;
	mq.pop();
	//	std::cout << mq.front() << std::endl;
#endif
	std::cout << "......test_signalbind end" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
#include<boost/log/trivial.hpp>
#include<_c2Application/tsMemQueue.h>
#pragma pack(push, 1)
struct aa {
	size_t	_sizeSize;
	char	_nType;
	int		_nLFStamp;	//Logic Frame Stamp.
	virtual void print() {
		std::cout << "aa print!!!= " << _nLFStamp << std::endl;
	}
	aa() {
		_sizeSize = sizeof(aa);
	}
	virtual void print2(int arg1, int arg2) {
	}
};
struct bb : public aa {
	int bbbb;
	bb() {
		_sizeSize = sizeof(bb);
	}
	virtual void print() override {
		std::cout << "bb print!!!= " << _nLFStamp << std::endl;
	}
	//virtual void print2(int arg1) {
	//}
	//virtual void print2(int arg1, int arg2, int arg3) {
	//}
};
struct cc {
	char	_nType;
	int		_nLFStamp;	//Logic Frame Stamp.
	int cccc;
};
#pragma pack(pop)
static void _test_ref(aa &the) {
	std::cout << "test ref. _sizeSize= " << the._sizeSize << std::endl;
}
static void test_memqueue() {
	std::cout << "test_memqueue begin......" << std::endl;
	aa thea;
	bb theb;
	_test_ref(thea);
	std::cout << sizeof(aa) << typeid(aa).name() << std::endl;
	std::cout << sizeof(bb) << typeid(bb).name() << std::endl;
	std::cout << sizeof(cc) << typeid(cc).name() << std::endl;
	/*------------------------------------------------------------------------*/
	c2::tsMemQueue mq(1024);
	theb._nLFStamp = 12345;
	theb.bbbb = 54321;
	mq.push(&theb, theb._sizeSize);
	bb *pnewb = (bb*)malloc(theb._sizeSize);
//	bb *pnewb = (bb*)new char(theb._sizeSize);
	mq.pop(pnewb, theb._sizeSize);
	pnewb->print();
	((aa*)pnewb)->print();
	aa &newb = *pnewb;
	newb.print();
	std::cout << "pnewb's typed= " << typeid(*(aa*)pnewb).name() << std::endl;
	std::cout << "newb's typed= " << typeid(newb).name() << std::endl;
	pnewb->print2(1, 1);

	/*------------------------------------------------------------------------*/
	std::cout << "......test_memqueue end" << std::endl;
}
