// ******************************************************************************
// Filename:    Player.h
// Project:     Vox
// Author:      Steven Ball
//
// Purpose:
//   The player class contains all the functioanlity of a player, an entity that
//   is controlled and can act within the world and game.
// 
// Revision History:
//   Initial Revision - 27/10/15
//
// Copyright (c) 2005-2016, Steven Ball
// ******************************************************************************

#pragma once

//#include "PlayerStats.h"
//#include "PlayerClass.h"
#include "../Renderer/Renderer.h"
#include "../blocks/ChunkManager.h"
#include "../models/VoxelCharacter.h"
//#include "../Lighting/LightingManager.h"
//#include "../Particles/BlockParticleManager.h"
//#include "../TextEffects/TextEffectsManager.h"
//#include "../Inventory/InventoryManager.h"
//#include "../Items/ItemManager.h"
//#include "../Projectile/ProjectileManager.h"
//#include "../Enemy/EnemyManager.h"
//#include "../Enemy/Enemy.h"

//class InventoryGUI;
//class CharacterGUI;
//class CraftingGUI;
//class LootGUI;
//class ActionBar;

static const int PlayerEquippedProperties_None = 0;
static const int PlayerEquippedProperties_Sword = 1;
static const int PlayerEquippedProperties_Axe = 2;
static const int PlayerEquippedProperties_Hammer = 4;
static const int PlayerEquippedProperties_Mace = 8;
static const int PlayerEquippedProperties_Dagger = 16;
static const int PlayerEquippedProperties_Sickle = 32;
static const int PlayerEquippedProperties_2HandedSword = 64;
static const int PlayerEquippedProperties_Shield = 128;
static const int PlayerEquippedProperties_Boomerang = 256;
static const int PlayerEquippedProperties_Bow = 512;
static const int PlayerEquippedProperties_Staff = 1024;
static const int PlayerEquippedProperties_Wand = 2048;
static const int PlayerEquippedProperties_Pickaxe = 4096;
static const int PlayerEquippedProperties_Torch = 8192;
static const int PlayerEquippedProperties_PlacementItem = 16384;
static const int PlayerEquippedProperties_PlacementScenery = 32768;
static const int PlayerEquippedProperties_PlacementBlock = 65536;
static const int PlayerEquippedProperties_Consumable = 131072;
static const int PlayerEquippedProperties_Bomb = 262144;
static const int PlayerEquippedProperties_SpellHands = 524288;

class LightingManager;
class BlockParticleManager;
class Player
{
public:
	/* Public methods */
	Player(Renderer* pRenderer, ChunkManager* pChunkManager, QubicleBinaryManager* pQubicleBinaryManager, LightingManager* pLightingManager, BlockParticleManager* pBlockParticleManager);
	~Player();

	// Linkage
// 	void SetInventoryManager(InventoryManager* pInventoryManager);
// 	void SetItemManager(ItemManager* pItemManager);
// 	void SetProjectileManager(ProjectileManager* pProjectileManager);
// 	void SetTextEffectsManager(TextEffectsManager* pTextEffectsManager);
// 	void SetEnemyManager(EnemyManager* pEnemyManager);
// 	void SetInventoryGUI(InventoryGUI* pInventoryGUI);
// 	void SetCharacterGUI(CharacterGUI* pCharacterGUI);
// 	void SetCraftingGUI(CraftingGUI* pCraftingGUI);
// 	void SetLootGUI(LootGUI* pLootGUI);
// 	void SetActionBar(ActionBar* pActionBar);

	// Get voxel character pointer
	VoxelCharacter* GetVoxelCharacter();

	// Player reset
	void ResetPlayer();

	// Accessors / Setters
// 	void SetClass(PlayerClass ePlayerClass);
// 	PlayerClass GetClass();
	void SetName(string name);
	string GetName();
	void SetType(string typeName);
	string GetType();
	void SetModelname(string modelName);
	string GetModelName();
	void SetPosition(vec3 pos);
	void SetRespawnPosition(vec3 pos);
	vec3 GetRespawnPosition();
	vec3 GetCenter();
	vec3 GetForwardVector();
	vec3 GetRightVector();
	vec3 GetUpVector();
	float GetRadius();
	void UpdateRadius();
	void SetForwardVector(vec3 forward);
	void SetRotation(float rot);
	float GetRotation();

