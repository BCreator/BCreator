// ******************************************************************************
// Filename:    Plane3D.cpp
// Project:     Vox
// Author:      Steven Ball
//
// Purpose:
//   3D Plane implementation.
//
// Revision History:
//   Initial Revision - 28/11/08
//
// Copyright (c) 2005-2016, Steven Ball
// ******************************************************************************

#include "3dGeometry.h"
#include <glm/glm.hpp>
#include <glm/detail/func_geometric.hpp>
using namespace glm;


// Constructors
Plane3D::Plane3D()
{
	d = 0.0f;
}

Plane3D::Plane3D(vec3 lNormal, vec3 lPoint)
{
	mNormal = lNormal;
	mPoint = lPoint;

	mNormal = normalize(mNormal);
	d = -(dot(mNormal, mPoint));
}

Plane3D::Plane3D(vec3 lv1, vec3 lv2, vec3 lv3)
{
	vec3 aux1;
	vec3 aux2;

	aux1 = lv1 - lv2;
	aux2 = lv3 - lv2;

	mNormal = cross(aux2, aux1);

	mNormal = normalize(mNormal);
	mPoint = lv2;

	d = -(dot(mNormal, mPoint));
}

Plane3D::Plane3D(float a, float b, float c, float d)
{
	// Set the normal vector
	mNormal = vec3(a, b, c);

	// Compute the length of the vector
	float lLength = length(mNormal);
	
	// Normalize the vector
	mNormal = vec3(a / lLength, b / lLength, c / lLength);

	// And divide d by the length as well
	this->d = d / lLength;
}


// Operations
float Plane3D::GetPointDistance(vec3 lPoint)
{
	return (d + dot(mNormal, lPoint));
}
