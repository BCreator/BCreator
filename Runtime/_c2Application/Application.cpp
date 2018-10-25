#include<vector>
#include<queue>
#include<iostream>
#include<boost/assert.hpp>
#include<boost/log/trivial.hpp>
#include"../c2Application.h"
#include"./tsMemQueue.h"

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
C2API void c2ActSubEvt(c2IAction &Act, Uint32 esEvtTypeAddChunkOffset, size_t EvtSize) {
	Act._EvtSize	= EvtSize;
	g_ActSubEvtList.push_back(std::make_pair(&Act, esEvtTypeAddChunkOffset));
}
static std::list<std::pair<c2IAction*, Uint32>>	g_ActUnsubEvtList;
C2API void c2ActUnsubEvt(c2IAction &Act, Uint32 esEvtTypeAddChunkOffset) {
	Act._EvtSize	= 0;
	g_ActUnsubEvtList.push_back(std::make_pair(&Act, esEvtTypeAddChunkOffset));
}

/******************************************************************************/
/* Driving framework of the whole application*/
static c2::tsMemQueue g_EventQueue(C2EVTQUEUE_INITSIZE);
C2API void c2PublishEvt(const c2IEvent &Event, size_t EventSize,
		const Uint64 esFixFrameStamp) {//FIXME: 用64的es是为了够大，但仍然跑爆问题？
	Event._esFixFrameStamp = esFixFrameStamp;
	g_EventQueue.push(&Event, EventSize);
}

////////////////////////////////////////////////////////////////////////////////
/*
 Frame & FixFrame control
*/
static char g_pTempEventBuffer4UpdateLogicalFrame[C2EVTMSG_MAXSIZE];

static void UpdateFixFrame(int Elapsed) {
	/*处理上一帧实际订阅消息。*/
	for (std::pair<c2IAction*, Uint32>& v : g_ActSubEvtList) {
		BOOST_ASSERT(v.first && (0!= v.first->_EvtSize));
		g_Evt2ActsetVector[v.second].insert(v.first);
	}
	g_ActSubEvtList.clear();
	/*处理上一帧实际退订消息，进行删除。*/
	for (std::pair<c2IAction*, Uint32>& v : g_ActUnsubEvtList) {
		BOOST_ASSERT(v.first && (0==v.first->_EvtSize));
		g_Evt2ActsetVector[v.second].erase(v.first);
	}
	g_ActUnsubEvtList.clear();
	/*分发消息*/
	static size_t st_evtsize;
	while (!g_EventQueue.isEmpty()) {
		st_evtsize= g_EventQueue.pop(g_pTempEventBuffer4UpdateLogicalFrame, C2EVTMSG_MAXSIZE);
		c2IEvent &evt = *((c2IEvent*)g_pTempEventBuffer4UpdateLogicalFrame);
		for (c2IAction *tp_act : g_Evt2ActsetVector[evt._esTypeAddChunkOffset]) {
		//for each (auto tp_act in sig) {
			BOOST_ASSERT(tp_act&&tp_act->_EvtSize== st_evtsize);
			tp_act->_pEvt = &evt;//传递此event作为参数。主要是BrainTree的update无参数，所以我们也不方便通过update函数传入参数。
			tp_act->update();
			tp_act->_pEvt = nullptr;//处理完，event指向则重置。
		}
	}
}
static void UpdateFrame(int Elapsed) {
	/*TODO*/
}

////////////////////////////////////////////////////////////////////////////////
/*
 System Events would be published by c2 app interval.
*/
static c2SysEvt::initialized g_SysEvtInitialized (
	c2AppendEvtTypesChunk(c2SysET::AMMOUT + 1)
);
C2API c2SysEvt::initialized& c2GetSysEvtInitialized() {
	return g_SysEvtInitialized;
}

