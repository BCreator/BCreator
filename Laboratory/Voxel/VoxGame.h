//FIXME：暂时我自己模拟的

enum GameMode {
	GameMode_Debug = 0,
	GameMode_Loading,
	GameMode_FrontEnd,
	GameMode_Game,
};

class Camera;
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
	GameMode GetGameMode() {
		return m_gameMode;
	}

	unsigned int m_defaultViewport;
	unsigned int GetDefaultViewport() {
		return m_defaultViewport;
	}

	Camera* m_pGameCamera;
	Camera* GetGameCamera() {
		return m_pGameCamera;
	}

	VoxSettings* m_pVoxSettings;
	Renderer* m_pRenderer;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	ChunkManager* m_pChunkManager;
	BiomeManager* m_pBiomeManager;
	void init(int nWndWidth, int nWndHeight, int depthBits, int stencilBits);
	void update(int nElapsed);
	void preRender();
	void render();
	void destory();
};
