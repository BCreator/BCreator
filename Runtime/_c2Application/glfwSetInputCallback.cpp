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
static const char* get_key_name(int key)
{
	switch (key)
	{
		// Printable keys
	case GLFW_KEY_A:            return "A";
	case GLFW_KEY_B:            return "B";
	case GLFW_KEY_C:            return "C";
	case GLFW_KEY_D:            return "D";
	case GLFW_KEY_E:            return "E";
	case GLFW_KEY_F:            return "F";
	case GLFW_KEY_G:            return "G";
	case GLFW_KEY_H:            return "H";
	case GLFW_KEY_I:            return "I";
	case GLFW_KEY_J:            return "J";
	case GLFW_KEY_K:            return "K";
	case GLFW_KEY_L:            return "L";
	case GLFW_KEY_M:            return "M";
	case GLFW_KEY_N:            return "N";
	case GLFW_KEY_O:            return "O";
	case GLFW_KEY_P:            return "P";
	case GLFW_KEY_Q:            return "Q";
	case GLFW_KEY_R:            return "R";
	case GLFW_KEY_S:            return "S";
	case GLFW_KEY_T:            return "T";
	case GLFW_KEY_U:            return "U";
	case GLFW_KEY_V:            return "V";
	case GLFW_KEY_W:            return "W";
	case GLFW_KEY_X:            return "X";
	case GLFW_KEY_Y:            return "Y";
	case GLFW_KEY_Z:            return "Z";
	case GLFW_KEY_1:            return "1";
	case GLFW_KEY_2:            return "2";
	case GLFW_KEY_3:            return "3";
	case GLFW_KEY_4:            return "4";
	case GLFW_KEY_5:            return "5";
	case GLFW_KEY_6:            return "6";
	case GLFW_KEY_7:            return "7";
	case GLFW_KEY_8:            return "8";
	case GLFW_KEY_9:            return "9";
	case GLFW_KEY_0:            return "0";
	case GLFW_KEY_SPACE:        return "SPACE";
	case GLFW_KEY_MINUS:        return "MINUS";
	case GLFW_KEY_EQUAL:        return "EQUAL";
	case GLFW_KEY_LEFT_BRACKET: return "LEFT BRACKET";
	case GLFW_KEY_RIGHT_BRACKET: return "RIGHT BRACKET";
	case GLFW_KEY_BACKSLASH:    return "BACKSLASH";
	case GLFW_KEY_SEMICOLON:    return "SEMICOLON";
	case GLFW_KEY_APOSTROPHE:   return "APOSTROPHE";
	case GLFW_KEY_GRAVE_ACCENT: return "GRAVE ACCENT";
	case GLFW_KEY_COMMA:        return "COMMA";
	case GLFW_KEY_PERIOD:       return "PERIOD";
	case GLFW_KEY_SLASH:        return "SLASH";
	case GLFW_KEY_WORLD_1:      return "WORLD 1";
	case GLFW_KEY_WORLD_2:      return "WORLD 2";

		// Function keys
	case GLFW_KEY_ESCAPE:       return "ESCAPE";
	case GLFW_KEY_F1:           return "F1";
	case GLFW_KEY_F2:           return "F2";
	case GLFW_KEY_F3:           return "F3";
	case GLFW_KEY_F4:           return "F4";
	case GLFW_KEY_F5:           return "F5";
	case GLFW_KEY_F6:           return "F6";
	case GLFW_KEY_F7:           return "F7";
	case GLFW_KEY_F8:           return "F8";
	case GLFW_KEY_F9:           return "F9";
	case GLFW_KEY_F10:          return "F10";
	case GLFW_KEY_F11:          return "F11";
	case GLFW_KEY_F12:          return "F12";
	case GLFW_KEY_F13:          return "F13";
	case GLFW_KEY_F14:          return "F14";
	case GLFW_KEY_F15:          return "F15";
	case GLFW_KEY_F16:          return "F16";
	case GLFW_KEY_F17:          return "F17";
	case GLFW_KEY_F18:          return "F18";
	case GLFW_KEY_F19:          return "F19";
	case GLFW_KEY_F20:          return "F20";
	case GLFW_KEY_F21:          return "F21";
	case GLFW_KEY_F22:          return "F22";
	case GLFW_KEY_F23:          return "F23";
	case GLFW_KEY_F24:          return "F24";
	case GLFW_KEY_F25:          return "F25";
	case GLFW_KEY_UP:           return "UP";
	case GLFW_KEY_DOWN:         return "DOWN";
	case GLFW_KEY_LEFT:         return "LEFT";
	case GLFW_KEY_RIGHT:        return "RIGHT";
	case GLFW_KEY_LEFT_SHIFT:   return "LEFT SHIFT";
	case GLFW_KEY_RIGHT_SHIFT:  return "RIGHT SHIFT";
	case GLFW_KEY_LEFT_CONTROL: return "LEFT CONTROL";
	case GLFW_KEY_RIGHT_CONTROL: return "RIGHT CONTROL";
	case GLFW_KEY_LEFT_ALT:     return "LEFT ALT";
	case GLFW_KEY_RIGHT_ALT:    return "RIGHT ALT";
	case GLFW_KEY_TAB:          return "TAB";
	case GLFW_KEY_ENTER:        return "ENTER";
	case GLFW_KEY_BACKSPACE:    return "BACKSPACE";
	case GLFW_KEY_INSERT:       return "INSERT";
	case GLFW_KEY_DELETE:       return "DELETE";
	case GLFW_KEY_PAGE_UP:      return "PAGE UP";
	case GLFW_KEY_PAGE_DOWN:    return "PAGE DOWN";
	case GLFW_KEY_HOME:         return "HOME";
	case GLFW_KEY_END:          return "END";
	case GLFW_KEY_KP_0:         return "KEYPAD 0";
	case GLFW_KEY_KP_1:         return "KEYPAD 1";
	case GLFW_KEY_KP_2:         return "KEYPAD 2";
	case GLFW_KEY_KP_3:         return "KEYPAD 3";
	case GLFW_KEY_KP_4:         return "KEYPAD 4";
	case GLFW_KEY_KP_5:         return "KEYPAD 5";
	case GLFW_KEY_KP_6:         return "KEYPAD 6";
	case GLFW_KEY_KP_7:         return "KEYPAD 7";
	case GLFW_KEY_KP_8:         return "KEYPAD 8";
	case GLFW_KEY_KP_9:         return "KEYPAD 9";
	case GLFW_KEY_KP_DIVIDE:    return "KEYPAD DIVIDE";
	case GLFW_KEY_KP_MULTIPLY:  return "KEYPAD MULTPLY";
	case GLFW_KEY_KP_SUBTRACT:  return "KEYPAD SUBTRACT";
	case GLFW_KEY_KP_ADD:       return "KEYPAD ADD";
	case GLFW_KEY_KP_DECIMAL:   return "KEYPAD DECIMAL";
	case GLFW_KEY_KP_EQUAL:     return "KEYPAD EQUAL";
	case GLFW_KEY_KP_ENTER:     return "KEYPAD ENTER";
	case GLFW_KEY_PRINT_SCREEN: return "PRINT SCREEN";
	case GLFW_KEY_NUM_LOCK:     return "NUM LOCK";
	case GLFW_KEY_CAPS_LOCK:    return "CAPS LOCK";
	case GLFW_KEY_SCROLL_LOCK:  return "SCROLL LOCK";
	case GLFW_KEY_PAUSE:        return "PAUSE";
	case GLFW_KEY_LEFT_SUPER:   return "LEFT SUPER";
	case GLFW_KEY_RIGHT_SUPER:  return "RIGHT SUPER";
	case GLFW_KEY_MENU:         return "MENU";

	default:                    return "UNKNOWN";
	}
}

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

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
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
	e._nButton = button;
	e._nAction = action;
	e._nModifier = mods;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void cursor_position_callback(GLFWwindow* window, double x, double y) {
	static c2SysEvt::cursor_moved e(g_nSysETChunkOffset);
	e._x = x;
	e._y = y;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void cursor_enter_callback(GLFWwindow* window, int entered) {
	static c2SysEvt::cursor_enter e(g_nSysETChunkOffset);
	e._bEnter = entered;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void scroll_callback(GLFWwindow* window, double x, double y) {
	static c2SysEvt::scrolled e(g_nSysETChunkOffset);
	e._x = x;
	e._y = y;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	static c2SysEvt::key e(g_nSysETChunkOffset);
	e._nKey = key;
	e._nScancode = scancode;
	e._nAction = action;
	e._nModifier = mods;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void char_callback(GLFWwindow* window, unsigned int codepoint) {
	static c2SysEvt::char_input e(g_nSysETChunkOffset);
	e._nCodePoint = codepoint;
	c2PublishEvt(e, sizeof(e), g_nFixframeStamp);
}

static void char_mods_callback(GLFWwindow* window, unsigned int codepoint, int mods) {
	static c2SysEvt::charmods_input e(g_nSysETChunkOffset);
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
