#include "stdafx.h"
#include "..\Public\Anim_BabyGolem.h"

#include "GameInstance.h"
#include "Player.h"

CAnim_BabyGolem::CAnim_BabyGolem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CAnim_BabyGolem::CAnim_BabyGolem(const CAnim_BabyGolem& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnim_BabyGolem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnim_BabyGolem::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_GolemDesc = *(GOLEM_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_GolemDesc.vPos);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_GolemDesc.fRot));

	m_pModelCom->Set_AnimIndex(8, false);
	PLAYONLY(L"Golem_GainedHead");
	VOLUME(L"Golem_GainedHead", 70);

	return S_OK;
}

void CAnim_BabyGolem::PriorityTick(_float fTimeDelta)
{
}

void CAnim_BabyGolem::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CAnim_BabyGolem::LateTick(_float fTimeDelta)
{
	if (m_pModelCom->Get_Finished() && m_bInitialAnim)
	{
		m_pModelCom->Set_AnimIndex(6, false); 
		PLAYONLY(L"Golem_ShortTalk");
		VOLUME(L"Golem_ShortTalk", 70);
		m_bInitialAnim = false;
	}

	if (m_pModelCom->Get_Finished())
	{
		PLAYONLY(L"Golem_LastTalk");
		VOLUME(L"Golem_LastTalk", 70);
		m_pModelCom->Set_AnimIndex(0, true); 
	}

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CAnim_BabyGolem::Render()
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

HRESULT CAnim_BabyGolem::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim_BabyGolem"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CAnim_BabyGolem::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CAnim_BabyGolem* CAnim_BabyGolem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnim_BabyGolem* pInstance = new CAnim_BabyGolem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAnim_BabyGolem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAnim_BabyGolem::Clone(void* pArg)
{
	CAnim_BabyGolem* pInstance = new CAnim_BabyGolem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAnim_BabyGolem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnim_BabyGolem::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
