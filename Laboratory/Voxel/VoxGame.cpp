#include"Renderer/camera.h"
#include"blocks/ChunkManager.h"
#include"blocks/BiomeManager.h"
#include"Renderer/Renderer.h"
#include"VoxSettings.h"
#include"models/QubicleBinaryManager.h"

#include "VoxGame.h"

VoxGame *VoxGame::c_instance = 0;

VoxGame* VoxGame::GetInstance()
{
	if (c_instance == 0)
		c_instance = new VoxGame;

	return c_instance;
}

void VoxGame::init(int nWndWidth, int nWndHeight, int depthBits, int stencilBits) {
	m_pVoxSettings = new VoxSettings();
	m_pVoxSettings->LoadSettings();
	m_pVoxSettings->LoadOptions();
	m_pRenderer = new Renderer(nWndWidth, nWndHeight, depthBits, stencilBits);
	m_pQubicleBinaryManager = new QubicleBinaryManager(m_pRenderer);
	m_pChunkManager = new ChunkManager(m_pRenderer, m_pVoxSettings, m_pQubicleBinaryManager);
	m_pChunkManager->SetStepLockEnabled(m_pVoxSettings->m_stepUpdating);
	m_pBiomeManager = new BiomeManager(m_pRenderer);
}

void VoxGame::update(int nElapsed) {
	float m_deltaTime = 1.0f / nElapsed;
	m_pBiomeManager->Update(m_deltaTime);
}
void VoxGame::preRender() {

}
void VoxGame::render() {

}
void VoxGame::destory() {

}
