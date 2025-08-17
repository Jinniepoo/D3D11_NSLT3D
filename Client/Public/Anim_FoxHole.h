#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CAnim_Foxhole final : public CGameObject
{
public:
	using HOLE_DESC = struct tagHoleDesc
	{
		_vector vPos = {};
		_bool	bToPuzzle = { false };
	};

private:
	CAnim_Foxhole(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnim_Foxhole(const CAnim_Foxhole& rhs);
	virtual ~CAnim_Foxhole() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool    ToPuzzle() { return m_HoleDesc.bToPuzzle; }

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HOLE_DESC  m_HoleDesc;
	_bool	   m_bCollision = { true };

public:
	static CAnim_Foxhole* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END