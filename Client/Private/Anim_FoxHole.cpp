#include "stdafx.h"
#include "..\Public\Anim_FoxHole.h"

#include "GameInstance.h"
#include "Player.h"
#include "Effect_Checkpoint.h"
#include "Level_Loading.h"

CAnim_Foxhole::CAnim_Foxhole(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CAnim_Foxhole::CAnim_Foxhole(const CAnim_Foxhole& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnim_Foxhole::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnim_Foxhole::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_HoleDesc = *(HOLE_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_HoleDesc.vPos);

	m_pModelCom->Set_AnimIndex(0, true);

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player")->front());
	Safe_AddRef(m_pPlayer);

	if (m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void CAnim_Foxhole::PriorityTick(_float fTimeDelta)
{
}

void CAnim_Foxhole::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	_float3 vPlayerPos;
	XMStoreFloat3(&vPlayerPos, m_pPlayer->Get_Player_Transform()->Get_State(CTransform::STATE_POSITION));


	CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Collider", 0));

	if (m_pColliderCom->Intersect(pTargetCollider) && m_bCollision)
	{
		m_pModelCom->Set_AnimIndex(1, false);
		m_HoleDesc.bToPuzzle = true;
		PLAYONLY(L"To_Puzzle");
		VOLUME(L"To_Puzzle", 50);
	}

}

void CAnim_Foxhole::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CAnim_Foxhole::Render()
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

HRESULT CAnim_Foxhole::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim_FoxHole"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc{};

	BoundingDesc.vExtents = { 0.3f, 0.2f, 0.3f };
	BoundingDesc.vCenter = { -BoundingDesc.vExtents.z - 0.2f, BoundingDesc.vExtents.y, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CAnim_Foxhole::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CAnim_Foxhole* CAnim_Foxhole::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnim_Foxhole* pInstance = new CAnim_Foxhole(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAnim_Foxhole"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAnim_Foxhole::Clone(void* pArg)
{
	CAnim_Foxhole* pInstance = new CAnim_Foxhole(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAnim_Foxhole"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnim_Foxhole::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	if (m_pPlayer)
		Safe_Release(m_pPlayer);
}
