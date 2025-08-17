#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Coins final : public CGameObject
{
public:
	using UI_DESC = struct tagUIDesc
	{
		LEVEL eLevel = { LEVEL_END };
	};
public:
	CUI_Coins(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Coins(const CUI_Coins& rhs);
	virtual ~CUI_Coins() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void	Set_Anim(_bool bAnim) { m_bAnim = bAnim; }
	static _int	Coin_Gained() { return m_iCoinScore; }
	void	Set_CoinScore() { m_iCoinScore++; }

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
	static _int					m_iCoinScore;

private:
	float		m_fAnimationTime = 0.f;
	_bool		m_bAnimating = false;

	_bool		m_bAnim = true;

	LEVEL		m_eLevel = { LEVEL_END };
	UI_DESC		m_UIDesc;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_Coins* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END