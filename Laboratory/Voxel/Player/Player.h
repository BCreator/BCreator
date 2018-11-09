//FIXME：暂时我自己模拟的个假的
class Player {
public:
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
	void Player::ClearChunkCacheForChunk(Chunk* pChunk)	{
		if (m_pCachedGridChunk == pChunk) {
			m_pCachedGridChunk = NULL;
		}
	}

	vec3 m_position;
	vec3 m_up;
	float m_radius;
	bool m_bDoStepUpAnimation;
	float m_stepUpAnimationYOffset;
	vec3 GetCenter() {
		vec3 center = m_position + m_up * m_radius;
		center -= vec3(0.0f, (m_bDoStepUpAnimation ? 0.0f : m_stepUpAnimationYOffset), 0.0f);

		return center;
	}
};
