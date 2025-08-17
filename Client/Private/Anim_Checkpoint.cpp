#include "stdafx.h"
#include "..\Public\Anim_Checkpoint.h"

#include "GameInstance.h"
#include "Player.h"
#include "Effect_Checkpoint.h"

CCheckpoint::CCheckpoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCheckpoint::CCheckpoint(const CCheckpoint& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCheckpoint::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCheckpoint::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_CheckDesc = *(CHECK_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_CheckDesc.vCheckPos);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(225.f));
	m_pTransformCom->Set_Scale(_float3(1.5f, 1.5f, 1.5f));

	m_pModelCom->Set_AnimIndex(0, true);

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player")->front());
	Safe_AddRef(m_pPlayer);

	if (m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void CCheckpoint::PriorityTick(_float fTimeDelta)
{
}

void CCheckpoint::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	//m_pModelCom->Get_BoneMatrixPtr("Bone Tail04");

	_float3 vPlayerPos;
	XMStoreFloat3(&vPlayerPos, m_pPlayer->Get_Player_Transform()->Get_State(CTransform::STATE_POSITION));


	CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_TailCollider", 0));

	if (m_pColliderCom->Intersect(pTargetCollider) && m_bCollision)
	{
		m_pModelCom->Set_AnimIndex(1, false);

		CEffect_Checkpoint::CPEFFECT_DESC CPEffectDesc;
		CPEffectDesc.vPos = m_CheckDesc.vCheckPos;

		_float3 vTemp;
		XMStoreFloat3(&vTemp, m_CheckDesc.vCheckPos);
		
		vTemp.x -= 5.f;
		vTemp.y += 8.f;
		vTemp.z += 6.f;

		CPEffectDesc.vPos = XMVectorSetW(XMLoadFloat3(&vTemp), 1.f);
		m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_Effect_Checkpoint", L"Prototype_GameObject_Effect_Checkpoint", &CPEffectDesc);
		m_bCollision = false;
	}

}

void CCheckpoint::LateTick(_float fTimeDelta)
{
	if (m_pModelCom->Get_Finished())
		m_pModelCom->Set_AnimIndex(2, true);

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CCheckpoint::Render()
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

HRESULT CCheckpoint::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim_Checkpoint"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc{};

	BoundingDesc.vExtents = { 7.f, 0.5f, 0.f };
	BoundingDesc.vCenter = { 11.5f, BoundingDesc.vExtents.y + 2.8f, 4.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCheckpoint::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CCheckpoint* CCheckpoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCheckpoint* pInstance = new CCheckpoint(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCheckpoint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCheckpoint::Clone(void* pArg)
{
	CCheckpoint* pInstance = new CCheckpoint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCheckpoint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCheckpoint::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	if (m_pPlayer)
		Safe_Release(m_pPlayer);
}
