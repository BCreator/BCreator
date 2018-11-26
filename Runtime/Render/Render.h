#pragma once

//1////////////////////////////////////////////////////////////////////////////
struct Render {//如果藏不住上层用户可以看到，那么就得改名为c2Render。但也不要无意义的封装。
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
