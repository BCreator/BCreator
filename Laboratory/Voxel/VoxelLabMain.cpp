#include<iostream>
#include <stdio.h>

#include<boost/log/trivial.hpp>

#include"../../Runtime/c2Foundation/c2Part.h"
#include"../../Runtime/c2PreDefined.h"
#include"../../Runtime/c2DefEvent.h"
#include"../../Runtime/c2Application.h"

////////////////////////////////////////////////////////////////////////////////
class btAct4test : public BrainTree::Node {
	Status update() override {
		BOOST_LOG_TRIVIAL(info) << "  -> printed by BrainTree::Node.";
		return Node::Status::Success;
	}
};
static void ShowExampleAppCustomNodeGraph(bool* opened);
class onUpdateFixFrameVoxel : public c2IAction {
public:
	int _b_showsameline;
	onUpdateFixFrameVoxel() {
#if 1//just test
		auto repeater = std::make_shared<BrainTree::Repeater>(5);
		repeater->setChild(std::make_shared<btAct4test>());
		setRoot(repeater);
#endif
		_b_showsameline = 0;
	}
	virtual Status update() {
		ImGui::Begin("C2 Director");
		ImGui::Text("NLP");
		if (ImGui::Button("open"))
			_b_showsameline++;
		if (_b_showsameline >= 3)
			ImGui::Text("more than 3 times.");
		ImGui::SameLine();
		ImGui::End();
		return BehaviorTree::update();
	}
};

class onUpdateFrameVoxel : public c2IAction {
public:
	virtual Status update() {
		static const c2SysEvt::updateframe*  tpevt;
		BOOST_ASSERT(_pEvt);
		tpevt = static_cast<const c2SysEvt::updateframe*>(_pEvt);
// 		pVoxGame->Update(static_cast<int>(tpevt->_esElapsed));
// 		pVoxGame->PreRender();
// 		pVoxGame->Render();
		return Status::Success;
	}
};

////////////////////////////////////////////////////////////////////////////////
class onSysInitializedVoxel : public c2IAction {
	virtual Status update() {
		//TODO: I can plugin my extensions here.
		BOOST_LOG_TRIVIAL(info) << "C2engine intialized.";
// 		pVoxGame = VoxGame::GetInstance();
// 		pVoxGame->Init(1024, 768, 32, 8);

		return Status::Success;
	}
};

