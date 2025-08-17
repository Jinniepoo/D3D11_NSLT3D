#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWoodenCrate_Heart final : public CGameObject
{
public:
	using CRATE_DESC = struct tagCrateDesc
	{
		_vector vPos;
	};
private:
	CWoodenCrate_Heart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWoodenCrate_Heart(const CWoodenCrate_Heart& rhs);
	virtual ~CWoodenCrate_Heart() = default;

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

	HRESULT Drop_Heart(_vector vPos);

private:
	CRATE_DESC		m_CrateDesc;

	_bool	Near_Player();

public:
	static CWoodenCrate_Heart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END