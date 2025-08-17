#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Player.h"

BEGIN(Client)

class CCamera_Player final : public CCamera
{
private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	PriorityTick(_float fTimeDelta);
	virtual void	Tick(_float fTimeDelta);
	virtual void	LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	void	SetOriginPos(const _float3& vRelativeDistFromPlayer);

public:
	void	ActivatePlayerCam(_bool bActivate) { m_bPlayerCam = bActivate; }

private:
	_float	m_fMouseSensor = {};
	_float  m_fSpeedPerSec = {};
	_bool	m_bPlayerCam = { true };

protected:
	CPlayer*	m_pPlayer = { nullptr };

	_float3		m_vPlayerPosition = { 0.f, 0.f, 0.f };
	_float3		m_vCamPosOrigin = { 0.f, 0.f, 0.f };
	_float3		m_vCamPosPrev = { 0.f, 0.f, 0.f };

	LEVEL		m_eLevel = { LEVEL_END };
public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END