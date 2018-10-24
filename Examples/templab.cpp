#include<iostream>
#include<conio.h>

////////////////////////////////////////////////////////////////////////////////
static void test_event();
static void test_signalbind();
static void test_glfwevent();
static void test_signal2_bt();
static void test_memqueue();

//以后可以规整为Unittest更标准的方式。
static int main() {
//	test_event();
//	test_memqueue();
//	test_signalbind();
//	test_signal2_bt();
	test_glfwevent();

	std::cout << "Press any key to quit." << std::endl;
	_getch();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
/*test_gleqevent
*/
#define GLEQ_IMPLEMENTATION
#include"../Runtime/_c2Application/gleq.h"
#include"../Runtime/c2Application.h"
c2gleqevts::c2GLEQevent st_c2_gleqevt(0);
struct c2GLEQAction : public c2IAction {
	virtual Status doItNow(const c2IEvent &Evt, size_t EvtSize) {
		std::cout << typeid(*this).name() << "::doItNow | ......" << std::endl;
		const GLEQevent &event = static_cast<const c2gleqevts::c2GLEQevent&>(Evt)._GLEQevent;
		BOOST_ASSERT(EvtSize == sizeof(static_cast<const c2gleqevts::c2GLEQevent&>(Evt)));
		switch (event.type) {
		case GLEQ_WINDOW_MOVED:
			printf("Window moved to %i,%i\n", event.pos.x, event.pos.y);
			break;
		case GLEQ_BUTTON_PRESSED:
			printf("Mouse button %i pressed (mods 0x%x)\n",
				event.mouse.button,
				event.mouse.mods);
			break;
		case GLEQ_WINDOW_RESIZED:
			printf("Window resized to %ix%i\n", event.size.width,
				event.size.height);
			break;
		case GLEQ_KEY_PRESSED:
			printf("Key 0x%02x pressed (scancode 0x%x mods 0x%x)\n",
				event.keyboard.key,
				event.keyboard.scancode,
				event.keyboard.mods);
			break;
		case GLEQ_KEY_REPEATED:
			printf("Key 0x%02x repeated (scancode 0x%x mods 0x%x)\n",
				event.keyboard.key,
				event.keyboard.scancode,
				event.keyboard.mods);
			break;
		case GLEQ_KEY_RELEASED:
			printf("Key 0x%02x released (scancode 0x%x mods 0x%x)\n",
				event.keyboard.key,
				event.keyboard.scancode,
				event.keyboard.mods);
			if (0x1 == event.keyboard.scancode)
				c2UnsubEvt(st_c2_gleqevt, *this);
			break;
		default:
			printf("Error: Unknown event %i\n", event.type);
			break;
		}
		return Status::Success;
	}
};
//static void _init_gleqevent() {
//	Uint32 etc_offset = c2AppendEvtTypesChunk(C2ET1::EVENTTYPE_AMMOUT + 1);
//	Uint32 etc2_offset = c2AppendEvtTypesChunk(C2ET2::EVENTTYPE_AMMOUT + 1);
////	BOOST_STATIC_ASSERT(0 == GLEQ_NONE);
////#if GLFW_VERSION_MINOR >= 3
////	Uint32 etc_offset_gleq = GLEQ_WINDOW_SCALE_CHANGED - GLEQ_NONE;
////#elif GLFW_VERSION_MINOR >= 2
////	Uint32 etc_offset_gleq = GLEQ_JOYSTICK_DISCONNECTED - GLEQ_NONE;
////#elif GLFW_VERSION_MINOR >= 1
////	Uint32 etc_offset_gleq = GLEQ_FILE_DROPPED - GLEQ_NONE;
////#else
////	Uint32 etc_offset_gleq = GLEQ_MONITOR_DISCONNECTED - GLEQ_NONE;
////#endif
////	etc_offset_gleq = c2AppendEvtTypesChunk(etc_offset_gleq);
//}
static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}
static void test_glfwevent() {
	std::cout << "test_glfwevent begin......" << std::endl;
	Uint32 etc_offset_gleq;
	etc_offset_gleq = c2AppendEvtTypesChunk(c2gleqet::EVENTTYPE_AMMOUT + 1);
//	c2gleqevts::c2GLEQevent st_c2_gleqevt(etc_offset_gleq);
	c2GLEQAction gleq_action;
	c2SubEvt(st_c2_gleqevt, gleq_action);
	/*glfw begin*/
	glfwSetErrorCallback(error_callback);
	/* Initialize the library */
	if (!glfwInit())
		return;
	gleqInit();
	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow *window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	gleqTrackWindow(window);
	/* Loop until the user closes the window */
#if 0
	GLEQevent event;
#endif
	Uint64 es_logicalframe_stamp = 0;
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		c2PumpEvent();
		//从GLEQ拿消息
		while (gleqNextEvent(&(st_c2_gleqevt._GLEQevent))) {
			c2PublishEvt(st_c2_gleqevt, sizeof(st_c2_gleqevt), es_logicalframe_stamp);
		}
		gleqFreeEvent(&(st_c2_gleqevt._GLEQevent));
		/*逻辑、渲染等主循环帧*/
		c2UpdateLogicFrame(es_logicalframe_stamp);
		++es_logicalframe_stamp;
#if 0
		/* Poll for and process events */
		glfwPollEvents();
		while (gleqNextEvent(&event)) {
			switch (event.type) {
			case GLEQ_WINDOW_MOVED:
				printf("Window moved to %i,%i\n", event.pos.x, event.pos.y);
				break;
			case GLEQ_WINDOW_RESIZED:
				printf("Window resized to %ix%i\n", event.size.width,
					event.size.height);
				break;
			default:
				printf("Error: Unknown event %i\n", event.type);
				break;
			}
			gleqFreeEvent(&event);
		}
#endif
	}
	glfwTerminate();
	std::cout << "......test_glfwevent end" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
