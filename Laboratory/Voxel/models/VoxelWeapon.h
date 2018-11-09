// ******************************************************************************
// Filename:    VoxelWeapon.h
// Project:     Vox
// Author:      Steven Ball
//
// Purpose:
//
// Revision History:
//   Initial Revision - 12/08/14
//
// Copyright (c) 2005-2015, Steven Ball
// ******************************************************************************

#pragma once


#include "modelloader.h"
#include "QubicleBinaryManager.h"

class VoxelObject;


class VoxelWeaponLight
{
public:
	unsigned int m_lightId;
	vec3 m_lightOffset;
	float m_lightRadius;
	float m_lightDiffuseMultiplier;
	Colour m_lightColour;
	int m_connectedToSectionIndex;

	vec3 m_lightPosition;
};

class AnimatedSection
{
public:
	string m_fileName;
	VoxelObject* m_pVoxelObject;
	float m_renderScale;
	vec3 m_renderOffset;

	bool m_autoStart;
	bool m_playingAnimation;
	bool m_loopingAnimation;

	// Animated parts
	float m_translateSpeedX;
	float m_translateSpeedY;
	float m_translateSpeedZ;
	float m_translateRangeXMin;
	float m_translateRangeXMax;
	float m_translateRangeYMin;
	float m_translateRangeYMax;
	float m_translateRangeZMin;
	float m_translateRangeZMax;
	float m_translateSpeedTurnSpeedX;
	float m_translateSpeedTurnSpeedY;
	float m_translateSpeedTurnSpeedZ;

	vec3 m_rotationPoint;
	float m_rotationSpeedX;
	float m_rotationSpeedY;
	float m_rotationSpeedZ;
	float m_rotationRangeXMin;
	float m_rotationRangeXMax;
	float m_rotationRangeYMin;
	float m_rotationRangeYMax;
	float m_rotationRangeZMin;
	float m_rotationRangeZMax;
	float m_rotationSpeedTurnSpeedX;
	float m_rotationSpeedTurnSpeedY;
	float m_rotationSpeedTurnSpeedZ;

	vec3 m_animatedSectionPosition;

	float m_translateX;
	float m_translateY;
	float m_translateZ;
	float m_translateMaxSpeedX;
	float m_translateMaxSpeedY;
	float m_translateMaxSpeedZ;
	bool m_translateXUp;
	bool m_translateXDown;
	bool m_translateYUp;
	bool m_translateYDown;
	bool m_translateZUp;
	bool m_translateZDown;

	float m_rotationX;
	float m_rotationY;
	float m_rotationZ;
	float m_rotationMaxSpeedX;
	float m_rotationMaxSpeedY;
	float m_rotationMaxSpeedZ;
	bool m_rotationXUp;
	bool m_rotationXDown;
	bool m_rotationYUp;
	bool m_rotationYDown;
	bool m_rotationZUp;
	bool m_rotationZDown;
};

class ParticleEffect
{
public:
	unsigned int m_particleEffectId;
	string m_fileName;
	vec3 m_positionOffset;
	int m_connectedToSectionIndex;

	vec3 m_particleEffectPosition;
};

class WeaponTrailPoint
{
public:
	vec3 m_startPoint;
	vec3 m_endPoint;
	bool m_pointActive;
	float m_animaionTime;
};

class WeaponTrail
{
public:
	float m_trailTime;
	vec3 m_startOffsetPoint;
	vec3 m_endOffsetPoint;
	Colour m_trailColour;
	bool m_followOrigin;

	int m_numTrailPoints;
	int m_trailNextAddIndex;
	WeaponTrailPoint* m_pTrailPoints;
	Matrix4x4 m_origin;
	float m_parentScale;
};


class VoxelWeapon
{
public:
	/* Public methods */
	VoxelWeapon(Renderer* pRenderer, QubicleBinaryManager* pQubicleBinaryManager);
	~VoxelWeapon();

	void Reset();

	bool IsLoaded();

	// Rebuild
	void RebuildVoxelModel(bool faceMerge);

	void LoadWeapon(const char *weaponFilename, bool useManager = true);
	void SaveWeapon(const char *weaponFilename);
	void UnloadWeapon();

	void SetVoxelCharacterParent(VoxelCharacter* pParentCharacter);
	void SetBoneAttachment(const char* boneName);

	void SetRenderOffset(vec3 offset);
	vec3 GetRenderOffset();

	void SetRenderScale(float scale);
	float GetRenderScale();

	vec3 GetCenter();

	// Subsection animations
	void StartSubSectionAnimation();
	void StopSubSectionAnimation();
	bool HasSubSectionAnimationFinished(int index);

	// Weapon trails
	void StartWeaponTrails();
	void StopWeaponTrails();
	bool IsWeaponTrailsActive();

	// Lighting
	int GetNumLights();
	void SetLightingId(int lightIndex, unsigned int lightId);
	void GetLightParams(int lightIndex, unsigned int *lightId, vec3 *position, float *radius, float *diffuseMultiplier, Colour *colour, bool *connectedToSegment);

	// Particle effects
	int GetNumParticleEffects();
	void SetParticleEffectId(int particleEffectIndex, unsigned int particleEffectId);
	void GetParticleEffectParams(int particleEffectIndex, unsigned int *particleEffectId, vec3 *position, string* name, bool *connectedToSegment);

	// Animated sections
	int GetNumAimatedSections();
	AnimatedSection* GetAnimatedSection(int index);

	// Gameplay params
	float GetWeaponRadius();
	void SetFirstPersonMode(bool firstPerson);

	// Camera settings
	void SetCameraYRotation(float yRot);

	// Rendering modes
	void SetWireFrameRender(bool wireframe);
	void SetMeshAlpha(float alpha);
	void SetMeshSingleColour(float r, float g, float b);

	// Updating
	void SetWeaponTrailsParams(Matrix4x4 originMatrix, float scale);
	void CreateWeaponTrailPoint();
	void Update(float dt);

	// Rendering
	void Render(bool renderOutline, bool reflection, bool silhouette, Colour OutlineColour);
	void RenderPaperdoll();
	void RenderWeaponTrails();

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

	// Parent character we are connected to
	VoxelCharacter* m_pParentCharacter;

	// Bone index for attachment to character
	int m_boneIndex;

	// Matrix information for attachment to character
	string m_matrixName;
	int m_matrixIndex;

	// Render offset from center
	vec3 m_renderOffset;

	// Weapon scale
	float m_renderScale;

	// Animated sections
	int m_numAnimatedSections;
	AnimatedSection* m_pAnimatedSections;

	// Lighting on weapons
	int m_numLights;
	VoxelWeaponLight* m_pLights;

	// Particle effects
	int m_numParticleEffects;
	ParticleEffect* m_pParticleEffects;

	// Weapon trails
	int m_numWeaponTrails;
	WeaponTrail* m_pWeaponTrails;
	bool m_weaponTrailsStarted;

	// Gameplay params
	float m_weaponRadius;
	bool m_firstPersonMode;

	// Camera variables
	float m_cameraYRotation;
};
