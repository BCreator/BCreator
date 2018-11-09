// ******************************************************************************
// Filename:    VoxelCharacter.h
// Project:     Vox
// Author:      Steven Ball
//
// Purpose:
//
// Revision History:
//   Initial Revision - 24/07/14
//
// Copyright (c) 2005-2015, Steven Ball
// ******************************************************************************

#pragma once

#include "modelloader.h"
#include "QubicleBinaryManager.h"


// Facial expression
typedef struct FacialExpression
{
	string m_facialExpressionName;
	string m_eyesTextureFile;
	string m_mouthTextureFile;
	unsigned int m_eyeTexture;
	unsigned int m_mouthTexture;
} FacialExpression;

// Talking animation
typedef struct TalkingAnimation
{
	string m_talkingAnimationTextureFile;
	unsigned int m_talkingAnimationTexture;
} TalkingAnimation;

class VoxelWeapon;

enum AnimationSections
{
	AnimationSections_FullBody = 0,
	AnimationSections_Head_Body,
	AnimationSections_Legs_Feet,
	AnimationSections_Left_Arm_Hand,
	AnimationSections_Right_Arm_Hand,
	AnimationSections_NUMSECTIONS,
};

class VoxelCharacter
{
public:
	/* Public methods */
	VoxelCharacter(Renderer* pRenderer, QubicleBinaryManager* pQubicleBinaryManager);
	~VoxelCharacter();

	void Reset();

	void LoadVoxelCharacter(const char* characterType, const char *qbFilename, const char *modelFilename, const char *animatorFilename, const char *facesFilename, const char* characterFilename, const char *charactersBaseFolder, bool useQubicleManager = true);
	void SaveVoxelCharacter(const char *qbFilename, const char *facesFilename, const char* characterFilename);
	void UnloadCharacter();

	// Rebuild
	void RebuildVoxelModel(bool faceMerge);

	// Faces
	bool LoadFaces(const char* characterType, const char *facesFileName, const char *charactersBaseFolder);
	bool SaveFaces(const char *facesFileName);
	void SetupFacesBones();
	void ModifyEyesTextures(const char *charactersBaseFolder, const char* characterType, const char* eyeTextureFolder);

	// Character file
	void LoadCharacterFile(const char* characterFilename);
	void SaveCharacterFile(const char* characterFilename);
	void ResetMatrixParamsFromCharacterFile(const char* characterFilename, const char* matrixToReset);

	// Character scale
	void SetCharacterScale(float scale);
	float GetCharacterScale();

	// Character alpha
	float GetCharacterAlpha();

	// Upper body and head tilt look, for camera rotation
	void SetHeadAndUpperBodyLookRotation(float lookRotationAngle, float zLookTranslate);
	float GetHeadAndUpperBodyLookRotation();
	float GetHeadAndUpperBodyLookzTranslate();

	void SetCharacterMatrixRenderParams(const char* matrixName, float scale, float xOffset, float yOffset, float zOffset);
	float GetBoneMatrixRenderScale(const char* matrixName);
	vec3 GetBoneMatrixRenderOffset(const char* matrixName);

	// Weapons
	void LoadRightWeapon(const char *weaponFilename);
	void LoadLeftWeapon(const char *weaponFilename);
	VoxelWeapon* GetRightWeapon();
	VoxelWeapon* GetLeftWeapon();
	void UnloadRightWeapon();
	void UnloadLeftWeapon();
	bool IsRightWeaponLoaded();
	bool IsLeftWeaponLoaded();

	// Setup animator and bones
	void SetUpdateAnimator(bool update);
	Matrix4x4 GetBoneMatrix(AnimationSections section, int index);
	Matrix4x4 GetBoneMatrix(AnimationSections section, const char* boneName);
	Matrix4x4 GetBoneMatrixPaperdoll(int index, bool left);
	int GetBoneIndex(const char* boneName);
	int GetMatrixIndexForName(const char* matrixName);
	MS3DModel* GetMS3DModel();
	MS3DAnimator* GetMS3DAnimator(AnimationSections section);
	QubicleBinary* GetQubicleModel();
	vec3 GetBoneScale();
	void SetBoneScale(float scale);

	// Rendering modes
	void SetWireFrameRender(bool wireframe);
	void SetRenderRightWeapon(bool render);
	void SetRenderLeftWeapon(bool render);
	void SetMeshAlpha(float alpha, bool force = false);
	void SetMeshSingleColour(float r, float g, float b);