/*test_event
*/
static void test_event() {
	std::cout << "test_event begin......" << std::endl;
	//initialize 
	Uint32 etc_offset = c2AppendEvtTypesChunk(C2ET1::EVENTTYPE_AMMOUT + 1);
	Uint32 etc2_offset = c2AppendEvtTypesChunk(C2ET2::EVENTTYPE_AMMOUT + 1);
	//Subscribe event
	C2EVT1::EventTest event(etc_offset);
	C2EVT2::Mouse event2(etc2_offset);
	event._s[0] = 'w';
	event._s[1] = 'x';
	event._s[2] = 'x';
	event._s[3] = 'x';
	event._s[4] = 'n';
	event._s[5] = 'i';
	event._s[6] = '\0';
	c2IAction action;
	c2Action2 action2;
	c2SubEvt(event, action);
	c2SubEvt(event2, action);
	c2SubEvt(event2, action2);
	//	c2SubEvt(event, c2Action2::doItNow);
		//Publish event
	c2PublishEvt(event, sizeof(event), 1840);
	c2PublishEvt(event2, sizeof(event2), 1841);
	c2UpdateLogicFrame(1);
	//Unsubscribe event
//	c2UnsubEvt(event, action);
	//# TODO next:
//- 1 如果确定event type，以及设计event种类。
//- 3 设计action的种类，接入BT等多种形式。
//- 4 
	std::cout << "......test_event end" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
/*test command
连续种类的command，在一定时间内输入do stack来实现undo redo。如果影起了其他的状态的
话，状态还应该能回退——如果全部都是无状态设计，做到这一点容易。这里实质讨论就是transaction
的问题了。
*/
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
using EventHandler			= boost::signals2::signal<testCommand::Status()>;
static std::vector<EventHandler>	EventHandlerVec;
using EventType = std::vector<EventHandler>::size_type;
static void test_signalbind() {
	std::cout << "test_signalbind begin......" << std::endl;
	EventHandlerVec.push_back(EventHandler());
	EventType evttype = EventHandlerVec.size()-1;
	BOOST_ASSERT(evttype>=0);
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
#include<boost/signals2/signal.hpp>
#include"../Runtime/_c2Application/BrainTree.h"//不合规矩
class btCommand: public BrainTree::Node{
	Status update() override {
		std::cout << "C2 Command" << std::endl;
		return Node::Status::Success;
	}
};
static void test_signal2_bt() {
	std::cout << "test_signal2_bt begin......" << std::endl;
	boost::signals2::signal<BrainTree::Node::Status()>	event;
	/*------------------------------------------------------------------------*/
	BrainTree::BehaviorTree tree;
	auto repeater = std::make_shared<BrainTree::Repeater>(5);
	repeater->setChild(std::make_shared<btCommand>());
	tree.setRoot(repeater);
	/*------------------------------------------------------------------------*/
	//Subscribe event
	event.connect(boost::bind(&BrainTree::BehaviorTree::update, tree));
	/*------------------------------------------------------------------------*/
	//Fire Event
	for (int i = 0; i < 5; i++)
		event();
	std::cout << "......test_signal2_bt end" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
#include<boost/log/trivial.hpp>
#include"../Runtime/_c2Application/tsMemQueue.h"
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
