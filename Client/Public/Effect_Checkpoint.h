#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CEffect_Checkpoint final : public CGameObject
{
public:
	using CPEFFECT_DESC = struct tagCPEffectDesc {
		_vector vPos;
	};

private:
	CEffect_Checkpoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Checkpoint(const CEffect_Checkpoint& rhs);
	virtual ~CEffect_Checkpoint() = default;

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

	CPEFFECT_DESC				m_CPEffectDesc;

private:
	HRESULT Ready_Components();

public:
	static CEffect_Checkpoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END