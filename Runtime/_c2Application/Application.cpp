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
#include"../ThirdParty/imgui/examples/imgui_impl_glfw.h"
#include"../ThirdParty/imgui/examples/imgui_impl_opengl3.h"

Uint32 g_nSysETChunkOffset	= 0;//必须确保system event type chunk是第一个append的
Uint64 g_nFixframeStamp		= 0;

////////////////////////////////////////////////////////////////////////////////
/*
Consumer subscribe event And Producer publish event
*/
#include<unordered_set>

/*暂时不可有重复，故用set。若使用multiset能实现类似boost::signal2对slot的灵活管控。
TODO：以后再完善*/
static std::vector<std::unordered_set<c2IAction*>> g_Evt2ActsetVector;

/*返回值为自定义事件类型Chunk的偏移值。每一个应用程序运行前就应该明确的，软件不能所以插拔
进来的event chunk，因为这会导致每个chunk的offect变化，同程序所匹配配合的其他网络配合端
或者序列化（例如录像，undo/redo等）所存数据而产生不兼容。*/
Uint32 c2AppendEvtTypesChunk(Uint32 nNewChunkSize) {
	Uint32 ret = g_Evt2ActsetVector.size();
	g_Evt2ActsetVector.resize(ret + nNewChunkSize);
	return ret;
}

/*============================================================================*/
/*只是先投递操作记录，实际操作在主循环消息处理帧主函数里进行，因为订阅或退订可能会在任
何时候（并不含有暗示改变我们多线程态度）被调用，不能破坏sig里的set及sigs。
TODO:还需要严密测试一下，例如使用订阅了某事件的ACT，里面再订阅或退订，并pub事件触发
ACT里再订阅或退订，等等。*/
static std::list<std::pair<c2IAction*, Uint32>>	g_ActSubEvtList;
C2API void c2asActSubEvt(c2IAction &Act,
						Uint32 esEvtTypeAddChunkOffset, size_t EvtSize) {
	g_ActSubEvtList.push_back(std::make_pair(&Act, esEvtTypeAddChunkOffset));
}
static std::list<std::pair<c2IAction*, Uint32>>	g_ActUnsubEvtList;
C2API void c2asActUnsubEvt(c2IAction &Act, Uint32 esEvtTypeAddChunkOffset) {
	g_ActUnsubEvtList.push_back(std::make_pair(&Act, esEvtTypeAddChunkOffset));
}

/******************************************************************************/
/*Driving framework of the whole application*/
static c2::tsMemQueue g_EventQueue(C2EVTQUEUE_INITSIZE);
C2API void c2PublishEvt(const c2IEvent &Event, size_t EventSize,
		const Uint64 esFixFrameStamp) {//FIXME: 用64的es是为了够大，但仍然跑爆问题？
	Event._nFixFrameStamp = esFixFrameStamp;
	g_EventQueue.push(&Event, EventSize);
}

////////////////////////////////////////////////////////////////////////////////
// Application framework
// -TODO：GLFW缺少移动设备上的一些INPUT消息，例如屏幕翻转、重力等
void glfwSetInputCallback(GLFWwindow* window);
static void glfwErrorCallback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}
C2API void c2AppRun(int SwapInterval, int nWndWidth, int nWndHeight,
						const char *sWndCaption, bool isBlocked) {
	std::atexit(glfwTerminate);
	/*增加系统事件chunk，必须确保system event type chunk是第一个append的*/
	g_nSysETChunkOffset = c2AppendEvtTypesChunk(c2SysET::AMMOUT + 1);
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
		/*处理上一帧实际订阅消息。*/
		for (std::pair<c2IAction*, Uint32>& v : g_ActSubEvtList) {
			BOOST_ASSERT(v.first);
			g_Evt2ActsetVector[v.second].insert(v.first);
		}
		g_ActSubEvtList.clear();
		/*处理上一帧实际退订消息，进行删除。*/
		for (std::pair<c2IAction*, Uint32>& v : g_ActUnsubEvtList) {
			BOOST_ASSERT(v.first);
			g_Evt2ActsetVector[v.second].erase(v.first);
		}
		g_ActUnsubEvtList.clear();
		/*分发消息*/
		static char g_pTempEventBuffer4UpdateFixFrame[C2EVTMSG_MAXSIZE];
		while (!g_EventQueue.isEmpty()) {
			g_EventQueue.pop(g_pTempEventBuffer4UpdateFixFrame, C2EVTMSG_MAXSIZE);
			c2IEvent &evt = *((c2IEvent*)g_pTempEventBuffer4UpdateFixFrame);
			for (c2IAction *tp_act : g_Evt2ActsetVector[evt.getTypeAddChunkOffset()]) {
				//for each (auto tp_act in sig) {
				BOOST_ASSERT(tp_act);
				tp_act->_pEvt = &evt;//传递此event作为参数。主要是BrainTree的update无参数，所以我们也不方便通过update函数传入参数。
				tp_act->update();
				tp_act->_pEvt = nullptr;//处理完后，则event指向重置。
			}
		}
		/*--------------------------------------------------------------------*/
		/*FIXME: elapsed计算*/
		static double t_tick = 0, t_pretick = 0, elapsed = 0;
		t_tick = glfwGetTime();
		elapsed = t_tick - t_pretick;
		t_pretick = t_tick;
		/*抛出fixupdate消息*/
		static c2SysEvt::updatefixframe sysevt_updatefixframe(g_nSysETChunkOffset);
		sysevt_updatefixframe._pWnd = window;
		sysevt_updatefixframe._dElapsed = elapsed;//FIXME: elapsed此处应该要有最大值保护。
		c2PublishEvt(sysevt_updatefixframe, sizeof(sysevt_updatefixframe), g_nFixframeStamp);
		/*TODO：同fix走事件投递不一样，直接用真正的同步性回调。事件体系初衷就是为逻辑等固定
		频率性质的逻辑服务的。*/
		/*抛出update消息*/
		static c2SysEvt::updateframe sysevt_updateframe(g_nSysETChunkOffset);
		sysevt_updateframe._pWnd = window;
		sysevt_updateframe._dElapsed = elapsed;//FIXME: elapsed此处应该要有最大值保护。
		c2PublishEvt(sysevt_updateframe, sizeof(sysevt_updateframe), g_nFixframeStamp);
		/*--------------------------------------------------------------------*/
		/*FIXME: fixframe步进。眼下是假的，正式应改为每秒的恒定帧率*/
		++g_nFixframeStamp;
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