	// Loading
	void LoadCharacter(string characterName, bool fromCharacterSelectScreen);

// 	// Unloading
// 	void LoadWeapon(bool left, string weaponFile);
// 	void UnloadWeapon(bool left);
// 
// 	// Equipping items
// 	void EquipItem(InventoryItem* pItem, bool supressAudio = false);
// 	void UnequipItem(EquipSlot equipSlot, bool left, bool right);
// 
// 	// Stat modifier values
// 	void RefreshStatModifierCacheValues();
// 
 	// Collision
 	bool CheckCollisions(vec3 positionCheck, vec3 previousPosition, vec3 *pNormal, vec3 *pMovement, bool *pStepUpBlock);

	// Selection
	bool GetSelectionBlock(vec3 *blockPos, int* chunkIndex, int* blockX, int* blockY, int* blockZ);
	bool GetPlacementBlock(vec3 *blockPos, int* chunkIndex, int* blockX, int* blockY, int* blockZ);

	// World
	void UpdateGridPosition();
	int GetGridX() const;
	int GetGridY() const;
	int GetGridZ() const;
	Chunk* GetCachedGridChunkOrFromPosition(vec3 pos);
	void ClearChunkCacheForChunk(Chunk* pChunk);

	// Camera
	void SetCameraPosition(vec3 cameraPos);
	void SetCameraForward(vec3 cameraForward);
	void SetCameraUp(vec3 up);
	void SetCameraRight(vec3 right);

	// Loading configuration and settings for the game
//	void LoadCharacterSettings();
	void StartGame();

// 	// Movement
// 	vec3 GetPositionMovementAmount();
// 	vec3 MoveAbsolute(vec3 direction, const float speed, bool shouldChangeForward = true);
// 	void Move(const float speed);
// 	void Strafe(const float speed);
// 	void Levitate(const float speed);
// 	void StopMoving();
// 	void Jump();
// 	bool CanJump();
// 	void SetMoveToTargetPosition(vec3 pos);
// 	void DisableMoveToTargetPosition();
// 	void SetLookAtTargetAfterMoveToPosition(vec3 lookAt);
//	void CreateFloorParticles();

	// Dead
	bool IsDead();
	
// 	// Level up
// 	void LevelUp();
// 
// 	// Gameplay
// 	float GetHealth();
// 	float GetMaxHealth();
// 	float GetMagic();
// 	float GetMaxMagic();
// 	void GiveHealth(float amount);
// 	void GiveCoins(int amount);
// 
// 	// Player stats
// 	void SetSupressStatsExport(bool supress);
// 	PlayerStats* GetPlayerStats();
// 	string GetStrengthModifierString();
// 	string GetDexterityModifierString();
// 	string GetIntelligenceModifierString();
// 	string GetVitalityModifierString();
// 	string GetArmorModifierString();
// 	string GetLuckModifierString();
// 
// 	// Combat
// 	void PressAttack();
// 	void ReleaseAttack();
// 	bool CanAttackLeft();
// 	bool CanAttackRight();
// 	bool GetAttackEnabled();
// 	float GetAttackRadius();
// 	float GetAttackRotation();
// 	float GetAttackSegmentAngle();
// 	void CheckEnemyDamageRadius(Enemy* pEnemy);
// 	void CheckProjectileDamageRadius(Projectile* pProjectile);
// 	void DoDamage(float amount, Colour textColour, vec3 knockbackDirection, float knockbackAmount, bool createParticleHit);
// 	void Explode();
// 	void Respawn();
//
// 	// World editing
// 	void DestroyBlock();
//
// 	// Enemy target
// 	void SetEnemyDied(Enemy* pEnemy);
// 	void SetTargetEnemy(Enemy* pTargetEnemy);
// 	Enemy* GetTargetEnemy();
// 
// 	// Crafting
// 	void SetCrafting(bool crafting);
// 	void SetCraftingItem(bool crafting);
// 	bool IsCrafting();

	// Looking
	void LookAtPoint(vec3 point);
	void SetRandomLookMode();
	void SetBodyTurnStopThreshold(float threshold);
	void SetBodyTurnSpeedMultiplier(float multiplier);

// 	// Projectile hitbox
// 	eProjectileHitboxType GetProjectileHitboxType();
// 	float GetProjectileHitboxRadius();
// 	float GetProjectileHitboxXLength();
// 	float GetProjectileHitboxYLength();
// 	float GetProjectileHitboxZLength();
// 	vec3 GetProjectileHitboxCenter();
// 
// 	// Player equipped attributes
// 	void SetNormal();
// 	void SetSword(bool s);
// 	void SetAxe(bool s);
// 	void SetHammer(bool s);
// 	void SetMace(bool s);
// 	void SetDagger(bool s);
// 	void SetSickle(bool s);
// 	void Set2HandedSword(bool s);
// 	void SetShield(bool s);
// 	void SetBoomerang(bool s);
// 	void SetBow(bool s);
// 	void SetStaff(bool s);
// 	void SetWand(bool s);
// 	void SetPickaxe(bool s);
// 	void SetTorch(bool s);
// 	void SetItemPlacing(bool s);
// 	void SetSceneryPlacing(bool s);
// 	void SetBlockPlacing(bool s);
// 	void SetConsumable(bool s);
// 	void SetBomb(bool s);
// 	void SetSpellHands(bool s);
// 
// 	bool IsNormal();
// 	bool IsSword();
// 	bool IsAxe();
// 	bool IsHammer();
// 	bool IsMace();
// 	bool IsDagger();
// 	bool IsSickle();
// 	bool Is2HandedSword();
// 	bool IsShield();
// 	bool IsBoomerang();
// 	bool IsBow();
// 	bool IsStaff();
// 	bool IsWand();
// 	bool IsPickaxe();
// 	bool IsTorch();
// 	bool IsItemPlacing();
// 	bool IsSceneryPlacing();
// 	bool IsBlockPlacing();
// 	bool IsConsumable();
// 	bool IsBomb();
// 	bool IsSpellHands();

