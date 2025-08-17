#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CSpring_Mushroom final : public CGameObject
{
public:
	using SPRING_DESC = struct tagSpringDesc
	{
		_vector vPos;
	};
private:
	CSpring_Mushroom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpring_Mushroom(const CSpring_Mushroom& rhs);
	virtual ~CSpring_Mushroom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	class CPlayer*	m_pPlayer = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	SPRING_DESC		m_SpringDesc;

	_bool	Near_Player();
	_bool	m_bSpring = { false };
	_float3	m_vScale = { 0.f, 0.f, 0.f };
	_float	m_fTime = { 0 };

public:
	static CSpring_Mushroom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END