	// Breathing animation
	void SetBreathingAnimationEnabled(bool enable);
	bool IsBreathingAnimationEnabled();
	bool IsBreathingAnimationStarted();
	void StartBreathAnimation();
	float GetBreathingAnimationOffsetForBone(int boneIndex);

	// Facial expressions
	int GetNumFacialExpressions() const;
	const char* GetFacialExpressionName(const int index);
	vec3 GetEyesOffset();
	vec3 GetMouthOffset();
	void SetEyesOffset(vec3 offset);
	void SetMouthOffset(vec3 offset);
	void PlayFacialExpression(const char *lFacialExpressionName);
	void PlayFacialExpression(int facialAnimationIndex);
	int GetCurrentFacialAnimation();
	void SetEyesBone(string eyesBoneName);
	void SetMouthBone(string mouthBoneName);
	string GetEyesBoneName();
	string GetMouthBoneName();
	int GetEyesBone();
	int GetMouthBone();
	int GetEyesMatrixIndex();
	int GetMouthMatrixIndex();
	void SetEyesTextureSize(float width, float height);
	void SetMouthTextureSize(float width, float height);
	float GetEyeTextureWidth();
	float GetEyeTextureHeight();
	float GetMouthTextureWidth();
	float GetMouthTextureHeight();

	// Face looking
	vec3 GetFaceLookingDirection();
	void SetFaceLookingDirection(vec3 looking);
	vec3 GetFaceTargetDirection();
	void SetFaceTargetDirection(vec3 target);
	float GetFaceLookToTargetSpeedMultiplier();
	void SetFaceLookToTargetSpeedMultiplier(float speedMultiplier);
	int GetHeadBoneIndex();
	int GetBodyBoneIndex();
	int GetLeftShoulderBoneIndex();
	int GetLeftHandBoneIndex();
	int GetRightShoulderBoneIndex();
	int GetRightHandBoneIndex();
	int GetLegsBoneIndex();
	int GetRightFootBoneIndex();
	int GetLeftFootBoneIndex();
	void SetRandomLookDirection(bool enable);
	bool IsRandomLookDirectionEnabled();

	// Wink animation
	void SetWinkAnimationEnabled(bool enable);
	bool IsWinkAnimationEnabled();
	void UpdateWinkAnimation(float dt);

	// Talking animation
	void SetTalkingAnimationEnabled(bool enable);
	bool IsTalkingAnimationEnabled();
	void SetRandomMouthSelection(bool random);
	bool IsRandomMouthSelectionEnabled();
	void UpdateTalkingAnimation(float dt);

	// Animations
	int GetNumAnimations();
	const char* GetAnimationName(int index);
	void PlayAnimation(AnimationSections section, bool waitForComplete, AnimationSections syncWithSection, const char *lAnimationName);
	int GetCurrentAnimationIndex(AnimationSections section);
	void SetBlendAnimation(AnimationSections section, bool waitForComplete, AnimationSections syncWithSection, const char *lStartAnimationName, const char *lEndAnimationName, float blendTime);
	void BlendIntoAnimation(AnimationSections section, bool waitForComplete, AnimationSections syncWithSection, const char *lAnimationName, float blendTime);
	bool HasAnimationFinished(AnimationSections section);
	bool HasAnimationLooped(AnimationSections section);
	void StepAnimationFrame(float dt);
	int GetStartFrame(const char *lAnimationName);
	int GetEndFrame(const char *lAnimationName);
	int GetCurrentFrame();
	int GetNumJoints();
	Joint* GetJoint(int index);
	Joint* GetJoint(const char* jointName);
	void PlayAnimationOnPaperDoll(const char *lAnimationName, bool left);

	// Matrices
	int GetNumModelMatrices();
	const char* GetModelMatrixName(int index);

	// Swapping and adding new matrices
	void SwapBodyPart(const char* bodyPartName, QubicleMatrix* pMatrix, bool copyMatrixParams);
	void AddQubicleMatrix(QubicleMatrix* pNewMatrix, bool copyMatrixParams);
	void RemoveQubicleMatrix(const char* matrixName);
	void SetQubicleMatrixRender(const char* matrixName, bool render);

	// Sub selection of individual body parts
	string GetSubSelectionName(int pickingId);

	// Update
	void Update(float dt, float animationSpeed[AnimationSections_NUMSECTIONS]);
	void SetWeaponTrailsOriginMatrix(float dt, Matrix4x4 originMatrix);

