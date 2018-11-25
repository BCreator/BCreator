#include<iostream>
#include<stdio.h>

#include<boost/log/trivial.hpp>

/*TODO: Use the math library based on OpenCL in the future*/
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<ThirdParty/imgui/imgui.h>
#include<c2Foundation/c2Part.h>
#include<c2PreDefined.h>
#include<c2DefEvent.h>
#include<c2Application.h>
#include<c2PartController/c2PartCamera.h>

#include<GLFW/glfw3.h>
#include<stb/stb_image.h>

////////////////////////////////////////////////////////////////////////////////
static int g_nWindWidth = 1680, g_nWindHeight = 1050;
static GLuint g_ShaderProgramObject = 0;
static GLuint vao;
static glm::mat4 projection_mat(1), view_mat(1), model_mat(1);
static int projectionloc, viewloc, modelloc;
static GLuint texture0, texture1;
/*view control*/
static float lastX = 0;
static float lastY = 0;
static bool firstMouse = true;
static Camera camera;

#define PI 3.14159f

////////////////////////////////////////////////////////////////////////////////
class onUpdateFixFrameES : public c2IAction {
public:
	onUpdateFixFrameES() {
		_b_showsameline = 0;
	}
	int _b_showsameline;
	virtual Status update() {
		const c2SysEvt::updatefixframe& evt = *(static_cast<const c2SysEvt::updatefixframe*>(_pEvt));
		/*--------------------------------------------------------------------*/
		/*test imgui*/
		ImGui::Begin("C2 Director");
		ImGui::Text("NLP");
		if (ImGui::Button("open"))
			_b_showsameline++;
		if (_b_showsameline >= 3) {
			ImGui::SameLine();
			ImGui::Text("more than 3 times.");
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
					1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		bool show_demo_window = false;
		if( show_demo_window )
			ImGui::ShowDemoWindow(&show_demo_window);
		ImGui::End();
		/*--------------------------------------------------------------------*/
		/*glfwGetFramebufferSize（window，＆width，＆height); TODO: to used in onSize*/
		glViewport(0, 0, g_nWindWidth, g_nWindHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(g_ShaderProgramObject);
		projection_mat = glm::perspective(glm::radians(45.0f),
			float(g_nWindWidth) / float(g_nWindHeight), 0.1f, 100.0f);
		glUniformMatrix4fv(projectionloc, 1, GL_FALSE, glm::value_ptr(projection_mat));
		/*Camera keyboard control*/
		if (glfwGetKey(evt._pWnd, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, evt._dElapsed);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, evt._dElapsed);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, evt._dElapsed);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, evt._dElapsed);
//		view_mat = glm::mat4(1);
//		view_mat = glm::translate(view_mat, glm::vec3(0.0f, 0.0f, -3.0f));
		view_mat = camera.GetViewMatrix();
		glUniformMatrix4fv(viewloc, 1, GL_FALSE, &view_mat[0][0]);
		/*Model control*/
		static glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};
		model_mat = glm::mat4(1);
// 		model_mat = glm::rotate(model_mat, (float)glfwGetTime() * glm::radians(80.0f),
// 						glm::vec3(0.5f, 1.0f, 0.0f));

		float greenValue = (sin(glfwGetTime()) / 2.0f) + 0.5f;
		glUniform3f(glGetUniformLocation(g_ShaderProgramObject, "MyColor"),
										0.2f, greenValue, 0.3f);

		/*画*/
		/*在es下，glActiveTexture(GL_TEXTURE1); 导致GL_TEXTURE0错误无法绘制，
		imgui（20181123日更新）内部es有关应该存在不干净的操作，而影响的。非es无此问题。*/
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glUniform1i(glGetUniformLocation(g_ShaderProgramObject, "Texture0"), 0);
  		glActiveTexture(GL_TEXTURE1);
   		glBindTexture(GL_TEXTURE_2D, texture1);
  		glUniform1i(glGetUniformLocation(g_ShaderProgramObject, "Texture1"), 1);

		glBindVertexArray(vao);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 tmodel_mat(1);
			tmodel_mat = glm::translate(model_mat, cubePositions[i]);
			float angle = 20.0f * i;
			tmodel_mat = glm::rotate(tmodel_mat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(tmodel_mat));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//		glDrawElements(GL_POINTS, 6, GL_UNSIGNED_INT, 0);
		return Status::Success;
	}
};

