#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CAnim_BabyGolem final : public CGameObject
{
public:
	using GOLEM_DESC = struct tagGolemDesc
	{
		_vector vPos = {};
		_float	fRot = {};
	};

private:
	CAnim_BabyGolem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnim_BabyGolem(const CAnim_BabyGolem& rhs);
	virtual ~CAnim_BabyGolem() = default;

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

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	GOLEM_DESC m_GolemDesc;
	_bool	   m_bInitialAnim = { true };

public:
	static CAnim_BabyGolem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END