//FIXME：暂时我自己模拟的
#include "blocks/BiomeManager.h"

enum GameMode {
	GameMode_Debug = 0,
	GameMode_Loading,
	GameMode_FrontEnd,
	GameMode_Game,
};

class Camera;
class Player;
class VoxSettings;
class Renderer;
class QubicleBinaryManager;
class ChunkManager;
class BiomeManager;
class VoxGame {
public:
	static VoxGame *c_instance;
	static VoxGame* GetInstance();

	GameMode m_gameMode;
	void SetGameMode(GameMode mode);
	GameMode GetGameMode();

	unsigned int m_defaultViewport;
	unsigned int GetDefaultViewport() {
		return m_defaultViewport;
	}

	Camera* m_pGameCamera;
	Camera* GetGameCamera() {
		return m_pGameCamera;
	}

	void UpdateCamera(float dt);
	void UpdateCameraFirstPerson(float dt);
	void RenderFirstPersonViewport();

	Player* m_pPlayer;
	VoxSettings* m_pVoxSettings;
	Renderer* m_pRenderer;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	ChunkManager* m_pChunkManager;
	BiomeManager* m_pBiomeManager;
	// Biome
	Biome m_currentBiome;
	unsigned int m_firstpersonViewport;
	void Init(int nWndWidth, int nWndHeight, int depthBits, int stencilBits);
	void Update(int nElapsed);
	void PreRender();
	void Render();
	void Destroy();

	// Materials
	unsigned int m_defaultMaterial;
	// Shaders
	unsigned int m_defaultShader;
	void BeginShaderRender();
	void EndShaderRender();
	void RenderTransparency();
};
