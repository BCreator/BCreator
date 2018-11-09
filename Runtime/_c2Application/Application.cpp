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

static Uint32 g_SysETChunkOffset= 0;//����ȷ��system event type chunk�ǵ�һ��append��

////////////////////////////////////////////////////////////////////////////////
/*
Consumer subscribe event And Producer publish event
*/
#include<unordered_set>

/*��ʱ�������ظ�������set����ʹ��multiset��ʵ������boost::signal2��slot�����ܿء�
TODO���Ժ�������*/
static std::vector<std::unordered_set<c2IAction*>> g_Evt2ActsetVector;

/*����ֵΪ�Զ����¼�����Chunk��ƫ��ֵ��ÿһ��Ӧ�ó�������ǰ��Ӧ����ȷ�ģ�����������Բ��
������event chunk����Ϊ��ᵼ��ÿ��chunk��offect�仯��ͬ������ƥ����ϵ�����������϶�
�������л�������¼��undo/redo�ȣ��������ݶ����������ݡ�*/
Uint32 c2AppendEvtTypesChunk(Uint32 nNewChunkSize) {
	Uint32 ret = g_Evt2ActsetVector.size();
	g_Evt2ActsetVector.resize(ret + nNewChunkSize);
	return ret;
}

/*============================================================================*/
/*ֻ����Ͷ�ݲ�����¼��ʵ�ʲ�������ѭ����Ϣ����֡����������У���Ϊ���Ļ��˶����ܻ�����
��ʱ�򣨲������а�ʾ�ı����Ƕ��߳�̬�ȣ������ã������ƻ�sig���set��sigs��
TODO:����Ҫ���ܲ���һ�£�����ʹ�ö�����ĳ�¼���ACT�������ٶ��Ļ��˶�����pub�¼�����
ACT���ٶ��Ļ��˶����ȵȡ�*/
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
		const Uint64 esFixFrameStamp) {//FIXME: ��64��es��Ϊ�˹��󣬵���Ȼ�ܱ����⣿
	Event._esFixFrameStamp = esFixFrameStamp;
	g_EventQueue.push(&Event, EventSize);
}