	// Rendering modes
	void SetWireFrameRender(bool wireframe);
	void SetPlayerAlpha(float alpha);
	void SetFirstPersonMode();
	void SetThirdPersonMode();

	// Rendering Helpers
	void CalculateWorldTransformMatrix();
	void RebuildVoxelCharacter(bool faceMerge);

	// Updating
	void Update(float dt);
	void UpdateAnimations(float dt);
	void UpdatePhysics(float dt);
// 	void UpdateMovement(float dt);
// 	void UpdateWorking(float dt);
// 	void UpdateLookingAndForwardTarget(float dt);
// 	void UpdateMagic(float dt);
// 	void UpdateTimers(float dt);
// 	void UpdateWeaponLights(float dt);
// 	void UpdateWeaponParticleEffects(float dt);
// 	void UpdateChargingAttack(float dt);
// 	void UpdateCombat(float dt);
// 	void UpdateBlockSelection(float dt);

	// Rendering
	void Render();
	void RenderFirstPerson();
	void RenderWeaponTrails();
	void RenderFace();
	void RenderPaperdoll();
	void RenderPaperdollFace();
	void RenderPortrait();
	void RenderPortraitFace();
	void RenderSelectionBlock();
// 	void RenderDebug();
// 	void RenderProjectileHitboxDebug();

protected:
	/* Protected methods */
	static void _StepUpAnimationFinished(void *apData);
	void StepUpAnimationFinished();

// 	static void _AttackEnabledTimerFinished(void *apData);
// 	void AttackEnabledTimerFinished();
// 
// 	static void _AttackEnabledDelayTimerFinished(void *apData);
// 	void AttackEnabledDelayTimerFinished();
// 
// 	static void _AttackAnimationTimerFinished(void *apData);
// 	void AttackAnimationTimerFinished();
// 
// 	static void _AttackAnimationTimerFinished_Alternative(void *apData);
// 	void AttackAnimationTimerFinished_Alternative();

private:
	/* Private methods */

public:
	/* Public members */
	static const vec3 PLAYER_CENTER_OFFSET;

	static const int PLAYER_NAME_PICKING = 1;

protected:
	/* Protected members */

private:
	/* Private members */
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	QubicleBinaryManager* m_pQubicleBinaryManager;
// 	LightingManager* m_pLightingManager;
// 	BlockParticleManager* m_pBlockParticleManager;
// 	InventoryManager* m_pInventoryManager;
// 	ItemManager* m_pItemManager;
// 	ProjectileManager* m_pProjectileManager;
// 	TextEffectsManager* m_pTextEffectsManager;
// 	EnemyManager* m_pEnemyManager;
// 	InventoryGUI* m_pInventoryGUI;
// 	CharacterGUI* m_pCharacterGUI;
// 	CraftingGUI* m_pCraftingGUI;
// 	LootGUI* m_pLootGUI;
// 	ActionBar* m_pActionBar;

	// Player position and movement variables
	vec3 m_position;
	vec3 m_velocity;
	vec3 m_force;

	// Store previous position each frame after we have worked out the new position
	// Used for collision and other movement calculations
	vec3 m_previousPosition;

	// Players respawn position
	vec3 m_respawnPosition;

	// The direction of gravity for the player
	vec3 m_gravityDirection;

	// Local axis
	vec3 m_forward;
	vec3 m_right;
	vec3 m_up;

	// Target forward / looking vector
	vec3 m_targetForward;

	// Keep track of how much we have changed position in the update, based on physics, etc.
	// So that the fake camera position can be updated, if we are in some kind of follow camera mode.
	vec3 m_positionMovementAmount;

