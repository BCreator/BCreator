// ******************************************************************************
// Filename:    Bezier3.cpp
// Project:     Vox
// Author:      Steven Ball
//
// Purpose:
//   Bezier Curve with 3 points - 1 control point.
//
// Revision History:
//   Initial Revision - 03/08/08
//
// Copyright (c) 2005-2016, Steven Ball
// ******************************************************************************

#include "3dGeometry.h"


// Constructors
Bezier3::Bezier3()
{
}

Bezier3::Bezier3(vec3 lStart, vec3 lEnd, vec3 lControl)
{
	mStartPoint = lStart;
	mEndPoint = lEnd;
	mControlPoint = lControl;
}

Bezier3::Bezier3(float xStart, float yStart, float zStart, float xEnd, float yEnd, float zEnd, float xControl, float yControl, float zControl)
{
	mStartPoint = vec3(xStart, yStart, zStart);
	mEndPoint = vec3(xEnd, yEnd, zEnd);
	mControlPoint = vec3(xControl, yControl, zControl);
}

// Operations
const vec3 Bezier3::GetInterpolatedPoint(float t) const
{
	float iT = 1.0f - t;
	float b0 = iT * iT;
	float b1 = 2 * t * iT;
	float b2 = t * t;

	float lx = mStartPoint.x * b0 + mControlPoint.x * b1 + mEndPoint.x * b2;
	float ly = mStartPoint.y * b0 + mControlPoint.y * b1 + mEndPoint.y * b2;
	float lz = mStartPoint.z * b0 + mControlPoint.z * b1 + mEndPoint.z * b2;

	return vec3(lx, ly, lz);
}
