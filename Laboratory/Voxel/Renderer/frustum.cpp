// ******************************************************************************
// Filename:  Frustum.cpp
// Project:   Vox
// Author:    Steven Ball
//
// Revision History:
//   Initial Revision - 28/11/08
//
// Copyright (c) 2005-2015, Steven Ball
// ******************************************************************************

#include "frustum.h"

#include <cmath>
#include <glm/glm.hpp>


Frustum::Frustum()
{
	/* Nothing */
}

Frustum::~Frustum()
{
	/* Nothing */
}

void Frustum::SetFrustum(float angle, float ratio, float nearD, float farD)
{
	this->ratio = ratio;
	this->angle = angle;
	this->nearDistance = nearD;
	this->farDistance = farD;

	tang = (float)tan(DegToRad(angle) * 0.5) ;
	nearHeight = nearDistance * tang;
	nearWidth = nearHeight * ratio; 
	farHeight = farDistance  * tang;
	farWidth = farHeight * ratio;
}

void Frustum::SetCamera(const vec3 &pos, const vec3 &target, const vec3 &up)
{
	vec3 dir, nc, fc, X, Y, Z;

	Z = pos - target;
	Z = normalize(Z);

	X = cross(up, Z);
	X = normalize(X);

	Y = cross(Z, X);

	nc = pos - Z * nearDistance;
	fc = pos - Z * farDistance;

	nearTopLeft = nc + Y * nearHeight - X * nearWidth;
	nearTopRight = nc + Y * nearHeight + X * nearWidth;
	nearBottomLeft = nc - Y * nearHeight - X * nearWidth;
	nearBottomRight = nc - Y * nearHeight + X * nearWidth;

	farTopLeft = fc + Y * farHeight - X * farWidth;
	farTopRight = fc + Y * farHeight + X * farWidth;
	farBottomLeft = fc - Y * farHeight - X * farWidth;
	farBottomRight = fc - Y * farHeight + X * farWidth;

	planes[FRUSTUM_TOP] = Plane3D(nearTopRight, nearTopLeft, farTopLeft);
	planes[FRUSTUM_BOTTOM] = Plane3D(nearBottomLeft, nearBottomRight, farBottomRight);
	planes[FRUSTUM_LEFT] = Plane3D(nearTopLeft, nearBottomLeft, farBottomLeft);
	planes[FRUSTUM_RIGHT] = Plane3D(nearBottomRight, nearTopRight, farBottomRight);
	planes[FRUSTUM_NEAR] = Plane3D(nearTopLeft, nearTopRight, nearBottomRight);
	planes[FRUSTUM_FAR] = Plane3D(farTopRight, farTopLeft, farBottomLeft);
}

int Frustum::PointInFrustum(const vec3 &point)
{
	int result = FRUSTUM_INSIDE;

	for(int i = 0; i < 6; i++)
	{
		if (planes[i].GetPointDistance(point) < 0)
		{
			return FRUSTUM_OUTSIDE;
		}
	}

	return(result);
}

int Frustum::SphereInFrustum(const vec3 &point, float radius)
{
	int result = FRUSTUM_INSIDE;
	float distance;

	for(int i = 0; i < 6; i++)
	{
		distance = planes[i].GetPointDistance(point);

		if (distance < -radius)
		{
			return FRUSTUM_OUTSIDE;
		}
		else if (distance < radius)
		{
			result =  FRUSTUM_INTERSECT;
		}
	}

	return(result);
}
int Frustum::CubeInFrustum(const vec3 &center, float x, float y, float z)
{
	int result = FRUSTUM_INSIDE;

	for(int i = 0; i < 6; i++)
	{
		// Reset counters for corners in and out
		int out = 0;
		int in = 0;

		if (planes[i].GetPointDistance(center + vec3(-x, -y, -z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (planes[i].GetPointDistance(center + vec3(x, -y, -z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (planes[i].GetPointDistance(center + vec3(-x, -y, z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (planes[i].GetPointDistance(center + vec3(x, -y, z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (planes[i].GetPointDistance(center + vec3(-x, y, -z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (planes[i].GetPointDistance(center + vec3(x, y, -z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (planes[i].GetPointDistance(center + vec3(-x, y, z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (planes[i].GetPointDistance(center + vec3(x, y, z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		// If all corners are out
		if(!in)
		{
			return FRUSTUM_OUTSIDE;
		}
		// If some corners are out and others are in	
		else if(out)
		{
			result = FRUSTUM_INTERSECT;
		}
	}

	return(result);
}
