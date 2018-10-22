#include<conio.h>
#include<iostream>

////////////////////////////////////////////////////////////////////////////////
static void test_event();
static void test_signalbind();
static void test_glfw();
static void test_signal2_bt();
static void test_memqueue();

static int main() {
	test_event();
	test_memqueue();
//	test_signalbind();
//	test_signal2_bt();
//	test_glfw();

	std::cout << "Press any key to quit." << std::endl;
	_getch();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
/*test_activity
*/
#include"../Runtime/c2Application.h"
static void test_event() {
	std::cout << "test_activity begin......" << std::endl;
	//Subscribe event
	c2EventTest event;
//	event._esType = _TEST_C2EVT;
	event._s[0] = 'w';
	event._s[1] = 'x';
	event._s[2] = 'x';
	event._s[3] = 'x';
	event._s[4] = 'n';
	event._s[5] = 'i';
	event._s[6] = '\0';
	c2IAction action;
	c2SubEvt(event, action);
	//Publish event
	c2PubEvt(event, sizeof(event), 1840);
	c2UpdateLogicFrame(1);
	//Unsubscribe event
	c2UnsubEvt(event, action);
	//# TODO next：
//- 1 如果确定event type，以及设计event种类。
//- 3 设计action的种类，接入BT等多种形式。
//- 4 
	std::cout << "......test_activity end" << std::endl;
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
#include<GLFW/glfw3.h>
#define GLEQ_IMPLEMENTATION
#include"../Runtime/_c2Application/gleq.h"//不合规矩
static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}
static void test_glfw() {
	std::cout << "test_glfw begin......" << std::endl;
	glfwSetErrorCallback(error_callback);
	/* Initialize the library */
	if (!glfwInit())
		return;
	gleqInit();
	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window= glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	gleqTrackWindow(window);
	/* Loop until the user closes the window */
	GLEQevent event;
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
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
	}
	glfwTerminate();
	std::cout << "......test_glfw end" << std::endl;
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
	int			_nLFStamp;	//Logic Frame Stamp.
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
	bb *newb = (bb*)malloc(theb._sizeSize);
//	bb *newb = (bb*)new char(theb._sizeSize);
	mq.pop(newb, theb._sizeSize);
	newb->print();
	((aa*)newb)->print();
	std::cout << "newb's typed= " << typeid(*(aa*)newb).name() << std::endl;
	newb->print2(1, 1);

	/*------------------------------------------------------------------------*/
	std::cout << "......test_memqueue end" << std::endl;
}
