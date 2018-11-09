#include"Renderer/camera.h"
#include"blocks/ChunkManager.h"
#include"blocks/BiomeManager.h"

#include<glm/detail/func_geometric.hpp>
#include"Player/Player.h"

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

void VoxGame::Update(int nElapsed) {
	float m_deltaTime = 1.0f / nElapsed;
	m_pBiomeManager->Update(m_deltaTime);
	m_pChunkManager->Update(m_deltaTime);
	// Update the camera based on movements
	if (m_gameMode == GameMode_Game)
	{
		UpdateCamera(m_deltaTime);
		m_pPlayer->SetCameraPosition(m_pGameCamera->GetPosition());
		m_pPlayer->SetCameraForward(normalize(m_pGameCamera->GetFacing()));
		m_pPlayer->SetCameraUp(normalize(m_pGameCamera->GetUp()));
		m_pPlayer->SetCameraRight(normalize(m_pGameCamera->GetRight()));
	}
}
void VoxGame::PreRender() {
	// Update matrices for game objects
	m_pPlayer->CalculateWorldTransformMatrix();
	//m_pNPCManager->CalculateWorldTransformMatrix();
	//m_pEnemyManager->CalculateWorldTransformMatrix();
	//m_pItemManager->CalculateWorldTransformMatrix();
	//m_pProjectileManager->CalculateWorldTransformMatrix();
}

void VoxGame::Init(int nWndWidth, int nWndHeight, int depthBits, int stencilBits) {
	m_pVoxSettings = new VoxSettings();
	m_pVoxSettings->LoadSettings();
	m_pVoxSettings->LoadOptions();

	m_pRenderer = NULL;
	m_pGameCamera = NULL;
	m_pQubicleBinaryManager = NULL;
	m_pPlayer = NULL;
	m_pChunkManager = NULL;
// 	 	m_pFrontendManager = NULL;
// 	 
// 	 	m_pInventoryGUI = NULL;
// 	 	m_pCharacterGUI = NULL;
// 	 	m_pLootGUI = NULL;
// 	 	m_pCraftingGUI = NULL;
// 	 	m_pQuestGUI = NULL;
// 	 	m_pActionBar = NULL;
// 	 	m_pHUD = NULL;
// 	 
// 	 	m_GUICreated = false;
// 	 
//
// 	m_pVoxSettings = pVoxSettings;
// 	m_pVoxWindow = new VoxWindow(this, m_pVoxSettings);
// 
// 	// Create the window
// 	m_pVoxWindow->Create();
// 
// 	/* Setup the FPS and deltatime counters */
// #ifdef _WIN32
// 	QueryPerformanceCounter(&m_fpsPreviousTicks);
// 	QueryPerformanceCounter(&m_fpsCurrentTicks);
// 	QueryPerformanceFrequency(&m_fpsTicksPerSecond);
// #else
// 	struct timeval tm;
// 	gettimeofday(&tm, NULL);
// 	m_fpsCurrentTicks = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
// 	m_fpsPreviousTicks = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
// #endif //_WIN32
// 	m_deltaTime = 0.0f;
// 	m_fps = 0.0f;
// 
// 	/* Mouse name picking */
// 	m_pickedObject = -1;
// 	m_bNamePickingSelected = false;
// 
// 	/* Custom cursors */
// 	m_bPressedCursorDown = false;
// 	m_bCustomCursorOn = false;
// 
// 	/* Paper doll viewport dimensions */
// 	m_paperdollViewportX = 0;
// 	m_paperdollViewportY = 0;
// 	m_paperdollViewportWidth = 800;
// 	m_paperdollViewportHeight = 800;
// 
// 	/* Portrain viewport dimensions */
// 	m_portraitViewportX = 0;
// 	m_portraitViewportY = 0;
// 	m_portraitViewportWidth = 800;
// 	m_portraitViewportHeight = 800;
// 
// 	/* Setup the initial starting wait timing */
// 	m_initialWaitTimer = 0.0f;
// 	m_initialWaitTime = 0.5f;
// 	m_initialStartWait = true;
// 
// 	/* Create the renderer */
// 	m_windowWidth = m_pVoxWindow->GetWindowWidth();
// 	m_windowHeight = m_pVoxWindow->GetWindowHeight();
// 	m_pRenderer = new Renderer(m_windowWidth, m_windowHeight, 32, 8);
	m_pRenderer = new Renderer(nWndWidth, nWndHeight, depthBits, stencilBits);
 
// 	/* Pause and quit */
// 	m_bGameQuit = false;
// 	m_bPaused = false;
// 
// 	/* Interactions */
// 	m_pInteractItem = NULL;
 
 	/* Biome */
 	m_currentBiome = Biome_None;
 
// 	/* Music and Audio */
// 	m_pMusicChannel = NULL;
// 	m_pMusicSound = NULL;
// 	m_currentBiomeMusic = Biome_None;
// 
// 	/* Create the GUI */
// 	m_pGUI = new OpenGLGUI(m_pRenderer);

	/* Create cameras */
	m_pGameCamera = new Camera(m_pRenderer);
	m_pGameCamera->SetPosition(vec3(8.0f, 8.25f, 15.5f));
	m_pGameCamera->SetFakePosition(m_pGameCamera->GetPosition());
	m_pGameCamera->SetFacing(vec3(0.0f, 0.0f, -1.0f));
	m_pGameCamera->SetUp(vec3(0.0f, 1.0f, 0.0f));
	m_pGameCamera->SetRight(vec3(1.0f, 0.0f, 0.0f));

	/* Create viewports */
//	m_pRenderer->CreateViewport(0, 0, m_windowWidth, m_windowHeight, 60.0f, &m_defaultViewport);
//	m_pRenderer->CreateViewport(0, 0, m_windowWidth, m_windowHeight, 60.0f, &m_firstpersonViewport);
	m_pRenderer->CreateViewport(0, 0, nWndWidth, nWndHeight, 60.0f, &m_firstpersonViewport);
// 	m_pRenderer->CreateViewport(m_paperdollViewportY, m_paperdollViewportX, m_paperdollViewportWidth, m_paperdollViewportHeight, 60.0f, &m_paperdollViewport);
// 	m_pRenderer->CreateViewport(m_portraitViewportY, m_portraitViewportX, m_portraitViewportWidth, m_portraitViewportHeight, 60.0f, &m_portraitViewport);
// 
// 	/* Create fonts */
// 	m_pRenderer->CreateFreeTypeFont("media/fonts/arial.ttf", 12, &m_defaultFont);
// 
// 	/* Create the custom cursor textures */
// 	int lTextureWidth, lTextureHeight, lTextureWidth2, lTextureHeight2;
// 	m_pRenderer->LoadTexture("media/textures/cursors/finger_cursor_normal.tga", &lTextureWidth, &lTextureHeight, &lTextureWidth2, &lTextureHeight2, &m_customCursorNormalBuffer);
// 	m_pRenderer->LoadTexture("media/textures/cursors/finger_cursor_clicked.tga", &lTextureWidth, &lTextureHeight, &lTextureWidth2, &lTextureHeight2, &m_customCursorClickedBuffer);
// 	m_pRenderer->LoadTexture("media/textures/cursors/finger_cursor_rotate.tga", &lTextureWidth, &lTextureHeight, &lTextureWidth2, &lTextureHeight2, &m_customCursorRotateBuffer);
// 	m_pRenderer->LoadTexture("media/textures/cursors/finger_cursor_zoom.tga", &lTextureWidth, &lTextureHeight, &lTextureWidth2, &lTextureHeight2, &m_customCursorZoomBuffer);
// 
// 	/* Create lights */
// 	m_defaultLightPosition = vec3(300.0f, 300.0f, 300.0f);
// 	m_defaultLightView = vec3(0.0f, 0.0f, 0.0f);
// 	vec3 lightDirection = m_defaultLightView - m_defaultLightPosition;
// 	m_pRenderer->CreateLight(Colour(1.0f, 1.0f, 1.0f, 1.0f), Colour(1.0f, 1.0f, 1.0f, 1.0f), Colour(0.0f, 0.0f, 0.0f, 1.0f),
// 		m_defaultLightPosition, lightDirection, 0.0f, 0.0f, 2.0f, 0.001f, 0.0f, true, false, &m_defaultLight);

	/* Create materials */
	m_pRenderer->CreateMaterial(Colour(1.0f, 1.0f, 1.0f, 1.0f), Colour(1.0f, 1.0f, 1.0f, 1.0f), Colour(1.0f, 1.0f, 1.0f, 1.0f), Colour(0.0f, 0.0f, 0.0f, 1.0f), 64, &m_defaultMaterial);

	/* Create the frame buffers */
// 	bool frameBufferCreated = false;
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "SSAO", &m_SSAOFrameBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 5.0f, "Shadow", &m_shadowFrameBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "Deferred Lighting", &m_lightingFrameBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "Transparency", &m_transparencyFrameBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "Water Reflection", &m_waterReflectionFrameBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "FXAA", &m_FXAAFrameBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "FullScreen 1st Pass", &m_firstPassFullscreenBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "FullScreen 2nd Pass", &m_secondPassFullscreenBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, 800, 800, 1.0f, "Paperdoll", &m_paperdollBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, 800, 800, 1.0f, "Paperdoll SSAO Texture", &m_paperdollSSAOTextureBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, 800, 800, 1.0f, "Portrait", &m_portraitBuffer);
// 	frameBufferCreated = m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, 800, 800, 1.0f, "Portrait SSAO Texture", &m_portraitSSAOTextureBuffer);

	/* Create the shaders */
	bool shaderLoaded = false;
	m_defaultShader = -1;
