#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Heart_Filling final : public CGameObject
{
public:
	typedef struct
	{
		_float	fX = {};
		_float	fY = {};
		LEVEL	eLevel = { LEVEL_END };
	}HEART_DESC;

public:
	CUI_Heart_Filling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Heart_Filling(const CUI_Heart_Filling& rhs);
	virtual ~CUI_Heart_Filling() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	PriorityTick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Decrease_Health() { m_iTextureIdx++; }
	void			Increase_Health() { m_iTextureIdx--; }
	void			Reset_Health() { m_iTextureIdx = 0; }
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	class CUI_Lives* m_pLives = { nullptr };

private:
	HRESULT			Ready_Components();
	HRESULT			Bind_ShaderResources();

private:
	_float			m_fX = {};
	_float			m_fY = {};
	_float			m_fSizeX = {};
	_float			m_fSizeY = {};
	_float4x4		m_ViewMatrix = {};
	_float4x4		m_ProjMatrix = {};

	LEVEL			m_eLevel = { LEVEL_END };
	static _int			m_iTextureIdx;
public:
	static CUI_Heart_Filling* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END