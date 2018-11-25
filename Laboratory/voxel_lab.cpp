#include<iostream>
#include<stdio.h>

#include<boost/log/trivial.hpp>

/*TODO: Use the math library based on OpenCL in the future*/
//#define GLM_FORCE_MESSAGES
//#define GLM_FORCE_SIMD_
//#define GLM_FORCE_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_INLINE
//#define GLM_FORCE_CTOR_INIT//GLM的mat默认是不强制identity的，要用这个宏，太容易造成错误了！manual里居然没有提。
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<ThirdParty/imgui/imgui.h>
#include<c2Foundation/c2Part.h>
#include<c2PreDefined.h>
#include<c2DefEvent.h>
#include<c2Application.h>
#include<c2PartController/c2PartCamera.h>
#include<Render/Shader.h>

#include<GLFW/glfw3.h>
#include<stb/stb_image.h>

//1/////////////////////////////////////////////////////////////////////////////
//#define USE_INTEGER

////////////////////////////////////////////////////////////////////////////////
static int g_nWindWidth = 1680, g_nWindHeight = 1050;
static GLuint VBO, vao_block, vao_lamp;
/*view control*/
static Camera camera;
static float lastX = 0;
static float lastY = 0;
static bool firstMouse = true;
/*lighting*/
static glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
static Shader lightingShader;
static Shader lampShader;

////////////////////////////////////////////////////////////////////////////////
class onUpdateFixFrameVoxel : public c2IAction {
public:
	virtual Status update() {
		const c2SysEvt::updatefixframe& evt = *(static_cast<const c2SysEvt::updatefixframe*>(_pEvt));
		/*--------------------------------------------------------------------*/
		ImGui::Begin("C2 Director");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		/*--------------------------------------------------------------------*/
		lightingShader.use();
		/*Camera keyboard control*/
		float delta = static_cast<float>(evt._dElapsed);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_LEFT_SHIFT))
			delta *= 5.0f;
		if (glfwGetKey(evt._pWnd, GLFW_KEY_E) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, delta);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, delta);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, delta);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_F) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, delta);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_Q) == GLFW_PRESS)
			camera.ProcessKeyboard(UP, delta);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(DOWN, delta);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
			(float)g_nWindWidth / (float)g_nWindHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		//		Render
		/*--------------------------------------------------------------------*/
		/*glfwGetFramebufferSize（window，＆width，＆height); TODO: to used in onSize*/
		glViewport(0, 0, g_nWindWidth, g_nWindHeight);//FIXME:放到framebuffer resize的地方
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*--------------------------------------------------------------------*/
		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);
		// world transformation
		glm::mat4 model(1.0f);//最好显性identiy！
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);
		glBindVertexArray(vao_block);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		/*--------------------------------------------------------------------*/
		// also draw the lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);//最好显性identiy！
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);
		glBindVertexArray(vao_lamp);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		/*--------------------------------------------------------------------*/
		return Status::Success;
	}
};

////////////////////////////////////////////////////////////////////////////////
class onSysInitializedVoxel : public c2IAction {
	virtual Status update() {
		const c2SysEvt::initialized& evt = *(static_cast<const c2SysEvt::initialized*>(_pEvt));
		glfwSetInputMode(evt._pWnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glEnable(GL_DEPTH_TEST);
		/*--------------------------------------------------------------------*/
#ifdef C2_USE_OPENGLES
		lightingShader.create("es3block.vs", "es3block.fs");
		lampShader.create("es3lamp.vs", "es3lamp.fs");
#else
		lightingShader.create("330block.vs", "330block.fs");
		lampShader.create("330lamp.vs", "330lamp.fs");
#endif//C2_USE_OPENGLES
#ifdef USE_INTEGER
		static  GLint vertices[] = {
				-1, -1, -1, +0,  +0, -1,
				+1, -1, -1, +0,  +0, -1,
				+1, +1, -1, +0,  +0, -1,
				+1, +1, -1, +0,  +0, -1,
				-1, +1, -1, +0,  +0, -1,
				-1, -1, -1, +0,  +0, -1
		};
#else//USE_INTEGER
		static GLfloat vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};
#endif//USE_INTEGER
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		/*--------------------------------------------------------------------*/
		//block
		glGenVertexArrays(1, &vao_block);
		glBindVertexArray(vao_block);
#ifdef USE_INTEGER
		glVertexAttribIPointer(0, 3, GL_INT, 6 * sizeof(GLint), (void*)0);
#else
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
#endif
		glEnableVertexAttribArray(0);
		// normal attribute
#ifdef USE_INTEGER
		glVertexAttribIPointer(1, 3, GL_INT, 6 * sizeof(GLint), (void*)(3 * sizeof(GLint)));
#else
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
#endif
		glEnableVertexAttribArray(1);
		/*--------------------------------------------------------------------*/
		//lamp
		glGenVertexArrays(1, &vao_lamp);
		glBindVertexArray(vao_lamp);
#ifdef USE_INTEGER
		glVertexAttribIPointer(0, 3, GL_INT, 6 * sizeof(GLint), (void*)0);
#else
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
#endif
		glEnableVertexAttribArray(0);
		/*--------------------------------------------------------------------*/
		//Rest
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		/*====================================================================*/
		BOOST_LOG_TRIVIAL(info) << "C2engine intialized.";
		return Status::Success;
	}
};