////////////////////////////////////////////////////////////////////////////////
class onSysInitializedES : public c2IAction {
	virtual Status update() {
		const c2SysEvt::initialized& evt = *(static_cast<const c2SysEvt::initialized*>(_pEvt));
		glfwSetInputMode(evt._pWnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.15f, 0.15f, 0.10f, 1.00f);
		/*------------------------------------------------------------------------*/
#if defined(C2_USE_OPENGLES)
		char s_version[] = "#version 300 es\n";
#elif defined(__APPLE__)
#	error Only es3.0 & glsl 300 es
		char s_version[] = "#version 150\n";
#else
//#	error Only es3.0 & glsl 300 es
		char s_version[] = "#version 330 core\n";
#endif
		GLint issuccesed;

		char s_vs[] =
			//"layout (location = 0) in vec4 Pos;\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec2 aTexCoord;\n"
			"layout (location = 2) in vec3 aColor;\n"
			"out vec2 TexCoord;\n"
			"out vec3 Color;\n"
			"uniform vec3 MyColor;"
			"uniform mat4 Projection;"
			"uniform mat4 View;"
			"uniform mat4 Model;"
			"void main() {\n"
			//"	gl_PointSize= 10.3f;"
			//"	gl_Position = Projection * View * Model * aPos;\n"
			"	gl_Position = Projection * View * Model * vec4(aPos, 1.0);\n"
			"	TexCoord	= aTexCoord;\n"
			"	Color		= aColor+MyColor;\n"
			"}\n";
		char s_fs[] =
			"precision mediump float;\n"
			"in vec3 Color;\n"
			"in vec2 TexCoord;\n"
			"out vec4 FragColor;\n"
			"uniform sampler2D Texture0;\n"
			"uniform sampler2D Texture1;\n"
			"void main() {\n"
//			"	FragColor	= texture(Texture0, TexCoord);\n"
//			"	FragColor	= mix(texture(Texture1, TexCoord), vec4(Color, 1.0), 0.2);\n"
			"	FragColor	= mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord), 0.2) * vec4(Color, 1.0);\n"
			"}\n";

		GLuint vsobject = glCreateShader(GL_VERTEX_SHADER);
		BOOST_ASSERT(vsobject);
		const GLchar* s_vswithversion[] = { s_version, s_vs };
		glShaderSource(vsobject, 2, s_vswithversion, NULL);
		glCompileShader(vsobject);
		glGetShaderiv(vsobject, GL_COMPILE_STATUS, &issuccesed);
		if (!issuccesed) {
			GLint infolen = 0;
			glGetShaderiv(vsobject, GL_INFO_LOG_LENGTH, &infolen);
			if (infolen <= 1)
				return Status::Failure;
			char *infolog = new char[sizeof(char) * infolen];
			glGetShaderInfoLog(vsobject, infolen, NULL, infolog);
			BOOST_LOG_TRIVIAL(error) << "Compile vs: " << infolog;
			delete[] infolog;
			glDeleteShader(vsobject);
			return Status::Failure;
		}

		GLuint fsobject = glCreateShader(GL_FRAGMENT_SHADER);
		BOOST_ASSERT(fsobject);
		const GLchar* s_fswithversion[] = { s_version, s_fs };
		glShaderSource(fsobject, 2, s_fswithversion, NULL);
		glCompileShader(fsobject);
		glGetShaderiv(fsobject, GL_COMPILE_STATUS, &issuccesed);
		if (!issuccesed) {
			GLint infolen = 0;
			glGetShaderiv(fsobject, GL_INFO_LOG_LENGTH, &infolen);
			if (infolen <= 1)
				return Status::Failure;
			char *infolog = new char[sizeof(char) * infolen];
			glGetShaderInfoLog(fsobject, infolen, NULL, infolog);
			BOOST_LOG_TRIVIAL(error) << "Compile fs: " << infolog;
			delete[] infolog;
			glDeleteShader(fsobject);
			return Status::Failure;
		}

		g_ShaderProgramObject = glCreateProgram();
		BOOST_ASSERT(g_ShaderProgramObject);
		glAttachShader(g_ShaderProgramObject, vsobject);
		glAttachShader(g_ShaderProgramObject, fsobject);
		glLinkProgram(g_ShaderProgramObject);
		glGetProgramiv(g_ShaderProgramObject, GL_LINK_STATUS, &issuccesed);
		if (!issuccesed) {
			GLint infolen = 0;
			glGetProgramiv(g_ShaderProgramObject, GL_INFO_LOG_LENGTH, &infolen);
			if (infolen <= 1)
				return Status::Failure;
			char *infolog = new char[sizeof(char) * infolen];
			glGetProgramInfoLog(g_ShaderProgramObject, infolen, NULL, infolog);
			BOOST_LOG_TRIVIAL(error) << "Link shader: " << infolog;
			delete[] infolog;
			glDeleteProgram(g_ShaderProgramObject);
			return Status::Failure;
		}
		projectionloc = glGetUniformLocation(g_ShaderProgramObject, "Projection");
		viewloc = glGetUniformLocation(g_ShaderProgramObject, "View");
		modelloc = glGetUniformLocation(g_ShaderProgramObject, "Model");


		/*------------------------------------------------------------------------*/
		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f
		};