////////////////////////////////////////////////////////////////////////////////
static int main_templab(void);
int main_VoxelLab() {
	return main_templab();

	Uint32 syset_chunkoffet = 0;
	onSysInitializedVoxel osi;
	c2asActSubEvt(osi, syset_chunkoffet + c2SysET::initialized,
		sizeof(c2SysEvt::initialized));

	onUpdateFixFrameVoxel ouff;
	c2asActSubEvt(ouff, syset_chunkoffet + c2SysET::updatefixframe,
		sizeof(c2SysEvt::updatefixframe));

	onUpdateFrameVoxel ouf;
	c2asActSubEvt(ouf, syset_chunkoffet + c2SysET::updateframe,
		sizeof(c2SysEvt::updateframe));

	c2AppRun(true, 1, 1024, 768, "C2engine.Creator");

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
#include<windows.h>
#include"Renderer/Renderer.h"
#include"Renderer/camera.h"
#include"blocks/ChunkManager.h"
#include<GLFW/glfw3.h>
#include"VoxSettings.h"
#include"models/QubicleBinaryManager.h"
#include"blocks/BiomeManager.h"
#include"Player/Player.h"

// class c2Render {
// public:
// 	c2Render(int nWndWidth, int nWndHeight, int nDepthBits, int nStencilBits);
// 	bool createFrameBuffer();
// 	bool loadShader();
// };
// 
// class c2Camera {
// public:
// 	c2Camera(const c2Render &Render);
// };
// 
// class Role {
// 
// };

class PartQubicle {

};

class c2VoxelBelt {
	void BeginShaderRender()
	{
		glShader* pShader = NULL;
		m_pRenderer->BeginGLSLShader(m_defaultShader);
		pShader = m_pRenderer->GetShader(m_defaultShader);

		glUniform1iARB(glGetUniformLocationARB(pShader->GetProgramObject(), "enableFog"), true);
		float lfogEnd = m_pChunkManager->GetLoaderRadius() - Chunk::CHUNK_SIZE*Chunk::BLOCK_RENDER_SIZE;
		float lfogStart = lfogEnd - Chunk::CHUNK_SIZE*Chunk::BLOCK_RENDER_SIZE*2.0f;
		GLfloat fogColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, 1.0f);
		glHint(GL_FOG_HINT, GL_DONT_CARE);
		glFogf(GL_FOG_START, lfogStart);
		glFogf(GL_FOG_END, lfogEnd);
		glEnable(GL_FOG);
	}
	void EndShaderRender()
	{
		glDisable(GL_FOG);
		m_pRenderer->EndGLSLShader(m_defaultShader);
	}

public:
	VoxSettings* m_pVoxSettings;
	Renderer* m_pRenderer;
	Camera* m_pGameCamera;
	unsigned int m_defaultViewport;
	unsigned int m_defaultLight;
	vec3 m_defaultLightPosition;
	vec3 m_defaultLightView;
	unsigned int m_defaultMaterial;
	unsigned int m_defaultShader;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	ChunkManager* m_pChunkManager;
	BiomeManager* m_pBiomeManager;
	Player* m_pPlayer;
	GameMode m_gameMode;
public:
	c2VoxelBelt(int nWndWidth, int nWndHeight, int nDepthBits, int nStencilBits) {
		m_pVoxSettings = new VoxSettings();
		m_pVoxSettings->LoadSettings();
		m_pVoxSettings->LoadOptions();
		m_pRenderer = new Renderer(nWndWidth, nWndHeight, nDepthBits, nStencilBits);
		/* Create cameras */
		m_pGameCamera = new Camera(m_pRenderer);
		m_pGameCamera->SetPosition(vec3(8.0f, 8.25f, 15.5f));
		m_pGameCamera->SetFakePosition(m_pGameCamera->GetPosition());
		m_pGameCamera->SetFacing(vec3(0.0f, 0.0f, -1.0f));
		m_pGameCamera->SetUp(vec3(0.0f, 1.0f, 0.0f));
		m_pGameCamera->SetRight(vec3(1.0f, 0.0f, 0.0f));
		/* Create viewports */
		m_pRenderer->CreateViewport(0, 0, nWndWidth, nWndHeight, 60.0f, &m_defaultViewport);
		/* Create lights */
		m_defaultLightPosition = vec3(300.0f, 300.0f, 300.0f);
		m_defaultLightView = vec3(0.0f, 0.0f, 0.0f);
		vec3 lightDirection = m_defaultLightView - m_defaultLightPosition;
		m_pRenderer->CreateLight(Colour(1.0f, 1.0f, 1.0f, 1.0f), Colour(1.0f, 1.0f, 1.0f, 1.0f), Colour(0.0f, 0.0f, 0.0f, 1.0f),
			m_defaultLightPosition, lightDirection, 0.0f, 0.0f, 2.0f, 0.001f, 0.0f, true, false, &m_defaultLight);
		/* Create materials */
		m_pRenderer->CreateMaterial(Colour(1.0f, 1.0f, 1.0f, 1.0f), Colour(1.0f, 1.0f, 1.0f, 1.0f), Colour(1.0f, 1.0f, 1.0f, 1.0f), Colour(0.0f, 0.0f, 0.0f, 1.0f), 64, &m_defaultMaterial);
		m_pRenderer->LoadGLSLShader("media/shaders/default.vertex", "media/shaders/default.pixel", &m_defaultShader);
		/*geometry*/
		m_pQubicleBinaryManager = new QubicleBinaryManager(m_pRenderer);
		m_pChunkManager = new ChunkManager(m_pRenderer, m_pVoxSettings, m_pQubicleBinaryManager);
		m_pChunkManager->SetStepLockEnabled(m_pVoxSettings->m_stepUpdating);
		m_pBiomeManager = new BiomeManager(m_pRenderer);
		m_pPlayer = new Player(m_pRenderer, m_pChunkManager, m_pQubicleBinaryManager, nullptr, nullptr);
		m_pChunkManager->SetPlayer(m_pPlayer);
		m_pChunkManager->SetBiomeManager(m_pBiomeManager);
		m_pBiomeManager->AddSafeZone(vec3(21.0f, 8.5f, 20.0f), 25.f, 30.0f, 25.0f);
		m_pBiomeManager->AddTown(vec3(8.0f, 8.0f, 8.0f), 75.f, 15.0f, 75.0f);
//		m_pChunkManager->InitializeChunkCreation();
		m_pChunkManager->CreateNewChunk(m_pPlayer->GetGridX(), m_pPlayer->GetGridY(), m_pPlayer->GetGridZ());
		m_gameMode = GameMode_Game;
	}

	void update(float fDeltaTime) {
		m_pChunkManager->Update(fDeltaTime);
	}

	void render() {
		m_pRenderer->BeginScene(true, true, true);
		m_pRenderer->PushMatrix();
		{
			m_pRenderer->SetProjectionMode(PM_PERSPECTIVE, m_defaultViewport);
			m_pRenderer->EnableVectorNormalize();
			m_pRenderer->SetCullMode(CM_BACK);
			m_pRenderer->EnableDepthTest(DT_LESS);
			m_pGameCamera->Look();
			m_pRenderer->PushMatrix();
			m_pRenderer->EnableLight(m_defaultLight, 0);
			m_pRenderer->PopMatrix();
			m_pRenderer->SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);
			m_pRenderer->ClearScene(true, true, true);
			BeginShaderRender();
			{
				m_pChunkManager->Render(false, m_gameMode, m_defaultViewport, m_pGameCamera->GetPosition());
			}
			EndShaderRender();
		}
		m_pRenderer->PopMatrix();
		m_pRenderer->EndScene();
	}
};

////////////////////////////////////////////////////////////////////////////////
static int main_templab(void)
{
	GLFWwindow* window;
	if (!glfwInit())
		return -1;
	const int width = 1024, height = 768, depthbits = 32, stencilbits = 8;
	window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	c2VoxelBelt voxel_belt(width, height, depthbits, stencilbits);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		static float m_deltaTime;
		static float m_fps;
#ifdef _WIN32
		static LARGE_INTEGER m_fpsPreviousTicks;
		static LARGE_INTEGER m_fpsCurrentTicks;
		static LARGE_INTEGER m_fpsTicksPerSecond;
		QueryPerformanceCounter(&m_fpsPreviousTicks);
		QueryPerformanceCounter(&m_fpsCurrentTicks);
		QueryPerformanceFrequency(&m_fpsTicksPerSecond);
#else
		static double m_fpsPreviousTicks;
		static double m_fpsCurrentTicks;
		static struct timeval tm;
		gettimeofday(&tm, NULL);
		m_fpsCurrentTicks = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
		m_fpsPreviousTicks = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
#endif //_WIN32
		m_deltaTime = 0.0f;
		m_fps = 0.0f;

		voxel_belt.update(m_deltaTime);
		voxel_belt.render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
