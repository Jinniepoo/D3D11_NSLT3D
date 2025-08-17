#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CAnim_Guard final : public CGameObject
{
public:
	using GUARD_DESC = struct tagGuardDesc
	{
		_vector vPos = {};
	};

private:
	CAnim_Guard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnim_Guard(const CAnim_Guard& rhs);
	virtual ~CAnim_Guard() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	GUARD_DESC m_GuardDesc;
	_bool	   m_bCollision = { true };

public:
	static CAnim_Guard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END