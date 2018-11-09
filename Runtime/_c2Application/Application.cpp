#ifdef _WIN32 //FIXME
#	include<windows.h>
#endif
#include<vector>
#include<queue>
#include<iostream>
#include<boost/assert.hpp>
#include<boost/log/trivial.hpp>
#include"../c2PreDefined.h"
#include"../c2DefEvent.h"
#include"../c2Application.h"
#include"./tsMemQueue.h"

static Uint32 g_SysETChunkOffset= 0;//必须确保system event type chunk是第一个append的

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
#include<utility>
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
	Event._esFixFrameStamp = esFixFrameStamp;
	g_EventQueue.push(&Event, EventSize);
}

////////////////////////////////////////////////////////////////////////////////
/*Application framework
 - FIXME: 暂时借用了GLFW.GLEQ的OS BIND作为我们的INPUT EVENT，这样就得用GLFW来创建窗口
 ，而IMGUI的OPENGL BIND的初始化也是同GLFW窗机相关的，简便起见，暂时这几个东西的初始化
 都耦合在这里了。实际上APPLICATION同RENDER应该解耦，更应该同GUI系统解耦。
- FIXME：使用GLEQ并不是为了他的队列，写glfw的callback也还好，就是暂时懒得自己定义很多
KEY。后面可以基于GLEQ修改，去掉他的队列。
- FIXME: GLEQ内的事件长度其实并不足够明确，并且没有明确的字节对齐。暂时又不想直接修改
gleq.h文件。目前暂时只是把GLEQ整个当一个消息类型，然后都交给他处理。
- TODO：GLFW缺少移动设备上的一些INPUT消息，例如屏幕翻转、重力等
- FIXME: 设置我们自己的GLFW键盘和鼠标回调，然后再分别调用Imgui及gleq的。IMGUI的EXAMPLE 
OS BIND里IO里已经有了通过几个wanted布尔值，来自行判断是否IMGUI消耗掉INPU消息，要不要让
给应用程序。imgui.cpp头文档里的FAQ第一条有相关说明。*/
#include "../ThirdParty/imgui/examples/imgui_impl_glfw.h"
#include "../ThirdParty/imgui/examples/imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

/******************************************************************************/
#define GLEQ_IMPLEMENTATION
#include"./gleq.h"
C2EvtTypeChunkBegin(c2gleqet)
	c2GLEQevent = 0,
	EVENTTYPE_AMMOUT,
C2EvtTypeChunkEnd
#pragma pack(push, 1)
/*可以考虑把GLEQ整个当一个消息类型，然后都交给他处理*/
C2DefOneEvtBegin(c2gleqet, c2gleqevts, c2GLEQevent)
	GLEQevent	_GLEQevent;
C2DefOneEvtEnd
#pragma pack(pop)
struct c2GLEQAction : public c2IAction {
	virtual Status update() {
		std::cout << typeid(*this).name() << "::update | ......" << std::endl;
		const GLEQevent &event =
				static_cast<const c2gleqevts::c2GLEQevent*>(_pEvt)->_GLEQevent;
		switch (event.type) {
		case GLEQ_WINDOW_MOVED:
			printf("Window moved to %i,%i\n", event.pos.x, event.pos.y);
			break;
		case GLEQ_WINDOW_RESIZED:
			printf("Window resized to %ix%i\n", event.size.width,
				event.size.height);
			break;
		/*--------------------------------------------------------------------*/
		case GLEQ_BUTTON_PRESSED:
			ImGui_ImplGlfw_MouseButtonCallback(event.window, event.mouse.button,
				event.mouse.mods, GLEQ_BUTTON_PRESSED);
			printf("Mouse button %i pressed (mods 0x%x)\n",
				event.mouse.button,
				event.mouse.mods);
			break;
		case GLEQ_BUTTON_RELEASED:
			ImGui_ImplGlfw_MouseButtonCallback(event.window, event.mouse.button,
										event.mouse.mods, GLEQ_BUTTON_RELEASED);
			printf("Mouse button %i pressed (mods 0x%x)\n",
				event.mouse.button,
				event.mouse.mods);
			break;
		case GLEQ_SCROLLED:
			ImGui_ImplGlfw_ScrollCallback(event.window, event.scroll.x, event.scroll.y);
			printf("Scrolled %0.2f,%0.2f\n",
				event.scroll.x, event.scroll.y);
			break;
		case GLEQ_CODEPOINT_INPUT:
			ImGui_ImplGlfw_CharCallback(event.window, event.codepoint);
			printf("Codepoint U+%05X input\n", event.codepoint);
			break;
		/*--------------------------------------------------------------------*/
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
			if (0x1 == event.keyboard.scancode) {
				BOOST_ASSERT(_pEvt);
				c2asActUnsubEvt(*this, _pEvt->_esTypeAddChunkOffset);
			}
			break;
		default:
			printf("Error: Unknown event %i\n", event.type);
			break;
		}
		return Status::Success;
	}
};

