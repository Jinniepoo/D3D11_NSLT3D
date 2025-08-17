#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CEffect_Dizzy final : public CGameObject
{
public:
	using EFFECT_DESC = struct tagEffectDesc {
		_vector vPos;
	};

private:
	CEffect_Dizzy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Dizzy(const CEffect_Dizzy& rhs);
	virtual ~CEffect_Dizzy() = default;

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

	EFFECT_DESC					m_EffectDesc;

private:
	HRESULT Ready_Components();

public:
	static CEffect_Dizzy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END