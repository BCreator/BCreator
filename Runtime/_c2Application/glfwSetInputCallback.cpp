#include <ctype.h>
#include<vector>
#include<queue>
#include<iostream>
#include<boost/assert.hpp>
#include<boost/log/trivial.hpp>
#include"../c2PreDefined.h"
#include"../c2DefEvent.h"
#include"../c2Application.h"
#include<GLFW/glfw3.h>

extern Uint32 g_nSysETChunkOffset;//必须确保system event type chunk是第一个append的
extern Uint64 g_nFixframeStamp;

////////////////////////////////////////////////////////////////////////////////
static unsigned int counter = 0;
static const char* get_action_name(int action)
{
	switch (action)
	{
	case GLFW_PRESS:
		return "pressed";
	case GLFW_RELEASE:
		return "released";
	case GLFW_REPEAT:
		return "repeated";
	}

	return "caused unknown action";
}

static const char* get_button_name(int button)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		return "left";
	case GLFW_MOUSE_BUTTON_RIGHT:
		return "right";
	case GLFW_MOUSE_BUTTON_MIDDLE:
		return "middle";
	default:
	{
		static char name[16];
		sprintf(name, "%i", button);
		return name;
	}
	}
}

static const char* get_mods_name(int mods)
{
	static char name[512];

	if (mods == 0)
		return " no mods";

	name[0] = '\0';

	if (mods & GLFW_MOD_SHIFT)
		strcat(name, " shift");
	if (mods & GLFW_MOD_CONTROL)
		strcat(name, " control");
	if (mods & GLFW_MOD_ALT)
		strcat(name, " alt");
	if (mods & GLFW_MOD_SUPER)
		strcat(name, " super");

	return name;
}

static const char* get_character_string(int codepoint)
{
	// This assumes UTF-8, which is stupid
	static char result[6 + 1];

	int length = wctomb(result, codepoint);
	if (length == -1)
		length = 0;

	result[length] = '\0';
	return result;
}

static void window_pos_callback(GLFWwindow* window, int x, int y)
{
	
	printf("%08x at %0.3f: Window position: %i %i\n",
		counter++, glfwGetTime(), x, y);
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
	
	printf("%08x at %0.3f: Window size: %i %i\n",
		counter++, glfwGetTime(), width, height);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	
	printf("%08x at %0.3f: Framebuffer size: %i %i\n",
		counter++, glfwGetTime(), width, height);

	glViewport(0, 0, width, height);
}

static void window_close_callback(GLFWwindow* window)
{
	
	printf("%08x at %0.3f: Window close\n",
		counter++, glfwGetTime());

//	glfwSetWindowShouldClose(window, slot->closeable);
}

static void window_refresh_callback(GLFWwindow* window)
{
	
	printf("%08x at %0.3f: Window refresh\n",
		counter++, glfwGetTime());

	glfwMakeContextCurrent(window);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
}

static void window_focus_callback(GLFWwindow* window, int focused)
{
	
	printf("%08x at %0.3f: Window %s\n",
		counter++, glfwGetTime(),
		focused ? "focused" : "defocused");
}

static void window_iconify_callback(GLFWwindow* window, int iconified)
{
	printf("%08x at %0.3f: Window was %s\n",
		counter++, glfwGetTime(),
		iconified ? "iconified" : "restored");
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	static c2SysEvt::mouse_button e(g_nSysETChunkOffset);
	e._pWnd = window;
	e._nButton = button;
	e._nAction = action;
	e._nModifier = mods;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void cursor_position_callback(GLFWwindow* window, double x, double y) {
	static c2SysEvt::cursor_moved e(g_nSysETChunkOffset);
	e._pWnd = window;
	e._x = x;
	e._y = y;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void cursor_enter_callback(GLFWwindow* window, int entered) {
	static c2SysEvt::cursor_enter e(g_nSysETChunkOffset);
	e._pWnd = window;
	e._bEnter = entered;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void scroll_callback(GLFWwindow* window, double x, double y) {
	static c2SysEvt::scrolled e(g_nSysETChunkOffset);
	e._pWnd = window;
	e._x = x;
	e._y = y;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	static c2SysEvt::key e(g_nSysETChunkOffset);
	e._pWnd = window;
	e._nKey = key;
	e._nScancode = scancode;
	e._nAction = action;
	e._nModifier = mods;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void char_callback(GLFWwindow* window, unsigned int codepoint) {
	static c2SysEvt::char_input e(g_nSysETChunkOffset);
	e._pWnd = window;
	e._nCodePoint = codepoint;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void char_mods_callback(GLFWwindow* window, unsigned int codepoint, int mods) {
	static c2SysEvt::charmods_input e(g_nSysETChunkOffset);
	e._pWnd = window;
	e._nCodePoint = codepoint;
	e._nModifier = mods;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	int i;
	

	printf("%08x at %0.3f: Drop input\n",
		counter++, glfwGetTime());

	for (i = 0; i < count; i++)
		printf("  %i: \"%s\"\n", i, paths[i]);
}

static void monitor_callback(GLFWmonitor* monitor, int event)
{
	if (event == GLFW_CONNECTED)
	{
		int x, y, widthMM, heightMM;
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwGetMonitorPos(monitor, &x, &y);
		glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);

		printf("%08x at %0.3f: Monitor %s (%ix%i at %ix%i, %ix%i mm) was connected\n",
			counter++,
			glfwGetTime(),
			glfwGetMonitorName(monitor),
			mode->width, mode->height,
			x, y,
			widthMM, heightMM);
	}
	else if (event == GLFW_DISCONNECTED)
	{
		printf("%08x at %0.3f: Monitor %s was disconnected\n",
			counter++,
			glfwGetTime(),
			glfwGetMonitorName(monitor));
	}
}

static void joystick_callback(int joy, int event)
{
	if (event == GLFW_CONNECTED)
	{
		int axisCount, buttonCount;

		glfwGetJoystickAxes(joy, &axisCount);
		glfwGetJoystickButtons(joy, &buttonCount);

		printf("%08x at %0.3f: Joystick %i (%s) was connected with %i axes and %i buttons\n",
			counter++, glfwGetTime(),
			joy,
			glfwGetJoystickName(joy),
			axisCount,
			buttonCount);
	}
	else
	{
		printf("%08x at %0.3f: Joystick %i was disconnected\n",
			counter++, glfwGetTime(), joy);
	}
}

////////////////////////////////////////////////////////////////////////////////
/*必须在imgui设置之前，ImGui_ImplGlfw_Init内部会帮忙保存previousCallback并执行的。
TODO: 我们是否也有必要沿袭此传统给上层用户？*/
void glfwSetInputCallback(GLFWwindow* window) {
	glfwSetWindowPosCallback(window, window_pos_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowCloseCallback(window, window_close_callback);
	glfwSetWindowRefreshCallback(window, window_refresh_callback);
	glfwSetWindowFocusCallback(window, window_focus_callback);
	glfwSetWindowIconifyCallback(window, window_iconify_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, char_callback);
	glfwSetCharModsCallback(window, char_mods_callback);
	glfwSetDropCallback(window, drop_callback);
}
