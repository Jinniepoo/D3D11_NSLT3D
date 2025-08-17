#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CAnim_Boss final : public CGameObject
{
public:
	using BOSS_DESC = struct tagGolemDesc
	{
		_vector vPos = {};
	};

private:
	CAnim_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnim_Boss(const CAnim_Boss& rhs);
	virtual ~CAnim_Boss() = default;

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

	BOSS_DESC m_BossDesc;

public:
	static CAnim_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END