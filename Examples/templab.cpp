#include<conio.h>
#include<iostream>

////////////////////////////////////////////////////////////////////////////////
static void test_signal2_bt();
static void test_activity();
static int main() {

	test_signal2_bt();
	test_activity();

	_getch();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
#include<GLFW/glfw3.h>
#define GLEQ_IMPLEMENTATION
#include"../Runtime/_c2Activity/gleq.h"//不合规矩
static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}
static void test_activity() {
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
				printf("Window resized to %ix%i\n", event.size.width, event.size.height);
				break;
			default:
				printf("Error: Unknown event %i\n", event.type);
				break;
			}
			gleqFreeEvent(&event);
		}
	}
	glfwTerminate();
}



////////////////////////////////////////////////////////////////////////////////
#include<boost/signals2/signal.hpp>
#include"../Runtime/_c2Activity/BrainTree.h"//不合规矩
class c2Command: public BrainTree::Node{
	Status update() override {
		std::cout << "C2 Command" << std::endl;
		return Node::Status::Success;
	}
};
static void test_signal2_bt() {
	boost::signals2::signal<BrainTree::Node::Status()>	event;
	
	/*------------------------------------------------------------------------*/
	BrainTree::BehaviorTree tree;
	auto repeater = std::make_shared<BrainTree::Repeater>(5);
	repeater->setChild(std::make_shared<c2Command>());
	tree.setRoot(repeater);

	/*------------------------------------------------------------------------*/
	//Subscribe event
	event.connect(boost::bind(&BrainTree::BehaviorTree::update, tree));

	/*------------------------------------------------------------------------*/
	//Fire Event
	for (int i = 0; i < 5; i++)
		event();
}