////////////////////////////////////////////////////////////////////////////////
/*Application framework
 - FIXME: ��ʱ������GLFW.GLEQ��OS BIND��Ϊ���ǵ�INPUT EVENT�������͵���GLFW����������
 ����IMGUI��OPENGL BIND�ĳ�ʼ��Ҳ��ͬGLFW������صģ�����������ʱ�⼸�������ĳ�ʼ��
 ������������ˡ�ʵ����APPLICATIONͬRENDERӦ�ý����Ӧ��ͬGUIϵͳ���
- FIXME��ʹ��GLEQ������Ϊ�����Ķ��У�дglfw��callbackҲ���ã�������ʱ�����Լ�����ܶ�
KEY��������Ի���GLEQ�޸ģ�ȥ�����Ķ��С�
- FIXME: GLEQ�ڵ��¼�������ʵ�����㹻��ȷ������û����ȷ���ֽڶ��롣��ʱ�ֲ���ֱ���޸�
gleq.h�ļ���Ŀǰ��ʱֻ�ǰ�GLEQ������һ����Ϣ���ͣ�Ȼ�󶼽���������
- TODO��GLFWȱ���ƶ��豸�ϵ�һЩINPUT��Ϣ��������Ļ��ת��������
- FIXME: ���������Լ���GLFW���̺����ص���Ȼ���ٷֱ����Imgui��gleq�ġ�IMGUI��EXAMPLE 
OS BIND��IO���Ѿ�����ͨ������wanted����ֵ���������ж��Ƿ�IMGUI���ĵ�INPU��Ϣ��Ҫ��Ҫ��
��Ӧ�ó���imgui.cppͷ�ĵ����FAQ��һ�������˵����*/
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
/*���Կ��ǰ�GLEQ������һ����Ϣ���ͣ�Ȼ�󶼽���������*/
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
	/*����ϵͳ�¼�chunk������ȷ��system event type chunk�ǵ�һ��append��*/
	g_SysETChunkOffset = c2AppendEvtTypesChunk(c2SysET::AMMOUT + 1);
	/*------------------------------------------------------------------------*/
	/*ת��GLFW.GLEQ��Ϣ*/
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
	ImGui_ImplGlfw_InitForOpenGL(window, false);//����Ϊfalse����Ϊ�����س������Լ���glfw input callback��
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
	/*�׳���ʼ������¼����ϲ�Ӧ���������Ҫ�ɶ���*/
	c2SysEvt::initialized sysevt_initialized(g_SysETChunkOffset);
	Uint64 es_fixframe_stamp = 0;
	c2PublishEvt(sysevt_initialized, sizeof(sysevt_initialized),
					es_fixframe_stamp);
	/*------------------------------------------------------------------------*/
	/* Loop until the user closes the window */
	void(*syseventscatch)() = isBlocked ? glfwWaitEvents : glfwPollEvents;
	while (!glfwWindowShouldClose(window)) {
		syseventscatch();
		/*��GLEQ����Ϣ*/
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
		/*�߼�����Ⱦ����ѭ��֡*/
		//int elapsed = 0;	//TODO
		/*������һ֡ʵ�ʶ�����Ϣ��*/
		for (std::pair<c2IAction*, Uint32>& v : g_ActSubEvtList) {
			BOOST_ASSERT(v.first);
			g_Evt2ActsetVector[v.second].insert(v.first);
		}
		g_ActSubEvtList.clear();
		/*������һ֡ʵ���˶���Ϣ������ɾ����*/
		for (std::pair<c2IAction*, Uint32>& v : g_ActUnsubEvtList) {
			BOOST_ASSERT(v.first);
			g_Evt2ActsetVector[v.second].erase(v.first);
		}
		g_ActUnsubEvtList.clear();
		/*�ַ���Ϣ*/
		static char g_pTempEventBuffer4UpdateFixFrame[C2EVTMSG_MAXSIZE];
		while (!g_EventQueue.isEmpty()) {
			g_EventQueue.pop(g_pTempEventBuffer4UpdateFixFrame, C2EVTMSG_MAXSIZE);
			c2IEvent &evt = *((c2IEvent*)g_pTempEventBuffer4UpdateFixFrame);
			for (c2IAction *tp_act : g_Evt2ActsetVector[evt._esTypeAddChunkOffset]) {
				//for each (auto tp_act in sig) {
				BOOST_ASSERT(tp_act);
				tp_act->_pEvt = &evt;//���ݴ�event��Ϊ��������Ҫ��BrainTree��update�޲�������������Ҳ������ͨ��update�������������
				tp_act->update();
				tp_act->_pEvt = nullptr;//���������eventָ�����á�
			}
		}
		/*--------------------------------------------------------------------*/
		/*FIXME: elapsed����*/
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
		/*�׳�fixupdate��Ϣ*/
		static c2SysEvt::updatefixframe sysevt_updatefixframe(g_SysETChunkOffset);
		sysevt_updatefixframe._esElapsed = elapsed;//FIXME: elapsed�˴�Ӧ��Ҫ�����ֵ������
		c2PublishEvt(sysevt_updatefixframe, sizeof(sysevt_updatefixframe), es_fixframe_stamp);
		/*TODO��ͬfix���¼�Ͷ�ݲ�һ����ֱ����������ͬ���Իص����¼���ϵ���Ծ���Ϊ�߼��ȹ̶�
		Ƶ�����ʵ��߼�����ġ�*/
		/*�׳�update��Ϣ*/
		static c2SysEvt::updateframe sysevt_updateframe(g_SysETChunkOffset);
		sysevt_updateframe._esElapsed = elapsed;//FIXME: elapsed�˴�Ӧ��Ҫ�����ֵ������
		c2PublishEvt(sysevt_updateframe, sizeof(sysevt_updateframe), es_fixframe_stamp);
		/*--------------------------------------------------------------------*/
		/*FIXME: fixframe�����������Ǽٵģ���ʽӦ��Ϊÿ��ĺ㶨֡��*/
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
