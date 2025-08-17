#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CMain_NSLT final : public CGameObject
{
private:
	CMain_NSLT(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_NSLT(const CMain_NSLT& rhs);
	virtual ~CMain_NSLT() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	PriorityTick(_float fTimeDelta);
	virtual void	Tick(_float fTimeDelta);
	virtual void	LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	/* NSLT */
	CShader*	m_pShaderCom = { nullptr };
	CTexture*	m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float						m_fX = {};
	_float						m_fY = {};
	_float						m_fSizeX = {};
	_float						m_fSizeY = {};
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

private:
	HRESULT Ready_Components();

public:
	static CMain_NSLT* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END