// 	m_phongShader = -1;
// 	m_SSAOShader = -1;
// 	m_shadowShader = -1;
// 	m_waterShader = -1;
// 	m_lightingShader = -1;
// 	m_cubeMapShader = -1;
// 	m_textureShader = -1;
// 	m_fxaaShader = -1;
// 	m_blurVerticalShader = -1;
// 	m_blurHorizontalShader = -1;
// 	m_paperdollShader = -1;
	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/default.vertex", "media/shaders/default.pixel", &m_defaultShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/phong.vertex", "media/shaders/phong.pixel", &m_phongShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/shadow.vertex", "media/shaders/shadow.pixel", &m_shadowShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/water_still.vertex", "media/shaders/water_still.pixel", &m_waterShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/texture.vertex", "media/shaders/texture.pixel", &m_textureShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/fullscreen/SSAO.vertex", "media/shaders/fullscreen/SSAO.pixel", &m_SSAOShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/fullscreen/fxaa.vertex", "media/shaders/fullscreen/fxaa.pixel", &m_fxaaShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/fullscreen/lighting.vertex", "media/shaders/fullscreen/lighting.pixel", &m_lightingShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/cube_map.vertex", "media/shaders/cube_map.pixel", &m_cubeMapShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/fullscreen/blur_vertical.vertex", "media/shaders/fullscreen/blur_vertical.pixel", &m_blurVerticalShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/fullscreen/blur_horizontal.vertex", "media/shaders/fullscreen/blur_horizontal.pixel", &m_blurHorizontalShader);
// 	shaderLoaded = m_pRenderer->LoadGLSLShader("media/shaders/paperdoll.vertex", "media/shaders/paperdoll.pixel", &m_paperdollShader);