/*TODO: onTerminin free everything.*/
class onTerminateVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onTerminate";
		return Status::Success;
	}
};


////////////////////////////////////////////////////////////////////////////////
class onMouseButtonVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onMouseButton";
		return Status::Success;
	}
};

class onCursorMovedVoxel : public c2IAction {
public:
	virtual Status update() {
		const c2SysEvt::cursor_moved& evt = *(static_cast<const c2SysEvt::cursor_moved*>(_pEvt));
		if (firstMouse) {
			lastX = evt._x;
			lastY = evt._y;
			firstMouse = false;
		}
		float xoffset = evt._x - lastX;
		float yoffset = lastY - evt._y; // reversed since y-coordinates go from bottom to top
		lastX = evt._x;
		lastY = evt._y;
		camera.ProcessMouseMovement(xoffset, yoffset);
		return Status::Success;
	}
};

class onCursorEnterVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCursorEnter";
		return Status::Success;
	}
};

class onScrolledVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onScrolled";
		return Status::Success;
	}
};

static const float cameraSpeed = .05;
static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
class onKeyVoxel : public c2IAction {
public:
	virtual Status update() {
		const c2SysEvt::key& evt = *(static_cast<const c2SysEvt::key*>(_pEvt));
		if (evt._nAction != GLFW_PRESS)
			return Status::Success;
		switch (evt._nKey) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(evt._pWnd, true);
			break;
		}
		// 		const char *ts = glfwGetKeyName(evt._nKey, evt._nScancode);
		// 		if(ts)
		// 			BOOST_LOG_TRIVIAL(info) << "+++++++++++++  onKey   " << ts;
		return Status::Success;
	}
};

class onCharInputVoxel : public c2IAction {
public:
	virtual Status update() {
		//		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCharInput";
		return Status::Success;
	}
};

class onCharModsInputVoxel : public c2IAction {
public:
	virtual Status update() {
		//		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCharModsInput";
		return Status::Success;
	}
};


////////////////////////////////////////////////////////////////////////////////
int main_voxel() {
	Uint32 syset_chunkoffet = 0;
	onSysInitializedVoxel initialized;
	c2asActSubEvt(initialized, syset_chunkoffet + c2SysET::initialized,
		sizeof(c2SysEvt::initialized));
	onTerminateVoxel terminate;
	c2asActSubEvt(terminate, syset_chunkoffet + c2SysET::terminate,
		sizeof(c2SysEvt::terminate));
	onUpdateFixFrameVoxel updatefixframe;
	c2asActSubEvt(updatefixframe, syset_chunkoffet + c2SysET::updatefixframe,
		sizeof(c2SysEvt::updatefixframe));

	onMouseButtonVoxel mouse_button;
	c2asActSubEvt(mouse_button, syset_chunkoffet + c2SysET::mouse_button,
		sizeof(c2SysEvt::mouse_button));
	onCursorMovedVoxel cursor_moved;
	c2asActSubEvt(cursor_moved, syset_chunkoffet + c2SysET::cursor_moved,
		sizeof(c2SysEvt::cursor_moved));
	onCursorEnterVoxel cursor_enter;
	c2asActSubEvt(cursor_enter, syset_chunkoffet + c2SysET::cursor_enter,
		sizeof(c2SysEvt::cursor_enter));
	onScrolledVoxel scrolled;
	c2asActSubEvt(scrolled, syset_chunkoffet + c2SysET::scrolled,
		sizeof(c2SysEvt::scrolled));
	onKeyVoxel key;
	c2asActSubEvt(key, syset_chunkoffet + c2SysET::key,
		sizeof(c2SysEvt::key));
	onCharInputVoxel char_input;
	c2asActSubEvt(char_input, syset_chunkoffet + c2SysET::char_input,
		sizeof(c2SysEvt::char_input));
	onCharModsInputVoxel charmods_input;
	c2asActSubEvt(charmods_input, syset_chunkoffet + c2SysET::charmods_input,
		sizeof(c2SysEvt::charmods_input));

	/**************************************************************************/
	c2AppRun(1, g_nWindWidth, g_nWindHeight, "C2engine.Creator", false);

	return 0;
}
