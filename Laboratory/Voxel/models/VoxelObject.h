// ******************************************************************************
// Filename:    VoxelObject.h
// Project:     Vox
// Author:      Steven Ball
//
// Purpose:
//
// Revision History:
//   Initial Revision - 11/08/14
//
// Copyright (c) 2005-2015, Steven Ball
// ******************************************************************************

#pragma once


#include "modelloader.h"
#include "QubicleBinaryManager.h"


class VoxelObject
{
public:
	/* Public methods */
	VoxelObject();
	~VoxelObject();

	void SetRenderer(Renderer* pRenderer);
	void SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager);

	void Reset();

	// Rebuild
	void RebuildVoxelModel(bool faceMerge);

	QubicleBinary* GetQubicleModel();
	Matrix4x4 GetModelMatrix(int qubicleMatrixIndex);

	vec3 GetCenter();

	void LoadObject(const char *qbFilename, bool useManager = true);
	void UnloadObject();

	// Rendering modes
	void SetWireFrameRender(bool wireframe);
	void SetMeshAlpha(float alpha);
	void SetMeshSingleColour(float r, float g, float b);

	void Update(float dt);
	void Render(bool renderOutline, bool reflection, bool silhouette, Colour OutlineColour);

protected:
	/* Protected methods */

private:
	/* Private methods */

public:
	/* Public members */

protected:
	/* Protected members */

private:
	/* Private members */
	Renderer* m_pRenderer;
	QubicleBinaryManager* m_pQubicleBinaryManager;

	// Loaded flag
	bool m_loaded;

	// If we are using the qubicle manager we don't need to delete our QB after use
	bool m_usingQubicleManager;

	// The qubicle binary data
	QubicleBinary* m_pVoxelModel;
};
