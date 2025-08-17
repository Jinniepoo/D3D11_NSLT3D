#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMazeBell final : public CGameObject
{
private:
	CMazeBell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMazeBell(const CMazeBell& rhs);
	virtual ~CMazeBell() = default;

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

private:
	_bool	Near_Player();
	_bool	m_bBellCollision = { false };

public:
	static CMazeBell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END