	// Rendering
	void Render(bool renderOutline, bool reflection, bool silhouette, Colour OutlineColour, bool subSelectionNamePicking);
	void RenderSubSelection(string subSelection, bool renderOutline, bool silhouette, Colour OutlineColour);
	void RenderBones();
	void RenderFace();
	void RenderFacingDebug();
	void RenderFaceTextures(bool eyesTexture, bool wireframe, bool transparency);
	void RenderWeapons(bool renderOutline, bool reflection, bool silhouette, Colour OutlineColour);
	void RenderWeaponTrails();
	void RenderPaperdoll();
	void RenderPortrait();
	void RenderFacePaperdoll();
	void RenderFacePortrait();
	void RenderWeaponsPaperdoll();

protected:
	/* Protected methods */
	static void _BreathAnimationFinished(void *apData);
	void BreathAnimationFinished();

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

	// Loaded flags
	bool m_loaded;
	bool m_loadedFaces;

	// FLag for updating the animator
	bool m_updateAnimator;

	// Flags to control weapon rendering
	bool m_renderRightWeapon;
	bool m_renderLeftWeapon;
	bool m_rightWeaponLoaded;
	bool m_leftWeaponLoaded;

	// Character Scale
	float m_characterScale;

	// Character alpha
	float m_characterAlpha;

	// Bone scale
	vec3 m_boneScale;

	// Tilt the character upper body and head, used for weapon rotation with camera look
	float m_lookRotationAngle;
	float m_zLookTranslate;

	// Breathing animation
	bool m_bBreathingAnimationEnabled;
	bool m_bBreathingAnimationStarted;
	float m_breathingBodyYOffset;
	float m_breathingHandsYOffset;
	float m_breathingAnimationInitialWaitTime;

	// Facial expression	
	int m_numFacialExpressions;
	FacialExpression *m_pFacialExpressions;
	unsigned int m_faceEyesTexture;
	unsigned int m_faceMouthTexture;
	vec3 m_eyesOffset;
	vec3 m_mouthOffset;
	int m_currentFacialExpression;
	int m_eyesBone;
	int m_mouthBone;
	int m_eyesMatrixIndex;
	int m_mouthMatrixIndex;
	float m_eyesTextureWidth;
	float m_eyesTextureHeight;
	float m_mouthTextureWidth;
	float m_mouthTextureHeight;

	// Face looking
	vec3 m_faceLookingDirection;
	vec3 m_faceTargetDirection;
	float m_faceLookToTargetSpeedMultiplier;
	int m_headBoneIndex;
	int m_bodyBoneIndex;
	int m_leftShoulderBoneIndex;
	int m_leftHandBoneIndex;
	int m_rightShoulderBoneIndex;
	int m_rightHandBoneIndex;
	int m_legsBoneIndex;
	int m_rightFootBoneIndex;
	int m_leftFootBoneIndex;
	bool m_bRandomLookDirectionEnabled;

	// Wink animation
	bool m_bWinkAnimationEnabled;
	string m_winkTextureFilename;
	unsigned int m_faceEyesWinkTexture;
	bool m_wink;
	float m_winkWaitTimer;
	float m_winkStayTime;
	string m_eyesBoneName;
	string m_mouthBoneName;

	// Talking animation
	bool m_bTalkingAnimationEnabled;
	int m_numTalkingMouths;
	TalkingAnimation *m_pTalkingAnimations;
	int m_currentTalkingTexture;
	float m_talkingWaitTimer;
	float m_talkingWaitTime;
	bool m_randomMouthSelection;
	float m_talkingPauseTimer;
	float m_talkingPauseTime;
	int m_talkingPauseMouthCounter;
	int m_talkingPauseMouthAmount;

	// If we are using the qubicle manager we don't need to delete our QB after use
	bool m_usingQubicleManager;

	// Weapons
	VoxelWeapon* m_pRightWeapon;
	VoxelWeapon* m_pLeftWeapon;

	QubicleBinary* m_pVoxelModel;
	MS3DModel* m_pCharacterModel;
	MS3DAnimator* m_pCharacterAnimator[AnimationSections_NUMSECTIONS];
	MS3DAnimator* m_pCharacterAnimatorPaperdoll_Left;
	MS3DAnimator* m_pCharacterAnimatorPaperdoll_Right;

	int m_currentFrame;
};
