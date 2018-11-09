// ******************************************************************************
// Filename:  Frustum.h
// Project:   Vox
// Author:    Steven Ball
//
// Purpose:
//   A geometric implementation of the viewing frustum for a viewport.
//
// Revision History:
//   Initial Revision - 28/11/08
//
// Copyright (c) 2005-2015, Steven Ball
// ******************************************************************************

#pragma once

#include "../Maths/3dGeometry.h"
#include <glm/vec3.hpp>
using namespace glm;


class Frustum
{
public:
	Frustum();
	~Frustum();

	void SetFrustum(float angle, float ratio, float nearD, float farD);
	void SetCamera(const vec3 &pos, const vec3 &target, const vec3 &up);

	int PointInFrustum(const vec3 &point);
	int SphereInFrustum(const vec3 &point, float radius);
	int CubeInFrustum(const vec3 &center, float x, float y, float z);

public:
	enum {
		FRUSTUM_TOP = 0,
		FRUSTUM_BOTTOM,
		FRUSTUM_LEFT,
		FRUSTUM_RIGHT,
		FRUSTUM_NEAR,
		FRUSTUM_FAR,
	};

	enum {
		FRUSTUM_OUTSIDE = 0,
		FRUSTUM_INTERSECT,
		FRUSTUM_INSIDE,
	};

	Plane3D planes[6];

	vec3 nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
	vec3 farTopLeft, farTopRight, farBottomLeft, farBottomRight;

	float nearDistance, farDistance;
	float nearWidth, nearHeight;
	float farWidth, farHeight;
	float ratio, angle, tang;
};
