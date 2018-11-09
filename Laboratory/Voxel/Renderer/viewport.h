// ******************************************************************************
// Filename:  Viewport.h
// Project:   Vox
// Author:    Steven Ball
//
// Purpose:
//   A viewport, used by the opengl renderer to hold viewport information.
//
// Revision History:
//   Initial Revision - 26/03/06
//
// Copyright (c) 2005-2015, Steven Ball
// ******************************************************************************

#pragma once

class Viewport {
public:
	int Bottom, Left, Width, Height;
	float Fov, Aspect;
	Matrix4x4 Perspective, Orthographic, Projection2d;
};