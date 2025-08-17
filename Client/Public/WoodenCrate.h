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

class CWoodenCrate final : public CGameObject
{
public:
	using CRATE_DESC = struct tagCrateDesc
	{
		_vector vPos;
	};
private:
	CWoodenCrate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWoodenCrate(const CWoodenCrate& rhs);
	virtual ~CWoodenCrate() = default;

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

	HRESULT Drop_Coins(_vector vPos);

private:
	CRATE_DESC		m_CrateDesc;

public:
	static CWoodenCrate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END