#include "stdafx.h"
#include "..\Public\Anim_GolemHead.h"

#include "GameInstance.h"
#include "Player.h"
#include "Effect_Checkpoint.h"

CAnim_GolemHead::CAnim_GolemHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CAnim_GolemHead::CAnim_GolemHead(const CAnim_GolemHead& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnim_GolemHead::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnim_GolemHead::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_GolemDesc = *(GOLEM_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_GolemDesc.vPos);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(m_GolemDesc.fRot));
	m_pTransformCom->Set_Scale(_float3(1.5f, 1.5f, 1.5f));

	m_pModelCom->Set_AnimIndex(0, true);

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player")->front());
	Safe_AddRef(m_pPlayer);

	if (m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void CAnim_GolemHead::PriorityTick(_float fTimeDelta)
{
}

void CAnim_GolemHead::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	_float3 vPlayerPos;
	XMStoreFloat3(&vPlayerPos, m_pPlayer->Get_Player_Transform()->Get_State(CTransform::STATE_POSITION));


	CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Collider", 0));

	if (m_pColliderCom->Intersect(pTargetCollider) && m_bCollision)
	{
		this->Set_Dead(true);
	}

}

void CAnim_GolemHead::LateTick(_float fTimeDelta)
{
	if (m_pModelCom->Get_Finished())
		m_pModelCom->Set_AnimIndex(2, true);

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CAnim_GolemHead::Render()
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

HRESULT CAnim_GolemHead::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim_GolemHead"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc{};

	BoundingDesc.vExtents = { 1.f, 0.7f, 1.f };
	BoundingDesc.vCenter = { 0.f, BoundingDesc.vExtents.y, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CAnim_GolemHead::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CAnim_GolemHead* CAnim_GolemHead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnim_GolemHead* pInstance = new CAnim_GolemHead(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAnim_GolemHead"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAnim_GolemHead::Clone(void* pArg)
{
	CAnim_GolemHead* pInstance = new CAnim_GolemHead(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAnim_GolemHead"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnim_GolemHead::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	if (m_pPlayer)
		Safe_Release(m_pPlayer);
}