////////////////////////////////////////////////////////////////////////////////
/*
 Application framework based on GLFW.GLEQ
*/
#define GLEQ_IMPLEMENTATION
#include"./gleq.h"
/*glfw.gleq events。GLEQ内的事件长度其实并不足够明确，并且没有明确的字节对齐。暂时又不想
直接修改gleq.h文件。目前暂时只是把GLEQ整个当一个消息类型，然后都交给他处理。
TODO：GLFW缺少移动设备上的一些INPUT消息，例如屏幕翻转、重力等*/
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
		BOOST_ASSERT(_EvtSize ==
				sizeof(static_cast<const c2gleqevts::c2GLEQevent&>(*_pEvt)));
		switch (event.type) {
		case GLEQ_WINDOW_MOVED:
			printf("Window moved to %i,%i\n", event.pos.x, event.pos.y);
			break;
		case GLEQ_BUTTON_PRESSED:
			printf("Mouse button %i pressed (mods 0x%x)\n",
				event.mouse.button,
				event.mouse.mods);
			break;
		case GLEQ_WINDOW_RESIZED:
			printf("Window resized to %ix%i\n", event.size.width,
				event.size.height);
			break;
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
				c2ActUnsubEvt(*this, _pEvt->_esTypeAddChunkOffset);
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
//static void _init_gleqevent() {
//	Uint32 etc_offset = c2AppendEvtTypesChunk(C2ET1::EVENTTYPE_AMMOUT + 1);
//	Uint32 etc2_offset = c2AppendEvtTypesChunk(C2ET2::EVENTTYPE_AMMOUT + 1);
////	BOOST_STATIC_ASSERT(0 == GLEQ_NONE);
////#if GLFW_VERSION_MINOR >= 3
////	Uint32 etc_offset_gleq = GLEQ_WINDOW_SCALE_CHANGED - GLEQ_NONE;
////#elif GLFW_VERSION_MINOR >= 2
////	Uint32 etc_offset_gleq = GLEQ_JOYSTICK_DISCONNECTED - GLEQ_NONE;
////#elif GLFW_VERSION_MINOR >= 1
////	Uint32 etc_offset_gleq = GLEQ_FILE_DROPPED - GLEQ_NONE;
////#else
////	Uint32 etc_offset_gleq = GLEQ_MONITOR_DISCONNECTED - GLEQ_NONE;
////#endif
////	etc_offset_gleq = c2AppendEvtTypesChunk(etc_offset_gleq);
//}
static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}C2API void c2AppRun(bool isBlocked, int SwapInterval) {
	/*转换GLFW.GLEQ消息*/
	Uint32 etc_offset_gleq;
	etc_offset_gleq = c2AppendEvtTypesChunk(c2gleqet::EVENTTYPE_AMMOUT + 1);
	c2gleqevts::c2GLEQevent st_c2_gleqevt(etc_offset_gleq);
	c2GLEQAction gleq_action;
	c2ActSubEvt(gleq_action, st_c2_gleqevt._esTypeAddChunkOffset, sizeof(st_c2_gleqevt));
	/*glfw begin*/
	glfwSetErrorCallback(error_callback);
	/* Initialize the library */
	if (!glfwInit())
		return;
	gleqInit();
	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow *window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(SwapInterval);
	gleqTrackWindow(window);
	/*抛出初始化完成事件，上层应用如果有需要可订阅*/
	//g_SysEvtInitialized.xxxx= xxxx;
	Uint64 es_logicalframe_stamp = 0;
	c2PublishEvt(g_SysEvtInitialized, sizeof(g_SysEvtInitialized),
					es_logicalframe_stamp);
	/* Loop until the user closes the window */
	void(*syseventscatch)() = isBlocked ? glfwWaitEvents : glfwPollEvents;
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		syseventscatch();
		/*从GLEQ拿消息*/
		while (gleqNextEvent(&(st_c2_gleqevt._GLEQevent))) {
			c2PublishEvt(st_c2_gleqevt, sizeof(st_c2_gleqevt),
									es_logicalframe_stamp);
		}
		gleqFreeEvent(&(st_c2_gleqevt._GLEQevent));
		/*逻辑、渲染等主循环帧*/
		int elapsed = 0;	//TODO
		UpdateFixFrame(elapsed);
		UpdateFrame(elapsed);
		++es_logicalframe_stamp;
	}
}

////////////////////////////////////////////////////////////////////////////////
/*
Part & Factory
*/
#include"../Metas/Part.h"
c2::Part::CreationDict		c2::Part::_CreationDict;	//FIXME: 暂时放这

C2API c2APart c2CreatePart(const char *sClass, const char *sName) {
	if (!sClass)
		return nullptr;
	c2::Part::CreationDict::iterator ci = c2::Part::_CreationDict.find(sClass);
	if (ci == c2::Part::_CreationDict.end())
		return nullptr;
	c2::Part::CreationFunc	create = ci->second;
	BOOST_ASSERT(create);
	return create();
}

C2API bool _c2RegistPartClass(const char *sClass, c2::Part::CreationFunc C) {
	if (!sClass || !C)
		return false;
	return c2::Part::_CreationDict.insert(	//如果已存在同样类名注册，则返回false。
		c2::Part::CreationDict::value_type(sClass, C)).second;
}
