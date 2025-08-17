#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_Clover_L final : public CGameObject
{
public:
	using UI_DESC = struct tagUIDesc
	{
		LEVEL eLevel = { LEVEL_END };
	};

private:
	CUI_Clover_L(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Clover_L(const CUI_Clover_L& rhs);
	virtual ~CUI_Clover_L() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void	Set_TextureIndex(_int iTextureIdx) { m_iTextureIdx = iTextureIdx; }

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float						m_fX = {};
	_float						m_fY = {};
	_float						m_fSizeX = {};
	_float						m_fSizeY = {};
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

private:
	float		m_fAnimationTime = 0.f;
	_bool		m_bAnimating = false;

	_int	    m_iTextureIdx = { 0 };
	_bool		m_bAnim = true;

	LEVEL		m_eLevel = { LEVEL_END };
	UI_DESC		m_UIDesc;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_Clover_L* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END