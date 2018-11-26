#include<boost/assert.hpp>

#include<glm/glm.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include"../c2Application.h"
#include"../c2PartController/c2PartCamera.h"
#include"../Render/Shader.h"
#include"../Render/Render.h"

#include"c2PartLight.h"

//1/////////////////////////////////////////////////////////////////////////////
static GLuint vao_lamp= 0;
static Shader lampShader;
static void _BuildVAOLight() {
#ifdef C2_USE_OPENGLES
	lampShader.create("es3lamp.vs", "es3lamp.fs");
#else
	lampShader.create("330lamp.vs", "330lamp.fs");
#endif//C2_USE_OPENGLES
	glGenVertexArrays(1, &vao_lamp);
	glBindVertexArray(vao_lamp);
	glBindBuffer(GL_ARRAY_BUFFER, c2GetBoxVBOFloat());
#ifdef USE_INTEGER
	glBindBuffer(GL_ARRAY_BUFFER, c2GetBoxVBOInteger());
	glVertexAttribIPointer(0, 3, GL_INT, 6 * sizeof(GLint), (void*)0);
#else
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
#endif
	glEnableVertexAttribArray(0);
}
static glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);//FIXME
void c2PartLight::draw(const Render &Rr) {
	if (_bNonGizmos)
		return;
	if (!vao_lamp)
		_BuildVAOLight();
	lampShader.use();
	lampShader.setMat4("projection", Rr._MatProjection);
	lampShader.setMat4("view", Rr._MatView);
	glm::mat4 model(1.0f);//最好显性identiy！
	model = glm::mat4(1.0f);//最好显性identiy！
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
	lampShader.setMat4("model", model);
	glBindVertexArray(vao_lamp);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
