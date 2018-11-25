struct Render {//����ز�ס���ñ�¶���û��Ļ����͵ø���Ϊc2Render�������ļ�����Ŀ¼��������Ҫ������Ŀ����װ
	glm::mat4 view, projection;
	void update(double dElapsed) {
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
		projection = glm::perspective(glm::radians(camera.Zoom),
			(float)g_nWindWidth / (float)g_nWindHeight, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
	}

	void update(double dElapsed);
};
