// ******************************************************************************
// Filename:  FrameBuffer.h
// Project:   Vox
// Author:    Steven Ball
//
// Purpose:
//   A frame buffer object, used to store the different g-buffer states of a
//   viewport.
//
// Revision History:
//   Initial Revision - 16/10/15
//
// Copyright (c) 2005-2015, Steven Ball
// ******************************************************************************

#pragma once

class FrameBuffer
{
public:
	string m_name;
	unsigned int m_diffuseTexture;
	unsigned int m_positionTexture;
	unsigned int m_normalTexture;
	unsigned int m_depthTexture;
	int m_width;
	int m_height;
	float m_viewportScale;
	GLuint m_fbo;
};
