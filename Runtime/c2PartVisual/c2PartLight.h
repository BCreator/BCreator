#pragma once

#include"../c2Foundation/c2Part.h"

//1/////////////////////////////////////////////////////////////////////////////
struct Render;
struct c2PartLight : public c2Part {
	bool _bNonGizmos = false;
	glm::vec3 _Pos;//TODO
	void draw(const Render &Rr);
};
