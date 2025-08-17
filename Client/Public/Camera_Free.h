#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	PriorityTick(_float fTimeDelta);
	virtual void	Tick(_float fTimeDelta);
	virtual void	LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	void	Camera_KeyInput(_float fTimeDelta);
	void	Camera_MouseInput(_float fTimeDelta);
	
public:
	void	OnOffDebugCam(_bool bTrueForOn) { m_bDebugCam = bTrueForOn; }
	void	OnOffMouseCam(_bool bTrueForOn) { m_bMouseCam = bTrueForOn; }
	 
private:
	_bool	m_bDebugCam = { false };
	_bool	m_bMouseCam = { false };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END