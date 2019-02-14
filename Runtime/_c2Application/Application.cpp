#include<utility>
#include<vector>
#include<queue>
#include<iostream>
#include<boost/assert.hpp>
#include<boost/log/trivial.hpp>
#include"../c2PreDefined.h"
#include"../c2DefEvent.h"
#include"../c2Application.h"
#include"./tsMemQueue.h"
#include<GLFW/glfw3.h>
#include<imgui/imgui.h>
#include"../ThirdParty/imgui/examples/imgui_impl_glfw.h"
#include"../ThirdParty/imgui/examples/imgui_impl_opengl3.h"

////////////////////////////////////////////////////////////////////////////////
Uint32 g_nSysETChunkOffset = 0;//必须确保system event type chunk是第一个append的
Uint64 g_nFixframeStamp = 0;

////////////////////////////////////////////////////////////////////////////////
// Application framework
static c2FrameFun g_DrawFun = nullptr;
static c2FrameFun g_UpdateFixFrameFun = nullptr;
C2API void c2SetDrawCallback(c2FrameFun Fun) {
	g_DrawFun = Fun;
}
C2API void c2SetUpdateFixFrameCallback(c2FrameFun Fun) {
	g_UpdateFixFrameFun = Fun;
}

// -TODO：GLFW缺少移动设备上的一些INPUT消息，例如屏幕翻转、重力等
static void glfwErrorCallback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}
void glfwSetInputCallback(GLFWwindow* window);
void ProcessEvts();
C2API void c2AppRun(int SwapInterval, int nWndWidth, int nWndHeight,
						const char *sWndCaption, bool isBlocked) {
	std::atexit(glfwTerminate);
	/*增加系统事件chunk，必须确保system event type chunk是第一个append的*/
	g_nSysETChunkOffset = c2AppendEvtTypesChunk(c2SysEvtType::AMOUNT + 1);
	/*------------------------------------------------------------------------*/
	/*glfw begin*/
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
		return;
	/*------------------------------------------------------------------------*/
	// Decide GL+GLSL versions
#if defined(C2_USE_OPENGLES)
	const char* glsl_version = "#version 300 es";
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#elif defined(__APPLE__)//TODO: not be tested ever.
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.3+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
	// Initialize OpenGL loader
#else
	// 我修改为GL 3.3 + GLSL 330 core，与imgui内部默认的GL 3.0 + GLSL 130不同
	const char* glsl_version = "#version 330 core";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow *window = glfwCreateWindow(nWndWidth, nWndHeight, sWndCaption, NULL, NULL);
	if (!window) {
		glfwTerminate();
		return;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(SwapInterval);
	// Initialize OpenGL loader
#ifdef C2_USE_OPENGLES
	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
		BOOST_LOG_TRIVIAL(fatal) << "Failed to initialize OpenGL loader!";
		return;
	}
#else

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
#endif
	if (err)
	{
		BOOST_LOG_TRIVIAL(fatal) << "Failed to initialize OpenGL loader!";
		return;
	}

#endif

	/*------------------------------------------------------------------------*/
	/*input event callback*/
	glfwSetInputCallback(window);//必须在ImGui_ImplOpenGL3_Init之前，原因见函数说明。
	/*------------------------------------------------------------------------*/
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfw_InitForOpenGL(window, true);//设置为true，imgui这个opengl实现example内部会保持我们的callback
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	/*------------------------------------------------------------------------*/
	/*抛出初始化完成事件，上层应用如果有需要可订阅*/
	c2SysEvt::initialized sysevt_initialized(g_nSysETChunkOffset);
	sysevt_initialized._pWnd = window;
	c2PublishEvt(sysevt_initialized, sizeof(sysevt_initialized),
					g_nFixframeStamp);
	/*------------------------------------------------------------------------*/
	/* Loop until the user closes the window */
	void(*syseventscatch)() = isBlocked ? glfwWaitEvents : glfwPollEvents;
	while (!glfwWindowShouldClose(window)) {
		syseventscatch();
		/*--------------------------------------------------------------------*/
		/*Imgui*/
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		/*--------------------------------------------------------------------*/
		/*逻辑、渲染等主循环帧*/
		//int elapsed = 0;	//TODO
		/*处理消息订阅和分发消息*/
		ProcessEvts();
		/*--------------------------------------------------------------------*/
		/*FIXME: elapsed计算*/
		static double t_tick = 0, t_pretick = 0, elapsed = 0;
		t_tick = glfwGetTime();
		elapsed = t_tick - t_pretick;
		t_pretick = t_tick;
		/*--------------------------------------------------------------------*/
		if (g_DrawFun) {
			g_DrawFun(window, elapsed, g_nFixframeStamp);
		}
		/*FIXME: fixframe步进。眼下是假的，正式应改为每秒的恒定帧率*/
		static double elapsedfix = 0.0f;
		elapsedfix += elapsed;
		if (elapsedfix > 0.1) {//10 frames / second 
			if (g_UpdateFixFrameFun) {
				g_UpdateFixFrameFun(window, elapsedfix, g_nFixframeStamp);
			}
			elapsedfix = 0.0f;
			++g_nFixframeStamp;
		}
		/*--------------------------------------------------------------------*/
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		/*--------------------------------------------------------------------*/
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
	}
	/*FIXME: 写在这里是无效的。*/
	c2SysEvt::terminate sysevt_terminate(g_nSysETChunkOffset);
	c2PublishEvt(sysevt_terminate, sizeof(sysevt_terminate), g_nFixframeStamp);
 }//c2AppRun

////////////////////////////////////////////////////////////////////////////////
/*
Part & Factory
*/
#include"../c2Foundation/c2Part.h"
c2Part::CreationDict		c2Part::_CreationDict;	//FIXME: 暂时放这

C2API c2APart c2CreatePart(const char *sClass, const char *sName) {
	if (!sClass)
		return nullptr;
	c2Part::CreationDict::iterator ci = c2Part::_CreationDict.find(sClass);
	if (ci == c2Part::_CreationDict.end())
		return nullptr;
	c2Part::CreationFunc	create = ci->second;
	BOOST_ASSERT(create);
	return create();
}

C2API bool _c2RegistPartClass(const char *sClass, c2Part::CreationFunc C) {
	if (!sClass || !C)
		return false;
	return c2Part::_CreationDict.insert(	//如果已存在同样类名注册，则返回false。
		c2Part::CreationDict::value_type(sClass, C)).second;
}
