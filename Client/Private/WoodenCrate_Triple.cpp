#include "stdafx.h"
#include "WoodenCrate_Triple.h"

#include "GameInstance.h"
#include "Player.h"
#include "Collectible_Coin.h"

CWoodenCrate_Triple::CWoodenCrate_Triple(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CWoodenCrate_Triple::CWoodenCrate_Triple(const CWoodenCrate_Triple& rhs)
	: CGameObject(rhs)
{
}

HRESULT CWoodenCrate_Triple::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWoodenCrate_Triple::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_CrateDesc = *(CRATE3_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_CrateDesc.vPos);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), m_CrateDesc.fRot);

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player")->front());
	Safe_AddRef(m_pPlayer);

	if (m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void CWoodenCrate_Triple::PriorityTick(_float fTimeDelta)
{
}

void CWoodenCrate_Triple::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_TailCollider", 0));
	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		Drop_Coins(m_CrateDesc.vPos);
		this->Set_Dead(true);
	}
}

void CWoodenCrate_Triple::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CWoodenCrate_Triple::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Bind_Buffers(i);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CWoodenCrate_Triple::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WoodenCrate_Triple"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc{};

	BoundingDesc.vExtents = { 1.5f, 1.5f, 1.5f };
	BoundingDesc.vCenter = { 0.f, 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWoodenCrate_Triple::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWoodenCrate_Triple::Drop_Coins(_vector vPos)
{
	_float4x4 matPos;
	_matrix matMonsterPos = XMMatrixIdentity();

	matMonsterPos.r[3] = vPos;
	XMStoreFloat4x4(&matPos, matMonsterPos);

	CCollectible_Coin::CLTB_DESC CltbDesc;
	CltbDesc.matModel = matPos;
	CltbDesc.m_CltbState = CCollectible_Coin::CLTB_DROP;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_Coins", L"Prototype_GameObject_Collectible_Coin", &CltbDesc)))
		return E_FAIL;

	return S_OK;
}

CWoodenCrate_Triple* CWoodenCrate_Triple::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWoodenCrate_Triple* pInstance = new CWoodenCrate_Triple(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWoodenCrate_Triple"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWoodenCrate_Triple::Clone(void* pArg)
{
	CWoodenCrate_Triple* pInstance = new CWoodenCrate_Triple(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWoodenCrate_Triple"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWoodenCrate_Triple::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	if (m_pPlayer)
		Safe_Release(m_pPlayer);
}