	// Player name
	string m_name;

// 	// Player class
// 	PlayerClass m_class;
 
 	// Player type and model name
 	string m_type;
 	string m_modelName;

	// Stepping up single world blocks by walking into them
	bool m_bDoStepUpAnimation;
	float m_stepUpAnimationYAmount;
	float m_stepUpAnimationPrevious;
	float m_stepUpAnimationYOffset;
	float m_stepUpAdditionYAmountChangedCache;

	// Grid position
	int m_gridPositionX;
	int m_gridPositionY;
	int m_gridPositionZ;

	// Cached chunk from grid position
	Chunk* m_pCachedGridChunk;

	// Camera variables
	vec3 m_cameraPosition;
	vec3 m_cameraForward;
	vec3 m_cameraUp;
	vec3 m_cameraRight;

	// Ground flag
	bool m_bIsOnGround;
	float m_groundCheckTimer;

	// Floor particles
	float m_floorParticleTimer;

	// Flag to control if we are allowed to jump or not, reset when landing
	bool m_bCanJump;
	float m_jumpTimer;

	// Idle flag
	bool m_bIsIdle;

	// Dead flag
	bool m_dead;

	// Create a ghost after we have died.
	bool m_createGhost;
	float m_createGhostTimer;

// 	// Game properties
// 	float m_maxHealth;
// 	float m_health;
// 	float m_maxMagic;
// 	float m_magic;
// 
// 	// Damage and knockback timers
// 	float m_damageTime;
// 	float m_damageTimer;
// 	float m_knockbackTime;
// 	float m_knockbackTimer;
// 	float m_hitFacialExpressionTime;
// 	float m_hitFacialExpressionTimer;
// 	bool m_returnToNormalFacialExpressionAfterHit;
// 
// 	// Crafting
// 	bool m_crafting;
// 	float m_workingAnimationWaitTimer;
// 	float m_workingAnimationDelay;
// 	bool m_createdAnvilHitParticleEffect;

	// Moving to target position, for item interaction points, and NPC dialog
	bool m_moveToTargetPosition;
	vec3 m_targetPosition;
	vec3 m_lookAtPositionAfterMoveToTarget;

	// Look at point
	bool m_bLookAtPoint;
	vec3 m_lookPoint;

	// Turn speed multiplier
	float m_bodyTurnSpeedMultiplier;

	// Turn stop threshold
	float m_bodyTurnStopThreshold;

// 	// Player stats
// 	PlayerStats* m_pPlayerStats;
// 
// 	// Combat
// 	bool m_bCanAttackLeft;
// 	bool m_bCanAttackRight;
// 	bool m_bCanInteruptCombatAnim;
// 	bool m_bCanThrowWeapon;
// 	float m_bowAttackDelay;
// 	bool m_attackEnabled;
// 	float m_attackEnabledTimer;
// 	float m_attackEnabledDelayTimer;
// 	float m_attackSegmentAngle;
// 	float m_attackRadius;
// 	float m_attackRotation;
// 
// 	// Target enemy pointer, used for target camera
// 	Enemy* m_pTargetEnemy;
// 
// 	// Projectile hitbox
// 	eProjectileHitboxType m_eProjectileHitboxType;
// 	float m_projectileHitboxRadius;
// 	float m_projectileHitboxXLength;
// 	float m_projectileHitboxYLength;
// 	float m_projectileHitboxZLength;
// 	vec3 m_projectileHitboxCenterOffset;
// 
// 	// Charging attacks
 	bool m_bIsChargingAttack;
// 	float m_chargeAmount;
// 	float m_chargeTime;
// 	vec3 m_chargeSpawnVelocity;
// 	vec3 m_chargeSpawnPosition;
// 
// 	// Bitfield flag to hold the equipped properties for a player
// 	unsigned int m_equippedProperties;

	// Block selection
	bool m_blockSelection;
	vec3 m_blockSelectionPos;

	// Cached values for stat modifications, cache is refreshed whenever we equip or unequip a new item
	int m_strengthModifier;
	int m_dexterityModifier;
	int m_intelligenceModifier;
	int m_vitalityModifier;
	int m_armorModifier;
	int m_luckModifier;

	// Player radius
	float m_radius;

	// Footstep sounds
	int m_footstepSoundIndex;
	float m_footstepSoundTimer;
	float m_footstepSoundDistance;

	// Animation params
	bool m_animationFinished[AnimationSections_NUMSECTIONS];
	float m_animationTimer;

	// Players world matrix
	Matrix4x4 m_worldMatrix;

	// Voxel character
	VoxelCharacter* m_pVoxelCharacter;
	QubicleBinary* m_pCharacterBackup;
};
