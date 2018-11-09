// ******************************************************************************
// Filename:    BoundingRegion.h
// Project:     Vox
// Author:      Steven Ball
//
// Purpose:
//
// Revision History:
//   Initial Revision - 20/03/15
//
// Copyright (c) 2005-2016, Steven Ball
// ******************************************************************************

#pragma once

#include "3dmaths.h"

#include <vector>
using namespace std;


enum BoundingRegionType
{
	BoundingRegionType_Sphere = 0,
	BoundingRegionType_Cube,
};

class BoundingRegion
{
public:
	/* Public methods */
	BoundingRegion() { /* Nothing */ }
	~BoundingRegion() { /* Nothing */ }

	void UpdatePlanes(Matrix4x4 transformationMatrix, float scale)
	{
		m_planes[0] = Plane3D(transformationMatrix * (vec3(-1.0f, 0.0f, 0.0f) * scale), transformationMatrix * (vec3(m_x_length * m_scale, 0.0f, 0.0f) * scale));
		m_planes[1] = Plane3D(transformationMatrix * (vec3(1.0f, 0.0f, 0.0f) * scale), transformationMatrix * (vec3(-m_x_length * m_scale, 0.0f, 0.0f) * scale));
		m_planes[2] = Plane3D(transformationMatrix * (vec3(0.0f, -1.0f, 0.0f) * scale), transformationMatrix * (vec3(0.0f, m_y_length * m_scale, 0.0f) * scale));
		m_planes[3] = Plane3D(transformationMatrix * (vec3(0.0f, 1.0f, 0.0f) * scale), transformationMatrix * (vec3(0.0f, -m_y_length * m_scale, 0.0f) * scale));
		m_planes[4] = Plane3D(transformationMatrix * (vec3(0.0f, 0.0f, -1.0f) * scale), transformationMatrix * (vec3(0.0f, 0.0f, m_z_length * m_scale) * scale));
		m_planes[5] = Plane3D(transformationMatrix * (vec3(0.0f, 0.0f, 1.0f) * scale), transformationMatrix * (vec3(0.0f, 0.0f, -m_z_length * m_scale) * scale));
	}

	void Render(Renderer* pRenderer)
	{
		if(m_boundingType == BoundingRegionType_Sphere)
		{
			pRenderer->PushMatrix();
				pRenderer->SetLineWidth(1.0f);
				pRenderer->ImmediateColourAlpha(1.0f, 1.0f, 1.0f, 0.25f);
				pRenderer->SetRenderMode(RM_WIREFRAME);

				pRenderer->TranslateWorldMatrix(m_origin.x, m_origin.y, m_origin.z);

				pRenderer->ScaleWorldMatrix(m_scale, m_scale, m_scale);

				pRenderer->DrawSphere(m_radius, 20, 20);
			pRenderer->PopMatrix();
		}

		if(m_boundingType == BoundingRegionType_Cube)
		{
			pRenderer->PushMatrix();
				pRenderer->SetLineWidth(1.0f);
				pRenderer->ImmediateColourAlpha(1.0f, 1.0f, 1.0f, 0.25f);
				pRenderer->SetRenderMode(RM_WIREFRAME);
				pRenderer->SetCullMode(CM_NOCULL);

				pRenderer->ScaleWorldMatrix(m_scale, m_scale, m_scale);

				pRenderer->TranslateWorldMatrix(m_origin.x, m_origin.y, m_origin.z);

				float l_length = m_x_length;
				float l_height = m_y_length;
				float l_width = m_z_length;

				pRenderer->EnableImmediateMode(IM_QUADS);
					pRenderer->ImmediateNormal(0.0f, 0.0f, -1.0f);
					pRenderer->ImmediateVertex(l_length, -l_height, -l_width);
					pRenderer->ImmediateVertex(-l_length, -l_height, -l_width);
					pRenderer->ImmediateVertex(-l_length, l_height, -l_width);
					pRenderer->ImmediateVertex(l_length, l_height, -l_width);

					pRenderer->ImmediateNormal(0.0f, 0.0f, 1.0f);
					pRenderer->ImmediateVertex(-l_length, -l_height, l_width);
					pRenderer->ImmediateVertex(l_length, -l_height, l_width);
					pRenderer->ImmediateVertex(l_length, l_height, l_width);
					pRenderer->ImmediateVertex(-l_length, l_height, l_width);

					pRenderer->ImmediateNormal(1.0f, 0.0f, 0.0f);
					pRenderer->ImmediateVertex(l_length, -l_height, l_width);
					pRenderer->ImmediateVertex(l_length, -l_height, -l_width);
					pRenderer->ImmediateVertex(l_length, l_height, -l_width);
					pRenderer->ImmediateVertex(l_length, l_height, l_width);

					pRenderer->ImmediateNormal(-1.0f, 0.0f, 0.0f);
					pRenderer->ImmediateVertex(-l_length, -l_height, -l_width);
					pRenderer->ImmediateVertex(-l_length, -l_height, l_width);
					pRenderer->ImmediateVertex(-l_length, l_height, l_width);
					pRenderer->ImmediateVertex(-l_length, l_height, -l_width);

					pRenderer->ImmediateNormal(0.0f, -1.0f, 0.0f);
					pRenderer->ImmediateVertex(-l_length, -l_height, -l_width);
					pRenderer->ImmediateVertex(l_length, -l_height, -l_width);
					pRenderer->ImmediateVertex(l_length, -l_height, l_width);
					pRenderer->ImmediateVertex(-l_length, -l_height, l_width);

					pRenderer->ImmediateNormal(0.0f, 1.0f, 0.0f);
					pRenderer->ImmediateVertex(l_length, l_height, -l_width);
					pRenderer->ImmediateVertex(-l_length, l_height, -l_width);
					pRenderer->ImmediateVertex(-l_length, l_height, l_width);
					pRenderer->ImmediateVertex(l_length, l_height, l_width);
				pRenderer->DisableImmediateMode();

				pRenderer->SetCullMode(CM_BACK);
			pRenderer->PopMatrix();
		}
	}

protected:
	/* Protected methods */

private:
	/* Private methods */

public:
	/* Public members */
	BoundingRegionType m_boundingType;
	vec3 m_origin;
	float m_radius;
	float m_x_length;
	float m_y_length;
	float m_z_length;
	float m_scale;
	Plane3D m_planes[6];

protected:
	/* Protected members */

private:
	/* Private members */
};

typedef vector<BoundingRegion*> BoundingRegionList;