// 	/* Create the mods manager */
// 	m_pModsManager = new ModsManager();
// 	m_pModsManager->LoadMods();
// 
// 	/* Create the audio manager */
// 	AudioManager::GetInstance()->Setup();

	/* Create the qubicle binary file manager */
	m_pQubicleBinaryManager = new QubicleBinaryManager(m_pRenderer);

	/* Create the chunk manager*/
	m_pChunkManager = new ChunkManager(m_pRenderer, m_pVoxSettings, m_pQubicleBinaryManager);
	m_pChunkManager->SetStepLockEnabled(m_pVoxSettings->m_stepUpdating);

	/* Create the biome manager */
	m_pBiomeManager = new BiomeManager(m_pRenderer);

// 	/* Create the lighting manager */
// 	m_pLightingManager = new LightingManager(m_pRenderer);
// 
// 	/* Create the scenery manager */
// 	m_pSceneryManager = new SceneryManager(m_pRenderer, m_pChunkManager);
// 
// 	/* Create the skybox */
// 	m_pSkybox = new Skybox(m_pRenderer);
// 
// 	/* Create the block particle manager */
// 	m_pBlockParticleManager = new BlockParticleManager(m_pRenderer, m_pChunkManager);
// 
// 	/* Create the text effects manager */
// 	m_pTextEffectsManager = new TextEffectsManager(m_pRenderer);
// 	m_pTextEffectsManager->SetCamera(m_pGameCamera);
// 
// 	/* Create the instance manager */
// 	m_pInstanceManager = new InstanceManager(m_pRenderer);

	/* Create the player */
//	m_pPlayer = new Player(m_pRenderer, m_pChunkManager, m_pQubicleBinaryManager, m_pLightingManager, m_pBlockParticleManager);
	m_pPlayer = new Player(m_pRenderer, m_pChunkManager, m_pQubicleBinaryManager, nullptr, nullptr);

// 	/* Create the NPC manager */
// 	m_pNPCManager = new NPCManager(m_pRenderer, m_pChunkManager);
// 
// 	/* Create the enemy manager */
// 	m_pEnemyManager = new EnemyManager(m_pRenderer, m_pChunkManager, m_pPlayer);
// 
// 	/* Create the quest manager */
// 	m_pQuestManager = new QuestManager();
// 
// 	/* Create the quest journal */
// 	m_pQuestJournal = new QuestJournal(m_pQuestManager);
// 
// 	/* Create the inventory manager */
// 	m_pInventoryManager = new InventoryManager();
// 
// 	/* Create the item manager */
// 	m_pItemManager = new ItemManager(m_pRenderer, m_pChunkManager, m_pPlayer);
// 
// 	/* Create the random loot manager */
// 	m_pRandomLootManager = new RandomLootManager();
// 
// 	/* Create the projectile manager */
// 	m_pProjectileManager = new ProjectileManager(m_pRenderer, m_pChunkManager);
// 
// 	/* Create the frontend manager */
// 	m_pFrontendManager = new FrontendManager(m_pRenderer, m_pGUI);
// 	m_pFrontendManager->SetWindowDimensions(m_windowWidth, m_windowHeight);
// 	m_pFrontendManager->SetCamera(m_pGameCamera);
// 
// 	/* Create the game GUI pages */
// 	m_pInventoryGUI = new InventoryGUI(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
// 	m_pCharacterGUI = new CharacterGUI(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
// 	m_pLootGUI = new LootGUI(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
// 	m_pCraftingGUI = new CraftingGUI(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
// 	m_pQuestGUI = new QuestGUI(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
// 	m_pActionBar = new ActionBar(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
// 	m_pHUD = new HUD(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);

	/* Create module and manager linkage */
	m_pChunkManager->SetPlayer(m_pPlayer);
//	m_pChunkManager->SetSceneryManager(m_pSceneryManager);
	m_pChunkManager->SetBiomeManager(m_pBiomeManager);
