#pragma once

//1////////////////////////////////////////////////////////////////////////////
struct Render {//����ز�ס�ϲ��û����Կ�������ô�͵ø���Ϊc2Render����Ҳ��Ҫ������ķ�װ��
	const Camera*	_pCamera;
	int				_nWidth, _nHeight;
	glm::mat4		_MatProjection, _MatView;
	glm::vec3		_PosView;

	Render(const Camera& TheCamera, int nWidth, int nHeight);	
	void onSize(int nWidth, int nHeight);
	void update(double dElapsed);
};


//1////////////////////////////////////////////////////////////////////////////
/*standard geometry vbo for gizmos and so on*/
int c2GetBoxVBOFloat();// size 1.0f
