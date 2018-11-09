// ******************************************************************************
// Filename:    BiomeManager.h
// Project:     Vox
// Author:      Steven Ball
//
// Purpose:
//
// Revision History:
//   Initial Revision - 06/04/15
//
// Copyright (c) 2005-2016, Steven Ball
// ******************************************************************************

#pragma once

#include "../Renderer/Renderer.h"
#include "BlocksEnum.h"

#include "noise/noise.h"
using namespace noise;


enum Biome
{
	Biome_None = 0,

	Biome_GrassLand,
	Biome_Desert,
	//Biome_Jungle,
	Biome_Tundra,
	//Biome_Swamp,
	Biome_AshLand,
	//Biome_Nightmare,

	BiomeType_NumBiomes,
};

class BiomeHeightBoundary
{
public:
	float m_heightUpperBoundary;
	float m_red1;
	float m_green1;
	float m_blue1;
	float m_red2;
	float m_green2;
	float m_blue2;
	BlockType m_blockType;
};

typedef std::vector<BiomeHeightBoundary*> BiomeHeightBoundaryList;

enum ZoneRegionType
{
	BiomeRegionType_Sphere = 0,
	BiomeRegionType_Cube,
};

class ZoneData
{
public:
	vec3 m_origin;
	ZoneRegionType m_regionType;
	float m_radius;
	float m_length;
	float m_height;
	float m_width;
	Plane3D m_planes[6];

	void UpdatePlanes(Matrix4x4 transformationMatrix);
};

typedef std::vector<ZoneData*> ZoneDataList;

class BiomeManager
{
public:
	/* Public methods */
	BiomeManager(Renderer* pRenderer);
	~BiomeManager();

	// Clear data
	void ClearBoundaryData();
	void ClearTownData();
	void ClearSafeZoneData();

	// Add data
	void AddBiomeBoundary(Biome biome, float heightUpperBoundary, float red1, float green1, float blue1, float red2, float green2, float blue2, BlockType blockType);
	void AddTown(vec3 townCenter, float radius);
	void AddTown(vec3 townCenter, float length, float height, float width);
	void AddSafeZone(vec3 safeZoneCenter, float radius);
	void AddSafeZone(vec3 safeZoneCenter, float length, float height, float width);

	// Get biome
	Biome GetBiome(vec3 position);

	// Town
	bool IsInTown(vec3 position, ZoneData **pReturnTown);
	float GetTowMultiplier(vec3 position);

	// Safe zone
	bool IsInSafeZone(vec3 position, ZoneData **pReturnSafeZone);

	// Check chunk and block type
	void GetChunkColourAndBlockType(float xPos, float yPos, float zPos, float noiseValue, float landscapeGradient, float *r, float *g, float *b, BlockType *blockType);

	// Update
	void Update(float dt);

	// Render
	void RenderDebug();
	void RenderTownsDebug();
	void RenderSafeZoneDebug();

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

	// Biome voronoi regions
	module::Voronoi biomeRegions;

	// Biome boundaries
	BiomeHeightBoundaryList m_vpBiomeHeightBoundaryList[BiomeType_NumBiomes];

	// Towns
	ZoneDataList m_vpTownsList;

	// Safe zones
	ZoneDataList m_vpSafeZonesList;
};