// 	m_pChunkManager->SetEnemyManager(m_pEnemyManager);
// 	m_pChunkManager->SetNPCManager(m_pNPCManager);
// 	m_pChunkManager->SetBlockParticleManager(m_pBlockParticleManager);
// 	m_pChunkManager->SetItemManager(m_pItemManager);
// 	m_pPlayer->SetInventoryManager(m_pInventoryManager);
// 	m_pPlayer->SetItemManager(m_pItemManager);
// 	m_pPlayer->SetProjectileManager(m_pProjectileManager);
// 	m_pPlayer->SetTextEffectsManager(m_pTextEffectsManager);
// 	m_pPlayer->SetEnemyManager(m_pEnemyManager);
// 	m_pPlayer->SetInventoryGUI(m_pInventoryGUI);
// 	m_pPlayer->SetCharacterGUI(m_pCharacterGUI);
// 	m_pPlayer->SetCraftingGUI(m_pCraftingGUI);
// 	m_pPlayer->SetLootGUI(m_pLootGUI);
// 	m_pPlayer->SetActionBar(m_pActionBar);
// 	m_pNPCManager->SetPlayer(m_pPlayer);
// 	m_pNPCManager->SetLightingManager(m_pLightingManager);
// 	m_pNPCManager->SetBlockParticleManager(m_pBlockParticleManager);
// 	m_pNPCManager->SetTextEffectsManager(m_pTextEffectsManager);
// 	m_pNPCManager->SetItemManager(m_pItemManager);
// 	m_pNPCManager->SetQubicleBinaryManager(m_pQubicleBinaryManager);
// 	m_pNPCManager->SetProjectileManager(m_pProjectileManager);
// 	m_pNPCManager->SetEnemyManager(m_pEnemyManager);
// 	m_pEnemyManager->SetLightingManager(m_pLightingManager);
// 	m_pEnemyManager->SetBlockParticleManager(m_pBlockParticleManager);
// 	m_pEnemyManager->SetTextEffectsManager(m_pTextEffectsManager);
// 	m_pEnemyManager->SetItemManager(m_pItemManager);
// 	m_pEnemyManager->SetProjectileManager(m_pProjectileManager);
// 	m_pEnemyManager->SetHUD(m_pHUD);
// 	m_pEnemyManager->SetQubicleBinaryManager(m_pQubicleBinaryManager);
// 	m_pEnemyManager->SetNPCManager(m_pNPCManager);
// 	m_pInventoryManager->SetPlayer(m_pPlayer);
// 	m_pInventoryManager->SetInventoryGUI(m_pInventoryGUI);
// 	m_pInventoryManager->SetLootGUI(m_pLootGUI);
// 	m_pInventoryManager->SetActionBar(m_pActionBar);
// 	m_pItemManager->SetLightingManager(m_pLightingManager);
// 	m_pItemManager->SetBlockParticleManager(m_pBlockParticleManager);
// 	m_pItemManager->SetQubicleBinaryManager(m_pQubicleBinaryManager);
// 	m_pItemManager->SetInventoryManager(m_pInventoryManager);
// 	m_pItemManager->SetNPCManager(m_pNPCManager);
// 	m_pProjectileManager->SetLightingManager(m_pLightingManager);
// 	m_pProjectileManager->SetBlockParticleManager(m_pBlockParticleManager);
// 	m_pProjectileManager->SetPlayer(m_pPlayer);
// 	m_pProjectileManager->SetQubicleBinaryManager(m_pQubicleBinaryManager);
// 	m_pQuestManager->SetNPCManager(m_pNPCManager);
// 	m_pQuestManager->SetInventoryManager(m_pInventoryManager);
// 	m_pQuestManager->SetQuestJournal(m_pQuestJournal);
// 	m_pQuestJournal->SetPlayer(m_pPlayer);
// 	m_pInventoryGUI->SetCharacterGUI(m_pCharacterGUI);
// 	m_pInventoryGUI->SetLootGUI(m_pLootGUI);
// 	m_pInventoryGUI->SetActionBar(m_pActionBar);
// 	m_pInventoryGUI->SetItemManager(m_pItemManager);
// 	m_pCharacterGUI->SetInventoryGUI(m_pInventoryGUI);
// 	m_pCharacterGUI->SetLootGUI(m_pLootGUI);
// 	m_pCharacterGUI->SetActionBar(m_pActionBar);
// 	m_pLootGUI->SetInventoryGUI(m_pInventoryGUI);
// 	m_pLootGUI->SetCharacterGUI(m_pCharacterGUI);
// 	m_pLootGUI->SetActionBar(m_pActionBar);
// 	m_pActionBar->SetInventoryGUI(m_pInventoryGUI);
// 	m_pActionBar->SetCharacterGUI(m_pCharacterGUI);
// 	m_pActionBar->SetLootGUI(m_pLootGUI);
// 	m_pQuestGUI->SetQuestJournal(m_pQuestJournal);
// 	m_pHUD->SetInventoryGUI(m_pInventoryGUI);
// 	m_pHUD->SetCharacterGUI(m_pCharacterGUI);
// 	m_pHUD->SetQuestGUI(m_pQuestGUI);
// 	m_pHUD->SetCraftingGUI(m_pCraftingGUI);
// 
// 	// Keyboard movement
// 	m_bKeyboardForward = false;
// 	m_bKeyboardBackward = false;
// 	m_bKeyboardStrafeLeft = false;
// 	m_bKeyboardStrafeRight = false;
// 	m_bKeyboardLeft = false;
// 	m_bKeyboardRight = false;
// 	m_bKeyboardUp = false;
// 	m_bKeyboardDown = false;
// 	m_bKeyboardSpace = false;
// 	m_bKeyboardMenu = false;
// 
// 	// Joystick flags
// 	m_bJoystickJump = false;
// 
// 	// Combat flags
// 	m_bAttackPressed_Mouse = false;
// 	m_bAttackReleased_Mouse = false;
// 	m_bAttackPressed_Joystick = false;
// 	m_bAttackReleased_Joystick = false;
// 	m_bCanDoAttack_Mouse = true;
// 	m_bCanDoAttack_Joystick = true;
// 	m_bTargetEnemyPressed_Joystick = false;
// 	m_bTargetEnemyReleased_Joystick = false;
// 
// 	// Camera movement
// 	m_bCameraRotate = false;
// 	m_pressedX = 0;
// 	m_pressedY = 0;
// 	m_currentX = 0;
// 	m_currentY = 0;
// 	m_cameraDistance = m_pGameCamera->GetZoomAmount();
// 	m_maxCameraDistance = m_cameraDistance;
// 
// 	// Auto camera mode
// 	m_autoCameraMovingModifier = 1.0f;
// 
// 	// Enemy target camera mode
// 	m_targetCameraXAxisAmount = 0.0f;
// 	m_targetCameraXAxisAmount_Target = 0.0f;
// 	m_targetCameraYRatio = 0.0f;
// 	m_targetCameraForwardRatio = 0.0f;
// 
// 	// Player movement
// 	m_keyboardMovement = false;
// 	m_gamepadMovement = false;
// 	m_movementSpeed = 0.0f;
// 	m_movementDragTime = 0.45f;
// 	m_movementIncreaseTime = 0.25f;
// 	m_maxMovementSpeed = 10.0f;
// 	m_movementStopThreshold = 0.05f;
// 
// 	// Blur
// 	m_globalBlurAmount = 0.0f;
// 
// 	// Cinematic letterbox mode
// 	m_letterBoxRatio = 0.0f;
// 
// 	// Water
// 	m_elapsedWaterTime = 0.0f;
// 
// 	// Paperdoll rendering
// 	m_paperdollRenderRotation = 0.0f;
// 
// 	// Toggle flags
// 	m_deferredRendering = true;
// 	m_modelWireframe = false;
// 	m_modelAnimationIndex = 0;
// 	m_multiSampling = true;
// 	m_ssao = true;
// 	m_blur = false;
// 	m_dynamicLighting = true;
// 	m_animationUpdate = true;
// 	m_fullscreen = m_pVoxSettings->m_fullscreen;
// 	m_debugRender = false;
// 	m_instanceRender = true;
// 
// 	// Camera mode
// 	m_cameraMode = CameraMode_Debug;
// 	m_previousCameraMode = CameraMode_Debug;
// 	m_shouldRestorePreviousCameraMode = false;

	// Game mode
	m_gameMode = GameMode_Loading;
