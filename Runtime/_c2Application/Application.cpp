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
Uint32 g_nSysETChunkOffset = 0;//����ȷ��system event type chunk�ǵ�һ��append��
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

// -TODO��GLFWȱ���ƶ��豸�ϵ�һЩINPUT��Ϣ��������Ļ��ת��������
static void glfwErrorCallback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}
void glfwSetInputCallback(GLFWwindow* window);
void ProcessEvts();
C2API void c2AppRun(int SwapInterval, int nWndWidth, int nWndHeight,
						const char *sWndCaption, bool isBlocked) {
	std::atexit(glfwTerminate);
	/*����ϵͳ�¼�chunk������ȷ��system event type chunk�ǵ�һ��append��*/
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
	// ���޸�ΪGL 3.3 + GLSL 330 core����imgui�ڲ�Ĭ�ϵ�GL 3.0 + GLSL 130��ͬ
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
	glfwSetInputCallback(window);//������ImGui_ImplOpenGL3_Init֮ǰ��ԭ�������˵����
	/*------------------------------------------------------------------------*/
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfw_InitForOpenGL(window, true);//����Ϊtrue��imgui���openglʵ��example�ڲ��ᱣ�����ǵ�callback
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	/*------------------------------------------------------------------------*/
	/*�׳���ʼ������¼����ϲ�Ӧ���������Ҫ�ɶ���*/
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
		/*�߼�����Ⱦ����ѭ��֡*/
		//int elapsed = 0;	//TODO
		/*������Ϣ���ĺͷַ���Ϣ*/
		ProcessEvts();
		/*--------------------------------------------------------------------*/
		/*FIXME: elapsed����*/
		static double t_tick = 0, t_pretick = 0, elapsed = 0;
		t_tick = glfwGetTime();
		elapsed = t_tick - t_pretick;
		t_pretick = t_tick;
		/*--------------------------------------------------------------------*/
		if (g_DrawFun) {
			g_DrawFun(window, elapsed, g_nFixframeStamp);
		}
		/*FIXME: fixframe�����������Ǽٵģ���ʽӦ��Ϊÿ��ĺ㶨֡��*/
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
	/*FIXME: д����������Ч�ġ�*/
	c2SysEvt::terminate sysevt_terminate(g_nSysETChunkOffset);
	c2PublishEvt(sysevt_terminate, sizeof(sysevt_terminate), g_nFixframeStamp);
 }//c2AppRun

////////////////////////////////////////////////////////////////////////////////
/*
Part & Factory
*/
#include"../c2Foundation/c2Part.h"
c2Part::CreationDict		c2Part::_CreationDict;	//FIXME: ��ʱ����

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
	return c2Part::_CreationDict.insert(	//����Ѵ���ͬ������ע�ᣬ�򷵻�false��
		c2Part::CreationDict::value_type(sClass, C)).second;
}
