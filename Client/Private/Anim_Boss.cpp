#include "stdafx.h"
#include "..\Public\Anim_Boss.h"

#include "GameInstance.h"

CAnim_Boss::CAnim_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CAnim_Boss::CAnim_Boss(const CAnim_Boss& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnim_Boss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnim_Boss::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_BossDesc = *(BOSS_DESC*)pArg;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_BossDesc.vPos);

	m_pModelCom->Set_AnimIndex(0, true);

	return S_OK;
}

void CAnim_Boss::PriorityTick(_float fTimeDelta)
{
}

void CAnim_Boss::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CAnim_Boss::LateTick(_float fTimeDelta)
{
	if (m_pModelCom->Get_Finished())
		m_pModelCom->Set_AnimIndex(2, true);

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CAnim_Boss::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pModelCom->Bind_Buffers(i);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CAnim_Boss::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Model_Anim_Boss"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CAnim_Boss::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CAnim_Boss* CAnim_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnim_Boss* pInstance = new CAnim_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAnim_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAnim_Boss::Clone(void* pArg)
{
	CAnim_Boss* pInstance = new CAnim_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAnim_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnim_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
