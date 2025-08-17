#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Lives final : public CGameObject
{
public:
	using UI_DESC = struct tagUIDesc
	{
		LEVEL eLevel = { LEVEL_END };
	};

public:
	CUI_Lives(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Lives(const CUI_Lives& rhs);
	virtual ~CUI_Lives() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	static _int	Get_PlayerLife() { return m_iPlayerLife; }
	void	Increase_PlayerLife() { m_iPlayerLife++; }
	void	Decrease_PlayerLife() { m_iPlayerLife--; }

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


	static _int					m_iPlayerLife;

	LEVEL		m_eLevel = { LEVEL_END };
	UI_DESC		m_UIDesc;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_Lives* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END