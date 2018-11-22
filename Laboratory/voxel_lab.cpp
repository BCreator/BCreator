#include<iostream>
#include<stdio.h>

#include<boost/log/trivial.hpp>

#include<ThirdParty/imgui/imgui.h>
#include<c2Foundation/c2Part.h>
#include<c2PreDefined.h>
#include<c2DefEvent.h>
#include<c2Application.h>

#include<GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include<stb/stb_image.h>

/**************************************************************************/
/*TODO: Use the math library based on OpenCL in the future*/
#include<glm/glm.hpp> //The math library used by GLFW internally
#include<glm/gtc/type_ptr.hpp>

////////////////////////////////////////////////////////////////////////////////
static int g_nWindWidth = 1280, g_nWindHeight = 720;
static GLuint g_ShaderProgramObject = 0;
static GLuint vao;
static glm::mat4 projection_mat(1), view_mat(1), model_mat(1);
static int projectionloc, viewloc, modelloc;
static GLuint texture0, texture1;

#define PI 3.14159f

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
int gl_check_errors(const char *file, int line) {
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

inline void CheckOpenGLError(const char* stmt, const char* fname, int line) {
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

////////////////////////////////////////////////////////////////////////////////
class onUpdateFixFrameVoxel : public c2IAction {
public:
	onUpdateFixFrameVoxel() {
		_b_showsameline = 0;
	}
	int _b_showsameline;
	virtual Status update() {
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
		bool show_demo_window = true;
		if( show_demo_window )
			ImGui::ShowDemoWindow(&show_demo_window);
		ImGui::End();
		/*--------------------------------------------------------------------*/
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
		/*glfwGetFramebufferSize£¨window£¬£¦width£¬£¦height); TODO: to used in onSize*/
		glViewport(0, 0, g_nWindWidth, g_nWindHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*Camera control*/
//		view_mat = glm::mat4(1);
		model_mat = glm::mat4(1);
//		view_mat = glm::translate(view_mat, glm::vec3(0.0f, 0.0f, -3.0f));
// 		model_mat = glm::rotate(model_mat, (float)glfwGetTime() * glm::radians(80.0f),
// 						glm::vec3(0.5f, 1.0f, 0.0f));
		glUniformMatrix4fv(viewloc, 1, GL_FALSE, glm::value_ptr(view_mat));

		float greenValue = (sin(glfwGetTime()) / 2.0f) + 0.5f;
		glUniform3f(glGetUniformLocation(g_ShaderProgramObject, "MyColor"),
										0.2f, greenValue, 0.3f);

		/*»­*/
 		glActiveTexture(GL_TEXTURE0);
 		glBindTexture(GL_TEXTURE_2D, texture0);
 		glActiveTexture(GL_TEXTURE1);
 		glBindTexture(GL_TEXTURE_2D, texture1);

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
class onSysInitializedVoxel : public c2IAction {
	virtual Status update() {
		glEnable(GL_DEPTH_TEST);
//		glClearColor(0.15f, 0.15f, 0.10f, 1.00f);
		/*------------------------------------------------------------------------*/
#if defined(C2_USE_OPENGLES)
		char s_version[] = "#version 300 es\n";
#elif defined(__APPLE__)
#	error Only es3.0 & glsl 300 es
		char s_version[] = "#version 150\n";
#else
#	error Only es3.0 & glsl 300 es
		char s_version[] = "#version 130\n";
#endif
		GLint issuccesed;

		char s_vs[] =
			//"layout (location = 0) in vec4 Pos;\n"
			"layout (location = 0) in vec3 InPos;\n"
			"layout (location = 1) in vec2 InTexCoord;\n"
			"layout (location = 2) in vec3 InColor;\n"
			"out vec2 TexCoord;\n"
			"out vec3 Color;\n"
			"uniform vec3 MyColor;"
			"uniform mat4 Projection;"
			"uniform mat4 View;"
			"uniform mat4 Model;"
			"void main() {\n"
			//"	gl_PointSize= 10.3f;"
			//"	gl_Position = Projection * View * Model * InPos;\n"
			"	gl_Position = Projection * View * Model * vec4(InPos, 1.0);\n"
			"	TexCoord	= InTexCoord;\n"
//			"	Color		= vec3(0.3, 0.0, 0.3)+MyColor;\n"
			"	Color		= InColor+MyColor;\n"
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

		char s_fs[] =
			"precision mediump float;\n"
			"in vec3 Color;\n"
			"in vec2 TexCoord;\n"
			"out vec4 FragColor;\n"
			"uniform sampler2D Texture0;\n"
			"uniform sampler2D Texture1;\n"
			"void main() {\n"
//			"	FragColor	= texture0(Texture0, TexCoord) * vec4(Color, 1.0);\n"
			"	FragColor	= mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord), 0.2) * vec4(Color, 1.0);\n"
			"}\n";
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

		GLint shaderprogram_object = glCreateProgram();
		BOOST_ASSERT(shaderprogram_object);
		glAttachShader(shaderprogram_object, vsobject);
		glAttachShader(shaderprogram_object, fsobject);
		glLinkProgram(shaderprogram_object);
		glGetProgramiv(shaderprogram_object, GL_LINK_STATUS, &issuccesed);
		if (!issuccesed) {
			GLint infolen = 0;
			glGetProgramiv(shaderprogram_object, GL_INFO_LOG_LENGTH, &infolen);
			if (infolen <= 1)
				return Status::Failure;
			char *infolog = new char[sizeof(char) * infolen];
			glGetProgramInfoLog(shaderprogram_object, infolen, NULL, infolog);
			BOOST_LOG_TRIVIAL(error) << "Link shader: " << infolog;
			delete[] infolog;
			glDeleteProgram(shaderprogram_object);
			return Status::Failure;
		}
		g_ShaderProgramObject = shaderprogram_object;

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

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		projection_mat	= glm::perspective(glm::radians(45.0f),
							float(g_nWindWidth) / float(g_nWindHeight), 0.1f, 100.0f);
		projectionloc	= glGetUniformLocation(g_ShaderProgramObject, "Projection");
		viewloc			= glGetUniformLocation(g_ShaderProgramObject, "View");
		modelloc		= glGetUniformLocation(g_ShaderProgramObject, "Model");
		glUseProgram(g_ShaderProgramObject);
		glUniformMatrix4fv(projectionloc, 1, GL_FALSE, glm::value_ptr(projection_mat));

		/*------------------------------------------------------------------------*/
		/*texture0*/
		int w, h, colorchannels;
		unsigned char* imagedata = stbi_load("container.jpg", &w, &h, &colorchannels, 0);
		BOOST_ASSERT(imagedata);
		glGenTextures(1, &texture0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 		float boardercolor[] = {1.0f, 0.0f, 0.0f, 1.0f};
 		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, boardercolor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, imagedata);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(imagedata);
		glBindTexture(GL_TEXTURE_2D, 0);
		//glUniform1i(glGetUniformLocation(g_ShaderProgramObject, "Texture0"), texture0);

		/*------------------------------------------------------------------------*/
		BOOST_LOG_TRIVIAL(info) << "C2engine intialized.";
		return Status::Success;
	}
 };

 /*TODO: onTerminin free everything.*/
 class onTerminateVoxel : public c2IAction {
 public:
	 virtual Status update() {
		 BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onTerminateVoxel";
		 return Status::Success;
	 }
 };

 
////////////////////////////////////////////////////////////////////////////////
class onMouseButtonVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onMouseButtonVoxel";
		return Status::Success;
	}
};

class onCursorMovedVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCursorMovedVoxel";
		return Status::Success;
	}
};

class onCursorEnterVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCursorEnterVoxel";
		return Status::Success;
	}
};

class onScrolledVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onScrolledVoxel";
		return Status::Success;
	}
};

const float cameraSpeed = .05;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
class onKeyVoxel : public c2IAction {
public:
	virtual Status update() {
		const Uint16 &scancode = static_cast<const c2SysEvt::key*>(_pEvt)->_nScancode;
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onKeyVoxel    "<< scancode;
		switch(scancode) {
		case 16://q
			break;
		case 17://w
			break;
		case 18://e
			cameraPos += cameraSpeed * cameraFront;
			break;
		case 19://r
			break;
		case 30://a
			break;
		case 31://s
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;
		case 32://d
			cameraPos -= cameraSpeed * cameraFront;
			break;
		case 33://f
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;
		case 44://z
			break;
		}
		view_mat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);;
		return Status::Success;
	}
};

class onCharInputVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCharInputVoxel";
		return Status::Success;
	}
};

class onCharModsInputVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCharModsInputVoxel";
		return Status::Success;
	}
};


////////////////////////////////////////////////////////////////////////////////
int main_Voxel() {
	Uint32 syset_chunkoffet = 0;
 	onSysInitializedVoxel initialized;
	c2asActSubEvt(initialized, syset_chunkoffet+c2SysET::initialized,
		sizeof(c2SysEvt::initialized));
	onTerminateVoxel terminate;
	c2asActSubEvt(terminate, syset_chunkoffet+c2SysET::terminate,
		sizeof(c2SysEvt::terminate));
	onUpdateFixFrameVoxel updatefixframe;
	c2asActSubEvt(updatefixframe, syset_chunkoffet+c2SysET::updatefixframe,
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
