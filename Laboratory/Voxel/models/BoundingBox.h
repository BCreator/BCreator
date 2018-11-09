#pragma once

#include <glm/vec3.hpp>
using namespace glm;


class BoundingBox
{
public:
	BoundingBox();
	~BoundingBox();

	float GetWidth() const;
	float GetLength() const;
	float GetHeight() const;

	vec3 GetCenter() const;

	float GetMinX() const;
	float GetMinY() const;
	float GetMinZ() const;

	float GetMaxX() const;
	float GetMaxY() const;
	float GetMaxZ() const;

public:
	float mMinX;
	float mMinY;
	float mMinZ;

	float mMaxX;
	float mMaxY;
	float mMaxZ;
};