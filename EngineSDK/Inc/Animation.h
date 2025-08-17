#pragma once

#include "Base.h"

/* 애니메이션의 하나(공격모션)의 정보를 가진다. */
BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	_bool Get_Finished() const {
		return m_isFinished;
	}

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);
	void Reset();

private:
	_char				m_szName[MAX_PATH] = {};
	_float				m_fDuration = {}; 
	_float				m_fTickPerSecond = {}; 
	_float				m_fCurrentTrackPosition = {}; 


	_uint						m_iNumChannels = {};
	vector<class CChannel*>		m_Channels;

	vector<_uint>				m_CurrentKeyFrameIndices;

	_bool						m_isFinished = { false };


public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END