// 	m_allowToChangeToGame = true;
// 	m_allowToChangeToFrontend = true;
	SetGameMode(m_gameMode);

// 	// Turn the cursor initially off if we have custom cursors enabled
// 	if (m_pVoxSettings->m_customCursors)
// 	{
// 		TurnCursorOff(true);
// 	}
// 
// 	// Create, setup and skin the GUI components
// 	CreateGUI();
// 	SetupGUI();
// 	SkinGUI();
// 	UpdateGUI(0.0f);
}

// Destruction
void VoxGame::Destroy()
{
	if (c_instance)
	{
// 		delete m_pSkybox;
// 		delete m_pChunkManager;
// 		delete m_pItemManager;
// 		delete m_pRandomLootManager;
// 		delete m_pInventoryManager;
// 		delete m_pFrontendManager;
// 		delete m_pPlayer;
// 		delete m_pNPCManager;
// 		delete m_pEnemyManager;
// 		delete m_pLightingManager;
// 		delete m_pSceneryManager;
// 		delete m_pBlockParticleManager;
// 		delete m_pTextEffectsManager;
// 		delete m_pInstanceManager;
// 		delete m_pBiomeManager;
// 		delete m_pQubicleBinaryManager;
// 		delete m_pModsManager;
// 		delete m_pGameCamera;
// 		delete m_pQuestManager;
// 		delete m_pQuestJournal;
// 		delete m_pInventoryGUI;
// 		delete m_pCharacterGUI;
// 		delete m_pLootGUI;
// 		delete m_pCraftingGUI;
// 		delete m_pQuestGUI;
// 		delete m_pActionBar;
// 		DestroyGUI();  // Destroy the GUI components before we delete the GUI manager object.
// 		delete m_pGUI;
// 		delete m_pRenderer;
// 
// 		AudioManager::GetInstance()->Shutdown();
// 
// 		m_pVoxWindow->Destroy();
// 
// 		delete m_pVoxWindow;

		delete c_instance;
	}
}

void VoxGame::SetGameMode(GameMode mode)
{
	GameMode previousgameMode = m_gameMode;
	m_gameMode = mode;

	if (m_gameMode == GameMode_Debug)
	{
	}

	if (m_gameMode == GameMode_FrontEnd)
	{
		if (previousgameMode == GameMode_Game || previousgameMode == GameMode_Loading)
		{
// 			// Close all open GUI windows
// 			CloseAllGUIWindows();
// 
// 			// Clear the items
// 			m_pItemManager->ClearItems();
// 			m_pItemManager->ClearItemSpawners();
// 
// 			// Clear the NPCs
// 			m_pNPCManager->ClearNPCs();
// 
// 			// Clear the enemies and enemy spawners
// 			m_pEnemyManager->ClearEnemies();
// 			m_pEnemyManager->ClearEnemySpawners();
// 
// 			// Clear all projectiles
// 			m_pProjectileManager->ClearProjectiles();
// 
// 			// Reset the inventory manager
// 			m_pInventoryManager->Reset();
// 
// 			// Clear the quests
// 			m_pQuestManager->ClearQuests();
// 
// 			// Clear the quest journal
// 			m_pQuestJournal->ClearJournal();
// 
// 			// Reset the quest GUI text components
// 			m_pQuestGUI->SetQuestData("", "");

			// Reset the player
			m_pPlayer->ResetPlayer();

			// Set the water level
			m_pChunkManager->SetWaterHeight(1.3f);

// 			// Unload actionbar
// 			if (m_pActionBar->IsLoaded())
// 			{
// 				if (m_pVoxSettings->m_renderGUI)
// 				{
// 					m_pActionBar->Unload();
// 				}
// 			}
// 
// 			// Unload the HUD
// 			if (m_pHUD->IsLoaded())
// 			{
// 				if (m_pVoxSettings->m_renderGUI)
// 				{
// 					m_pHUD->Unload();
// 				}
// 			}
// 
// 			// Setup the gamedata since we have just loaded fresh into the frontend.
// 			SetupDataForFrontEnd();
// 
// 			// Music
// 			StopMusic();
// 			StartFrontEndMusic();

			// Initial chunk creation
			m_pChunkManager->InitializeChunkCreation();
		}
	}

	if (m_gameMode == GameMode_Game)
	{
		if (previousgameMode == GameMode_FrontEnd || previousgameMode == GameMode_Loading)
		{
// 			// Close all open GUI windows
// 			CloseAllGUIWindows();
// 
// 			// Clear the items
// 			m_pItemManager->ClearItems();
// 			m_pItemManager->ClearItemSpawners();
// 
// 			// Clear the NPCs
// 			m_pNPCManager->ClearNPCs();
// 
// 			// Clear the enemies and enemy spawners
// 			m_pEnemyManager->ClearEnemies();
// 			m_pEnemyManager->ClearEnemySpawners();
// 
// 			// Clear all projectiles
// 			m_pProjectileManager->ClearProjectiles();
// 
// 			// Clear the quests
// 			m_pQuestManager->ClearQuests();
// 
// 			// Clear the quest journal
// 			m_pQuestJournal->ClearJournal();
// 
// 			// Reset the quest GUI text components
// 			m_pQuestGUI->SetQuestData("", "");

			// Reset the player
			m_pPlayer->ResetPlayer();

			// Set the water level
			m_pChunkManager->SetWaterHeight(1.3f);

// 			// Load action bar
// 			if (m_pActionBar->IsLoaded() == false)
// 			{
// 				if (m_pVoxSettings->m_renderGUI)
// 				{
// 					m_pActionBar->Load();
// 				}
// 			}
// 
// 			// Load the HUD
// 			if (m_pHUD->IsLoaded() == false)
// 			{
// 				if (m_pVoxSettings->m_renderGUI)
// 				{
// 					m_pHUD->Load();
// 				}
// 			}
// 
// 			// Setup the gamedata since we have just loaded fresh into a game.
// 			SetupDataForGame();
// 
// 			// Music
// 			StopMusic();
// 			StartGameMusic();

			// Initial chunk creation
			m_pChunkManager->InitializeChunkCreation();
		}
	}
}

