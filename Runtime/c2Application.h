#pragma once

#include<boost/signals2/signal.hpp>
#include"./ThirdParty/imgui/imgui.h"

//FIXME: ���º���ʱ����ע���ڴˣ��Ժ���ø��ִ����ķ�ʽ�����ⷽ����¡���������������
//ֻ��Ϊ�˱��ں����Դ˺�Ϊ����������޸���صĴ��롣
//#define C2_CHECK_MEM

////////////////////////////////////////////////////////////////////////////////
/*Action��ϵ FIXME: EVENT��Ϊ��ֵ�ķ�ʽ*/
#include"./_c2Application/BrainTree.h"
struct c2IAction : public BrainTree::BehaviorTree {
	//TODO������ֵ��������Ҫ��Ϊ��ȷ���Լ�δ��ͬOneRounte�Ĺ�ϵ��
	c2IAction() : _pEvt(nullptr) {}
	const c2IEvent*	_pEvt;
	virtual Status update() {
		BOOST_ASSERT(_pEvt );
		std::cout << "EvType: " << _pEvt->getTypeAddChunkOffset() << " -> "
			<< typeid(*this).name() << "::update: success..." << std::endl;
		return Status::Success;
	}
};

////////////////////////////////////////////////////////////////////////////////
/*Driving framework of the whole application*/
C2API void c2AppRun(int SwapInterval, int nWndWidth, int nWndHeight,
						const char *sWndCaption, bool isBlocked = true);

/******************************************************************************/
/*Consumer subscribe event And Producer publish event.*/
C2API void c2asActSubEvt(c2IAction &Act, Uint32 esEvtTypeAddChunkOffset, size_t EvtSize);
C2API void c2asActUnsubEvt(c2IAction &Act, Uint32 esEvtTypeAddChunkOffset);
C2API void c2PublishEvt(const c2IEvent &Event, const size_t EventSize,
								const Uint64 esFixFrameStamp);

////////////////////////////////////////////////////////////////////////////////
/*Part & Factory*/
#include"./c2Foundation/c2Part.h"
using c2APart = c2Part::ARPart;
C2API c2APart c2CreatePart(const char *sClass, const char *sName = nullptr);
C2API bool _c2RegistPartClass(const char *sClass, c2Part::CreationFunc C);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create);

////////////////////////////////////////////////////////////////////////////////
#if (defined(__APPLE__) && TARGET_OS_IOS) 
#	include<glad/glad.h>
#	define GLFW_INCLUDE_NONE
#	define C2_USE_OPENGLES
#elif (defined(__ANDROID__)) || (defined(__EMSCRIPTEN__)) 
#	include<glad/glad.h>
#	define GLFW_INCLUDE_NONE
#	define C2_USE_OPENGLES

#else

#if !defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)     \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)     \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)     \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
#define IMGUI_IMPL_OPENGL_LOADER_GL3W
#endif

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#endif
