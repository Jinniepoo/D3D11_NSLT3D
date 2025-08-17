#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CNoHead3 final : public CGameObject
{
private:
	CNoHead3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNoHead3(const CNoHead3& rhs);
	virtual ~CNoHead3() = default;

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
	class CUI_Clover_Paw*	m_pPaw = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool	m_bBellCollision = { false };

public:
	static CNoHead3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END