//FIXME：暂时我自己模拟的个假的
class LightingManager;
class BlockParticleManager;
class Player {
public:
	Player(	Renderer* pRenderer, ChunkManager* pChunkManager, QubicleBinaryManager* pQubicleBinaryManager,
			LightingManager* pLightingManager, BlockParticleManager* pBlockParticleManager);
	~Player();
	void Player::ResetPlayer();

	int GetGridX() const {
		return 0;
	}
	int GetGridY() const {
		return 0;
	}
	int GetGridZ() const {
		return 0;
	}

	Chunk*	m_pCachedGridChunk;
	void ClearChunkCacheForChunk(Chunk* pChunk);

	// Camera variables
	vec3 m_cameraPosition;
	vec3 m_cameraForward;
	vec3 m_cameraUp;
	vec3 m_cameraRight;
	void SetCameraPosition(vec3 cameraPos) {
		m_cameraPosition = cameraPos;
	}
	void SetCameraForward(vec3 cameraForward) {
		m_cameraForward = cameraForward;
	}
	void SetCameraUp(vec3 up) {
		m_cameraUp = up;
	}
	void SetCameraRight(vec3 right) {
		m_cameraRight = right;
	}

	static const vec3 PLAYER_CENTER_OFFSET;
	// Local axis
	vec3 m_forward;
	vec3 m_right;
	vec3 m_up;
	// Target forward / looking vector
	vec3 m_targetForward;
	void SetForwardVector(vec3 forward);

	vec3 m_position;
	float m_radius;
	bool m_bDoStepUpAnimation;
	float m_stepUpAnimationYOffset;
	vec3 GetCenter();

	// Rendering Helpers
	Matrix4x4 m_worldMatrix;
	void CalculateWorldTransformMatrix();
};
