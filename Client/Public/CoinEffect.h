#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CCoinEffect final : public CGameObject
{
public:
	using COINEFFECT_DESC = struct tagCoinEffectDesc {
		_vector vPos;
	};

private:
	CCoinEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCoinEffect(const CCoinEffect& rhs);
	virtual ~CCoinEffect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };

	_float						m_fFrame = {};
	_bool						m_bNoLoop = false;

	COINEFFECT_DESC				m_CoinEffectDesc;

private:
	HRESULT Ready_Components();

public:
	static CCoinEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END