GameMode VoxGame::GetGameMode()
{
	return m_gameMode;
}

////////////////////////////////////////////////////////////////////////////////
void VoxGame::UpdateCamera(float dt)
{
	//if (m_pPlayer->IsDead() == false) {
	//	if (m_bPaused == false) {
	//		if (m_cameraMode == CameraMode_MouseRotate)
	//			UpdateCameraAutoCamera(dt, false);
	//		else if (m_cameraMode == CameraMode_AutoCamera)
	//			UpdateCameraAutoCamera(dt, true);
	//		else if (m_cameraMode == CameraMode_FirstPerson)
				UpdateCameraFirstPerson(dt);
	//		else if (m_cameraMode == CameraMode_NPCDialog)
	//			UpdateCameraNPCDialog(dt);
	//		else if (m_cameraMode == CameraMode_EnemyTarget)
	//			UpdateCameraEnemyTarget(dt);
	//	}
	//	if (m_gameMode == GameMode_Game && m_cameraMode != CameraMode_Debug)
	//		UpdateCameraModeSwitching();
	//}
}

void VoxGame::UpdateCameraFirstPerson(float dt)
{
	m_pGameCamera->SetFakePosition(m_pPlayer->GetCenter() + Player::PLAYER_CENTER_OFFSET);
	m_pPlayer->SetForwardVector(m_pGameCamera->GetFacing());
}
void VoxGame::RenderFirstPersonViewport()
{
	m_pRenderer->PushMatrix();
	glLoadIdentity();

	m_pRenderer->SetProjectionMode(PM_PERSPECTIVE, m_firstpersonViewport);

	m_pRenderer->SetLookAtCamera(vec3(0.0f, 1.3f, -0.5f), vec3(0.0f, 1.3f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	// Render the player first person mode - only hands and weapons
	m_pRenderer->PushMatrix();
	m_pRenderer->StartMeshRender();

	BeginShaderRender();
	{
//		m_pPlayer->RenderFirstPerson();
	}
	EndShaderRender();

	m_pRenderer->EndMeshRender();

	m_pRenderer->PopMatrix();
	m_pRenderer->PopMatrix();
}


////////////////////////////////////////////////////////////////////////////////
void VoxGame::BeginShaderRender()
{
	glShader* pShader = NULL;

	//if (m_pVoxSettings->m_shadows)
	//{
	//	m_pRenderer->BeginGLSLShader(m_shadowShader);

	//	pShader = m_pRenderer->GetShader(m_shadowShader);
	//	GLuint shadowMapUniform = glGetUniformLocationARB(pShader->GetProgramObject(), "ShadowMap");
	//	m_pRenderer->PrepareShaderTexture(7, shadowMapUniform);
	//	m_pRenderer->BindRawTextureId(m_pRenderer->GetDepthTextureFromFrameBuffer(m_shadowFrameBuffer));
	//	glUniform1iARB(glGetUniformLocationARB(pShader->GetProgramObject(), "renderShadow"), m_pVoxSettings->m_shadows);
	//	glUniform1iARB(glGetUniformLocationARB(pShader->GetProgramObject(), "alwaysShadow"), false);
	//}
	//else
	//{
		m_pRenderer->BeginGLSLShader(m_defaultShader);

		pShader = m_pRenderer->GetShader(m_defaultShader);
	//}

	//bool fogEnabled = (m_pFrontendManager->GetFrontendScreen() == FrontendScreen_MainMenu) ? false : m_pVoxSettings->m_fogRendering;
	//glUniform1iARB(glGetUniformLocationARB(pShader->GetProgramObject(), "enableFog"), fogEnabled);
	//float lfogEnd = m_pChunkManager->GetLoaderRadius() - Chunk::CHUNK_SIZE*Chunk::BLOCK_RENDER_SIZE;
	//float lfogStart = lfogEnd - Chunk::CHUNK_SIZE*Chunk::BLOCK_RENDER_SIZE*2.0f;
	//GLfloat fogColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
	//glFogi(GL_FOG_MODE, GL_LINEAR);
	//glFogfv(GL_FOG_COLOR, fogColor);
	//glFogf(GL_FOG_DENSITY, 1.0f);
	//glHint(GL_FOG_HINT, GL_DONT_CARE);
	//glFogf(GL_FOG_START, lfogStart);
	//glFogf(GL_FOG_END, lfogEnd);
	//glEnable(GL_FOG);
}
void VoxGame::EndShaderRender()
{
	glDisable(GL_FOG);

	//if (m_pVoxSettings->m_shadows)
	//{
	//	m_pRenderer->EndGLSLShader(m_shadowShader);
	//}
	//else
	//{
		m_pRenderer->EndGLSLShader(m_defaultShader);
	//}
}

void VoxGame::RenderTransparency()
{
	m_pRenderer->PushMatrix();
	m_pRenderer->SetProjectionMode(PM_PERSPECTIVE, m_defaultViewport);
	m_pRenderer->SetCullMode(CM_BACK);

	// Set the lookat camera
	m_pGameCamera->Look();

	//if (m_deferredRendering)
	//{
	//	m_pRenderer->StartRenderingToFrameBuffer(m_transparencyFrameBuffer);
	//}

	//if (m_gameMode != GameMode_FrontEnd)
	//{
	//	// Render the player's face
	//	if (m_cameraMode != CameraMode_FirstPerson)
	//	{
	//		m_pPlayer->RenderFace();

	//		// Render the player's weapon trails
	//		m_pPlayer->RenderWeaponTrails();
	//	}
	//}

	//// NPC Faces
	//m_pNPCManager->RenderFaces();
	//m_pNPCManager->RenderWeaponTrails();

	//// Enemy Faces
	//m_pEnemyManager->RenderFaces();
	//m_pEnemyManager->RenderWeaponTrails();

	//// Projectile trails
	//m_pProjectileManager->RenderWeaponTrails();

	//if (m_deferredRendering)
	//{
	//	m_pRenderer->StopRenderingToFrameBuffer(m_transparencyFrameBuffer);
	//}
	m_pRenderer->PopMatrix();
}

void VoxGame::Render()
{
// 	if (m_pVoxWindow->GetMinimized())
// 	{
// 		// Don't call any render functions if minimized
// 		return;
// 	}

	// Begin rendering
	m_pRenderer->BeginScene(true, true, true);

// 	// Shadow rendering to the shadow frame buffer
// 	if (m_pVoxSettings->m_shadows)
// 	{
// 		RenderShadows();
// 	}

	// ---------------------------------------
	// Render 3d
	// ---------------------------------------
	m_pRenderer->PushMatrix();
	// Set the default projection mode
	m_pRenderer->SetProjectionMode(PM_PERSPECTIVE, m_defaultViewport);

	// Enable vector normalization, for scaling and lighting
	m_pRenderer->EnableVectorNormalize();

	// Set back culling as default
	m_pRenderer->SetCullMode(CM_BACK);

	// Set default depth test
	m_pRenderer->EnableDepthTest(DT_LESS);

	// Set the lookat camera
	m_pGameCamera->Look();

// 	// Enable the lights
// 	m_pRenderer->PushMatrix();
// 	m_pRenderer->EnableLight(m_defaultLight, 0);
// 	m_pRenderer->PopMatrix();
// 
// 	// Multisampling MSAA
// 	if (m_multiSampling)
// 	{
// 		m_pRenderer->EnableMultiSampling();
// 	}
// 	else
// 	{
// 		m_pRenderer->DisableMultiSampling();
// 	}
// 
// 	// Water reflections
// 	if (m_pVoxSettings->m_waterRendering && m_pChunkManager->IsUnderWater(m_pGameCamera->GetPosition()) == false && m_gameMode != GameMode_FrontEnd)
// 	{
// 		RenderWaterReflections();
// 	}
// 
// 	// SSAO frame buffer rendering start
// 	if (m_deferredRendering)
// 	{
// 		m_pRenderer->StartRenderingToFrameBuffer(m_SSAOFrameBuffer);
// 	}
// 
// 	m_pRenderer->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);
// 	m_pRenderer->ClearScene(true, true, true);
// 
// 	// Render the lights (DEBUG)
// 	m_pRenderer->PushMatrix();
// 	m_pRenderer->SetCullMode(CM_BACK);
// 	m_pRenderer->SetRenderMode(RM_SOLID);
// 	m_pRenderer->RenderLight(m_defaultLight);
// 	m_pRenderer->PopMatrix();
// 
// 	// Render the skybox
// 	RenderSkybox();

	BeginShaderRender();
	{
		// Render the chunks
		m_pChunkManager->Render(false);
	}
	EndShaderRender();

// 	// NPC sub selection - For character creation screen
// 	m_pNPCManager->RenderSubSelectionNormalNPCs();
// 
// 	// Render items outline and silhouette before the world/chunks
// 	m_pItemManager->Render(true, false, false, false);
// 	m_pItemManager->Render(false, false, true, false);
// 
// 	// NPCs (only non outline and hover)
// 	m_pNPCManager->RenderOutlineNPCs();
// 	m_pNPCManager->Render(false, false, true, false, false, false);
// 
// 	// Enemies outline
// 	m_pEnemyManager->RenderOutlineEnemies();
// 	m_pEnemyManager->Render(false, false, true, false);
// 
// 	BeginShaderRender();
// 	{
// 		// Scenery
// 		m_pSceneryManager->Render(false, false, false, false, false);
// 
// 		// Projectiles
// 		m_pProjectileManager->Render();
// 
// 		// Items
// 		m_pItemManager->Render(false, false, false, false);
// 
// 		// NPCs
// 		m_pNPCManager->ResetNumRenderNPCs();
// 		m_pNPCManager->Render(false, false, false, false, true, false);
// 
// 		// Enemies
// 		m_pEnemyManager->Render(false, false, false, false);
// 
// 		// NPCs (only outline and hover)
// 		m_pNPCManager->Render(false, false, false, true, false, false);
// 		m_pNPCManager->RenderSubSelectionOverlayNPCs();
// 	}
// 	EndShaderRender();
// 
// 	// Player selection block
// 	m_pPlayer->RenderSelectionBlock();
// 
// 	// Render the block particles
// 	m_pBlockParticleManager->Render(false);
// 
// 	// Render the instanced objects
// 	if (m_instanceRender)
// 	{
// 		m_pInstanceManager->Render();
// 	}
// 
// 	// Frontend
// 	m_pFrontendManager->Render();
// 
// 	BeginShaderRender();
// 	{
// 
// 		if (m_gameMode != GameMode_FrontEnd)
// 		{
// 			// Render the player
// 			if (m_cameraMode != CameraMode_FirstPerson)
// 			{
// 				m_pPlayer->Render();
// 			}
// 		}
// 	}
// 	EndShaderRender();
// 
// 	// Render the transparency items above the water render, so that they appear properly under water
// 	if (m_pVoxSettings->m_waterRendering && m_gameMode != GameMode_FrontEnd)
// 	{
// 		if (m_cameraMode != CameraMode_FirstPerson)
// 		{
// 			m_pPlayer->RenderFace();
// 		}
// 		m_pNPCManager->RenderFaces();
// 		m_pEnemyManager->RenderFaces();
// 
// 		if (m_cameraMode != CameraMode_FirstPerson)
// 		{
// 			m_pPlayer->RenderWeaponTrails();
// 		}
// 		m_pNPCManager->RenderWeaponTrails();
// 		m_pEnemyManager->RenderWeaponTrails();
// 
// 		// Render water
// 		RenderWater();
// 	}
// 
// 	// Debug rendering
// 	if (m_debugRender)
// 	{
// 		m_pLightingManager->DebugRender();
// 
// 		m_pBlockParticleManager->RenderDebug();
// 
// 		if (m_gameMode != GameMode_FrontEnd)
// 		{
// 			m_pPlayer->RenderDebug();
// 		}
// 
// 		m_pNPCManager->RenderDebug();
// 
// 		m_pEnemyManager->RenderDebug();
//  
//  		m_pSceneryManager->RenderDebug();
//  
//  		m_pItemManager->RenderDebug();
//  
//  		m_pChunkManager->RenderDebug();
//  
//  		m_pProjectileManager->RenderDebug();
//  
//  		m_pBiomeManager->RenderDebug();
//  
//  		if (m_gameMode == GameMode_FrontEnd)
//  		{
//  			m_pFrontendManager->RenderDebug();
//  		}
//  	}
// 
// 	// Render player first person viewport
// 	if (m_gameMode != GameMode_FrontEnd)
// 	{
// 		if (m_cameraMode == CameraMode_FirstPerson)
// 		{
// 			RenderFirstPersonViewport();
// 		}
// 	}
// 
// 	// SSAO frame buffer rendering stop
// 	if (m_deferredRendering)
// 	{
// 		m_pRenderer->StopRenderingToFrameBuffer(m_SSAOFrameBuffer);
// 	}
// 	m_pRenderer->PopMatrix();
// 
// 	// Render the deferred lighting pass
// 	if (m_dynamicLighting)
// 	{
// 		RenderDeferredLighting();
// 	}
// 
// 	// Render other viewports
// 	// Paperdoll for CharacterGUI
// 	RenderPaperdollViewport();
// 	// Portrait for HUD
// 	if (m_pVoxSettings->m_renderGUI)
// 	{
// 		RenderPortraitViewport();
// 	}
// 
	// ---------------------------------------
	// Render transparency
	// ---------------------------------------
	RenderTransparency();

// 	// Render the SSAO texture
// 	if (m_deferredRendering)
// 	{
// 		RenderSSAOTexture();
// 
// 		if (m_multiSampling && m_fxaaShader != -1)
// 		{
// 			RenderFXAATexture();
// 		}
// 
// 		if (m_blur || m_pChunkManager->IsUnderWater(m_pGameCamera->GetPosition()))
// 		{
// 			RenderFirstPassFullScreen();
// 			RenderSecondPassFullScreen();
// 		}
// 	}
// 
// 	// ---------------------------------------
// 	// Render 2d
// 	// ---------------------------------------
// 	m_pRenderer->PushMatrix();
// 	// Disable multisampling for 2d gui and text
// 	m_pRenderer->DisableMultiSampling();
// 
// 	// Crosshair
// 	if (m_cameraMode == CameraMode_FirstPerson && m_bPaused == false)
// 	{
// 		if (m_pPlayer->IsDead() == false)
// 		{
// 			RenderCrosshair();
// 		}
// 	}
// 
// 	// Text effects
// 	m_pTextEffectsManager->Render();
// 
// 	// Cinematic mode (letter box)
// 	RenderCinematicLetterBox();
// 
// 	// Render the HUD
// 	if (m_pVoxSettings->m_renderGUI)
// 	{
// 		RenderHUD();
// 	}
// 	m_pRenderer->PopMatrix();
// 
// 	// Render other deferred rendering pipelines
// 	// Paperdoll SSAO for CharacterGUI
// 	RenderDeferredRenderingPaperDoll();
// 	// Portrait SSAO for HUD
// 	if (m_pVoxSettings->m_renderGUI)
// 	{
// 		RenderDeferredRenderingPortrait();
// 	}
// 
// 	// Render the chunks 2d (debug text information)
// 	if (m_debugRender)
// 	{
// 		//m_pChunkManager->Render2D(m_pGameCamera, m_defaultViewport, m_defaultFont);
// 	}
// 
// 	// Frontend 2D
// 	if (m_gameMode == GameMode_FrontEnd)
// 	{
// 		m_pRenderer->PushMatrix();
// 		m_pRenderer->SetProjectionMode(PM_2D, m_defaultViewport);
// 		m_pRenderer->SetCullMode(CM_BACK);
// 
// 		m_pRenderer->SetLookAtCamera(vec3(0.0f, 0.0f, 250.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
// 
// 		m_pFrontendManager->Render2D();
// 		m_pRenderer->PopMatrix();
// 	}
// 
// 	// Render the GUI
// 	RenderGUI();
// 
// 	// Custom cursor
// 	if (m_pVoxSettings->m_customCursors && m_bCustomCursorOn)
// 	{
// 		if (m_gameMode != GameMode_FrontEnd || m_pFrontendManager->GetFrontendScreen() != FrontendScreen_Intro)
// 		{
// 			RenderCustomCursor();
// 		}
// 	}
// 
// 	// Render debug information and text
// 	RenderDebugInformation();
// 
// 	// Update the NPC screen positions for select character screen
// 	m_pNPCManager->UpdateScreenCoordinates2d(m_pGameCamera);

	// End rendering
	m_pRenderer->EndScene();

// 	// Pass render call to the window class, allow to swap buffers
// 	m_pVoxWindow->Render();
}
