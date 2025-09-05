#include "stdafx.h"
#include "..\Public\GolemHead.h"

#include "GameInstance.h"

CGolemHead::CGolemHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CGolemHead::CGolemHead(const CGolemHead& rhs)
	: CPartObject(rhs)
{
}

const _float4x4* CGolemHead::Get_BoneMatrixPtr(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

HRESULT CGolemHead::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGolemHead::Initialize(void* pArg)
{
	GOLEM_DESC* pDesc = static_cast<GOLEM_DESC*>(pArg);

	m_pBoneMatrix = pDesc->pBoneMatrix;
	m_pState = pDesc->pState;
	m_eLevel = pDesc->eLevel;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0, true);

	return S_OK;
}

void CGolemHead::PriorityTick(_float fTimeDelta)
{
}

void CGolemHead::Tick(_float fTimeDelta)
{
	if (*m_pState == CARRY)
	{
		m_pTransformCom->Set_Scale(_float3(1.35f, 1.35f, 1.35f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -0.35f, -0.1f, 1.f));

	}

	if (*m_pState == PICKUP)
	{
		m_pTransformCom->Set_Scale(_float3(1.25f, 1.25f, 1.25f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -0.9f, -0.5f, 1.f));
	}


	_matrix BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	m_pModelCom->Play_Animation(fTimeDelta);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * BoneMatrix);
}

void CGolemHead::LateTick(_float fTimeDelta)
{
	if (*m_pState == CARRY || *m_pState == PICKUP)
	{
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CGolemHead::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pModelCom->Bind_Buffers((_uint)i);
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CGolemHead::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4 pLightViewMatrix, pProjMatrix;

	XMStoreFloat4x4(&pLightViewMatrix, XMMatrixLookAtLH(XMVectorSet(-10.f, 30.f, -10.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&pProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pLightViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pProjMatrix)))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		if (FAILED(m_pModelCom->Bind_Buffers(i)))
			return E_FAIL;

		m_pModelCom->Render(i);		
	}

	return S_OK;
}

HRESULT CGolemHead::Ready_Components()
{
	if (LEVEL_GAMEPLAY == m_eLevel)
	{	/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim_GolemHead"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	if (LEVEL_PUZZLE == m_eLevel)
	{	/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Model_Anim_GolemHead"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	if (LEVEL_BOSS == m_eLevel)
	{	/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_Anim_GolemHead"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CGolemHead::Bind_ShaderResources()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CGolemHead::Set_Animation(const unsigned int pState, _uint pIndex, _bool bAnim)
{
	if (*m_pState & pState)
	{
		m_pModelCom->Set_AnimIndex(pIndex, bAnim);
	}
}

CGolemHead* CGolemHead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGolemHead* pInstance = new CGolemHead(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGolemHead"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGolemHead::Clone(void* pArg)
{
	CGolemHead* pInstance = new CGolemHead(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CGolemHead"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CGolemHead::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
