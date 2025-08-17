#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CGolemHead final : public Engine::CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_uint*			 pState;
		LEVEL			 eLevel = { LEVEL_END };
	}GOLEM_DESC;

private:
	CGolemHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGolemHead(const CGolemHead& rhs);
	virtual ~CGolemHead() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	PriorityTick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	_uint*					m_pState = { nullptr };

	LEVEL					m_eLevel = { LEVEL_END };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	const _float4x4*		m_pBoneMatrix = { nullptr };

private:
	void Set_Animation(const unsigned int pState, _uint pIndex, _bool bAnim);

public:
	static CGolemHead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END