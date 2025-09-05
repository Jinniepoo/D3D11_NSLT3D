#include "stdafx.h"
#include "..\Public\Burrow_Player.h"

#include "GameInstance.h"

CBurrow_Player::CBurrow_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBurrow_Player::CBurrow_Player(const CBurrow_Player& rhs)
	: CPartObject(rhs)
{
}

const _float4x4* CBurrow_Player::Get_BoneMatrixPtr(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

HRESULT CBurrow_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBurrow_Player::Initialize(void* pArg)
{
	BURROW_DESC* pDesc = static_cast<BURROW_DESC*>(pArg);

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

void CBurrow_Player::PriorityTick(_float fTimeDelta)
{
}

void CBurrow_Player::Tick(_float fTimeDelta)
{
	_matrix BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	Set_Animation(BURROW, 1, true);
	Set_Animation(BURROWWALK, 2, true);

	m_pModelCom->Play_Animation(fTimeDelta);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * BoneMatrix);
}

void CBurrow_Player::LateTick(_float fTimeDelta)
{
	if (*m_pState == BURROW || *m_pState == BURROWWALK)
	{
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_SHADOW, this);	
	}
}

HRESULT CBurrow_Player::Render()
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

HRESULT CBurrow_Player::Render_LightDepth()
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

HRESULT CBurrow_Player::Ready_Components()
{
	if (LEVEL_GAMEPLAY == m_eLevel)
	{	/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Burrow"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	if (LEVEL_PUZZLE == m_eLevel)
	{	/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Model_Player_Burrow"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}if (LEVEL_BOSS == m_eLevel)
	{	/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_Player_Burrow"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CBurrow_Player::Bind_ShaderResources()
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

void CBurrow_Player::Set_Animation(const unsigned int pState, _uint pIndex, _bool bAnim)
{
	if (*m_pState & pState)
	{
		m_pModelCom->Set_AnimIndex(pIndex, bAnim);
	}
}

CBurrow_Player* CBurrow_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBurrow_Player* pInstance = new CBurrow_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBody_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBurrow_Player::Clone(void* pArg)
{
	CBurrow_Player* pInstance = new CBurrow_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBurrow_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBurrow_Player::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
