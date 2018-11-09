// ******************************************************************************
// Filename:  camera.h
// Project:   Vox
// Author:    Steven Ball
//
// Purpose:
//   A quaternion based camera class that encapsulates camera and looking
//   functionality. Ability to move and rotate freely with 6 degrees of freedom.
//   Allows for movement, rotation (including arcball rotation around a point),
//   zooming and strafing along the camera's right vector.
//
// Revision History:
//   Initial Revision - 03/11/15
//
// Copyright (c) 2005-2015, Steven Ball
// ******************************************************************************

#pragma once

#include <glm/vec3.hpp>
using namespace glm;

class Renderer;


class Camera {
public:
	Camera(Renderer* pRenderer);

	// Set/Get
	void SetPosition(const vec3 &position) { m_position = position; }
	void SetFakePosition(const vec3 &fakePosition) { m_fakePosition = fakePosition; }
	void SetFacing(const vec3 &facing) { m_facing = facing; }
	void SetUp(const vec3 &up) { m_up = up; }
	void SetRight(const vec3 &right) { m_right = right; }
	const void SetZoomAmount(float amount) { m_zoomAmount = amount; }
	const vec3 GetPosition() const { return m_position; }
	const vec3 GetFakePosition() const { return m_fakePosition; }
	const vec3 GetFacing()const { return vec3(m_facing.x, m_facing.y, m_facing.z); }
	const vec3 GetUp() const { return m_up; }
	const vec3 GetRight() const { return m_right; }
	const vec3 GetView() const { return (m_position + (m_facing*m_zoomAmount)); }
	const float GetZoomAmount() const { return m_zoomAmount; }

	// Camera movement
	void Fly(const float speed, bool useFakePosition = false);
	void Move(const float speed, bool useFakePosition = false);
	void Levitate(const float speed, bool useFakePosition = false);
	void Strafe(const float speed, bool useFakePosition = false);
	void Rotate(const float xAmount, const float yAmount, const float zAmount);
	void RotateY(const float yAmount);
	void RotateAroundPoint(const float xAmount, const float yAmount, const float zAmount, bool useFakePosition = false);
	void RotateAroundPointY(const float yAmount, bool useFakePosition = false);
	void Zoom(const float amount, bool useFakePosition = false);

	// Viewing
	void Look() const;

private:
	Renderer *m_pRenderer;

	// The camera's world position
	vec3 m_position;

	// 
	vec3 m_fakePosition;

	// Local up vector
	vec3 m_up;
	
	// Local facing vector
	vec3 m_facing;
	
	// Local right vector
	vec3 m_right;

	// Zoom values used when rotating around a point, zoom is essentially how much projection into the
	// facing direction we are looking at, since facing is a local unit vector relative to the camera.
	float m_zoomAmount;
	float m_minZoomAmount;
	float m_maxZoomAmount;
};
