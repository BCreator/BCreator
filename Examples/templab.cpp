#include<conio.h>
#include<iostream>

////////////////////////////////////////////////////////////////////////////////
static void test_activity();
static void test_command();
static void test_glfw();
static void test_signal2_bt();
static int main() {

	test_activity();
//	test_command();
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
static void test_activity() {
	//Subscribe event
	c2Command ac;
	c2EventType et = c2SubscribeEvent(ac);
	//Publish event
	c2Event event;
	event._nType = et;
	c2PublishEvent(event);
	c2UpdateLogicFrame(1);
	c2UnsubscribeEvent(et, ac);
# TODO next：
- 1 如果确定event type，以及设计event种类。
- 2 订阅者，而不应该只是command吧？
- 3 设计command的种类，接入BT等多种形式。
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
static void test_command() {
	std::cout << "test_command begin......" << std::endl;
	EventHandlerVec.push_back(EventHandler());
	EventType et = EventHandlerVec.size()-1;
	BOOST_ASSERT(et>=0);
	testCommand ac;
	EventHandlerVec[et].connect(boost::bind(&testCommand::act, ac));
	EventHandlerVec[et].connect(boost::bind(&testCommand::act, ac));
	EventHandlerVec[et].connect(boost::bind(&testCommand::act, ac));
	EventHandlerVec[et]();
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
	std::cout << "......test_command end" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
#include<GLFW/glfw3.h>
#define GLEQ_IMPLEMENTATION
#include"../Runtime/_c2Activity/gleq.h"//不合规矩
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
#include"../Runtime/_c2Activity/BrainTree.h"//不合规矩
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