		/*------------------------------------------------------------------------*/
		GLuint vbo, ebo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
//		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		//Rest
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		/*------------------------------------------------------------------------*/
		int w, h, colorchannels;
		unsigned char* imagedata;
//		float boardercolor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		/*texture0*/
		glGenTextures(1, &texture0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, boardercolor);
		imagedata = stbi_load("container.jpg", &w, &h, &colorchannels, 0);
		BOOST_ASSERT(imagedata);
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, imagedata);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(imagedata);
		/*texture1*/
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, boardercolor);
		imagedata = stbi_load("awesomeface.png", &w, &h, &colorchannels, 0);
		BOOST_ASSERT(imagedata);
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagedata);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(imagedata);

		glBindTexture(GL_TEXTURE_2D, 0);//Rest

		/*------------------------------------------------------------------------*/
		BOOST_LOG_TRIVIAL(info) << "C2engine intialized.";
		return Status::Success;
	}
 };

 /*TODO: onTerminin free everything.*/
 class onTerminateES : public c2IAction {
 public:
	 virtual Status update() {
		 BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onTerminateES";
		 return Status::Success;
	 }
 };

 
////////////////////////////////////////////////////////////////////////////////
class onMouseButtonES : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onMouseButtonES";
		return Status::Success;
	}
};

class onCursorMovedES : public c2IAction {
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

class onCursorEnterES : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCursorEnterES";
		return Status::Success;
	}
};

class onScrolledES : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onScrolledES";
		return Status::Success;
	}
};

static const float cameraSpeed = .05;
static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
class onKeyES : public c2IAction {
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
// 			BOOST_LOG_TRIVIAL(info) << "+++++++++++++  onKeyES    " << ts;
		return Status::Success;
	}
};

class onCharInputES : public c2IAction {
public:
	virtual Status update() {
//		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCharInputES";
		return Status::Success;
	}
};

class onCharModsInputES : public c2IAction {
public:
	virtual Status update() {
//		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCharModsInputES";
		return Status::Success;
	}
};


////////////////////////////////////////////////////////////////////////////////
int main_gl() {
	Uint32 syset_chunkoffet = 0;
 	onSysInitializedES initialized;
	c2asActSubEvt(initialized, syset_chunkoffet+c2SysET::initialized,
		sizeof(c2SysEvt::initialized));
	onTerminateES terminate;
	c2asActSubEvt(terminate, syset_chunkoffet+c2SysET::terminate,
		sizeof(c2SysEvt::terminate));
	onUpdateFixFrameES updatefixframe;
	c2asActSubEvt(updatefixframe, syset_chunkoffet+c2SysET::updatefixframe,
		sizeof(c2SysEvt::updatefixframe));

	onMouseButtonES mouse_button;
	c2asActSubEvt(mouse_button, syset_chunkoffet + c2SysET::mouse_button,
		sizeof(c2SysEvt::mouse_button));
	onCursorMovedES cursor_moved;
	c2asActSubEvt(cursor_moved, syset_chunkoffet + c2SysET::cursor_moved,
		sizeof(c2SysEvt::cursor_moved));
	onCursorEnterES cursor_enter;
	c2asActSubEvt(cursor_enter, syset_chunkoffet + c2SysET::cursor_enter,
		sizeof(c2SysEvt::cursor_enter));
	onScrolledES scrolled;
	c2asActSubEvt(scrolled, syset_chunkoffet + c2SysET::scrolled,
		sizeof(c2SysEvt::scrolled));
	onKeyES key;
	c2asActSubEvt(key, syset_chunkoffet + c2SysET::key,
		sizeof(c2SysEvt::key));
	onCharInputES char_input;
	c2asActSubEvt(char_input, syset_chunkoffet + c2SysET::char_input,
		sizeof(c2SysEvt::char_input));
	onCharModsInputES charmods_input;
	c2asActSubEvt(charmods_input, syset_chunkoffet + c2SysET::charmods_input,
		sizeof(c2SysEvt::charmods_input));

	/**************************************************************************/
	c2AppRun(1, g_nWindWidth, g_nWindHeight, "C2engine.Creator", false);
 
 	return 0;
 }

////////////////////////////////////////////////////////////////////////////////
static const char* get_gl_error_text(int code) {
	switch (code) {
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";
	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";
	default:
		return "undefined error";
	}
}
static int gl_check_errors(const char *file, int line) {
	int errors = 0;
	while (true) {
		GLenum x = glGetError();
		if (x == GL_NO_ERROR)
			return errors;
		BOOST_LOG_TRIVIAL(error) << file << ":" << line << ": OpenGL error: "
			<< x << "(" << get_gl_error_text(x) << ")";
		errors++;
	}
}
#if _DEBUG
#  define GL(line) {							\
       line;									\
       if (gl_check_errors(__FILE__, __LINE__))	\
			BOOST_ASSERT(false);}
#else
#  define GL(line) line
#endif

static inline void CheckOpenGLError(const char* stmt, const char* fname, int line) {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("OpenGL error %08x, at %s:%i - for %s.\n", err, fname, line, stmt);
		exit(1);
	}
}

// helper macro that checks for GL errors.
#define GL_C(stmt) do {					\
	stmt;						\
	CheckOpenGLError(#stmt, __FILE__, __LINE__);	\
    } while (0)
