// ******************************************************************************
// Filename:    3dGeometry.cpp
// Project:     Vox
// Author:      Steven Ball
//
// Purpose:
//   3d geometry functionality.
//
// Revision History:
//   Initial Revision - 03/08/08
//
// Copyright (c) 2005-2016, Steven Ball
// ******************************************************************************

#pragma once

#include "3dmaths.h"
#include <glm/vec3.hpp>
using namespace glm;


class Plane3D
{
public:
	// Constructors
	Plane3D();
	Plane3D(vec3 lNormal, vec3 lPoint);
	Plane3D(vec3 lv1, vec3 lv2, vec3 lv3);
	Plane3D(float a, float b, float c, float d);

	// Operations
	float GetPointDistance(vec3 lPoint);

public:
	vec3 mPoint;
	vec3 mNormal;
	float d;
};


class Line3D
{
public:
	// Constructors
	Line3D();
	Line3D(vec3 lStart, vec3 lEnd);
	Line3D(float x1, float y1, float z1, float x2, float y2, float z2);

	// Properties
	const vec3 GetMidPoint() const;
	const vec3 GetVector() const;
	const float GetLength() const;
	const float GetLengthSquared() const;

	// Operations
	const vec3 GetInterpolatedPoint(float t) const;

public:
	vec3 mStartPoint;
	vec3 mEndPoint;
};


class Bezier3
{
public:
	// Constructors
	Bezier3();
	Bezier3(vec3 lStart, vec3 lEnd, vec3 lControl);
	Bezier3(float xStart, float yStart, float zStart, float xEnd, float yEnd, float zEnd, float xControl, float yControl, float zControl);

	// Operations
	const vec3 GetInterpolatedPoint(float t) const;

public:
	vec3 mStartPoint;
	vec3 mEndPoint;
	vec3 mControlPoint;
};


class Bezier4
{
public:
	// Constructors
	Bezier4();
	Bezier4(vec3 lStart, vec3 lEnd, vec3 lControl1, vec3 lControl2);
	Bezier4(float xStart, float yStart, float zStart, float xEnd, float yEnd, float zEnd, float xControl1, float yControl1, float zControl1, float xControl2, float yControl2, float zControl2);

	// Operations
	const vec3 GetInterpolatedPoint(float t) const;

public:
	vec3 mStartPoint;
	vec3 mEndPoint;
	vec3 mControlPoint1;
	vec3 mControlPoint2;
};