/******************************************************************************/
static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}
C2API void c2AppRun(bool isBlocked, int SwapInterval,
					int nWndWidth, int nWndHeight, const char *sWndCaption) {
	/*增加系统事件chunk，必须确保system event type chunk是第一个append的*/
	g_SysETChunkOffset = c2AppendEvtTypesChunk(c2SysET::AMMOUT + 1);
	/*------------------------------------------------------------------------*/
	/*转换GLFW.GLEQ消息*/
	Uint32 etc_offset_gleq;
	etc_offset_gleq = c2AppendEvtTypesChunk(c2gleqet::EVENTTYPE_AMMOUT + 1);
	c2gleqevts::c2GLEQevent st_c2_gleqevt(etc_offset_gleq);
	c2GLEQAction gleq_action;
	c2asActSubEvt(gleq_action, st_c2_gleqevt._esTypeAddChunkOffset, sizeof(st_c2_gleqevt));
	/*glfw begin*/
	glfwSetErrorCallback(error_callback);
	/* Initialize the library */
	if (!glfwInit())
		return;
	gleqInit();
	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow *window = glfwCreateWindow(nWndWidth, nWndHeight, sWndCaption, NULL, NULL);
	if (!window) {
		glfwTerminate();
		return;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(SwapInterval);
	gleqTrackWindow(window);
	/*------------------------------------------------------------------------*/
	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
	// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() != 0;
#endif
	if (err) {
		BOOST_LOG_TRIVIAL(fatal) << "Failed to initialize OpenGL loader!";
		return;
	}
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
//	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplGlfw_InitForOpenGL(window, false);//设置为false，是为了重载成我们自己的glfw input callback。
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
//	glfwMakeContextCurrent(window);
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	/*------------------------------------------------------------------------*/
	/*抛出初始化完成事件，上层应用如果有需要可订阅*/
	c2SysEvt::initialized sysevt_initialized(g_SysETChunkOffset);
	Uint64 es_fixframe_stamp = 0;
	c2PublishEvt(sysevt_initialized, sizeof(sysevt_initialized),
					es_fixframe_stamp);
	/*------------------------------------------------------------------------*/
	/* Loop until the user closes the window */
	void(*syseventscatch)() = isBlocked ? glfwWaitEvents : glfwPollEvents;
	while (!glfwWindowShouldClose(window)) {
		syseventscatch();
		/*从GLEQ拿消息*/
		while (gleqNextEvent(&(st_c2_gleqevt._GLEQevent))) {
			c2PublishEvt(st_c2_gleqevt, sizeof(st_c2_gleqevt),
				es_fixframe_stamp);
		}
		gleqFreeEvent(&(st_c2_gleqevt._GLEQevent));
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
			for (c2IAction *tp_act : g_Evt2ActsetVector[evt._esTypeAddChunkOffset]) {
				//for each (auto tp_act in sig) {
				BOOST_ASSERT(tp_act);
				tp_act->_pEvt = &evt;//传递此event作为参数。主要是BrainTree的update无参数，所以我们也不方便通过update函数传入参数。
				tp_act->update();
				tp_act->_pEvt = nullptr;//处理完后，则event指向重置。
			}
		}
		/*--------------------------------------------------------------------*/
		/*FIXME: elapsed计算*/
		//boost::posix_time::second_clock
		static Uint32 t_tick = 0, t_pretick = 0, elapsed = 0;
#ifdef _WIN32
		//static LARGE_INTEGER t_tick, t_pretick;
		//QueryPerformanceCounter(&t_tick);
		//QueryPerformanceCounter(&t_pretick);
		t_tick = timeGetTime();
#else
#include <sys/time.h>
		struct timeval tm;
		gettimeofday(&tm, NULL);
		m_fpsCurrentTicks = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
		m_deltaTime = (m_fpsCurrentTicks - m_fpsPreviousTicks);
#endif //_WIN32
		elapsed = t_tick - t_pretick;
		t_pretick = t_tick;
		/*抛出fixupdate消息*/
		static c2SysEvt::updatefixframe sysevt_updatefixframe(g_SysETChunkOffset);
		sysevt_updatefixframe._esElapsed = elapsed;//FIXME: elapsed此处应该要有最大值保护。
		c2PublishEvt(sysevt_updatefixframe, sizeof(sysevt_updatefixframe), es_fixframe_stamp);
		/*TODO：同fix走事件投递不一样，直接用真正的同步性回调。事件体系初衷就是为逻辑等固定
		频率性质的逻辑服务的。*/
		/*抛出update消息*/
		static c2SysEvt::updateframe sysevt_updateframe(g_SysETChunkOffset);
		sysevt_updateframe._esElapsed = elapsed;//FIXME: elapsed此处应该要有最大值保护。
		c2PublishEvt(sysevt_updateframe, sizeof(sysevt_updateframe), es_fixframe_stamp);
		/*--------------------------------------------------------------------*/
		/*FIXME: fixframe步进。眼下是假的，正式应改为每秒的恒定帧率*/
		++es_fixframe_stamp;
		/*--------------------------------------------------------------------*/
		// Rendering
		ImGui::Render();
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		/*--------------------------------------------------------------------*/
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
	}
}

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
