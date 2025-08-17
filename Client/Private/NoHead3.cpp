#include "stdafx.h"
#include "..\Public\NoHead3.h"

#include "GameInstance.h"
#include "Player.h"
#include "Anim_BabyGolem.h"
#include "UI_Clover_Paw.h"

CNoHead3::CNoHead3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CNoHead3::CNoHead3(const CNoHead3& rhs)
	: CGameObject( rhs )
{
}

HRESULT CNoHead3::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNoHead3::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(48.622f, 22.8921f, 173.704f, 1.f));

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player")->front());
	Safe_AddRef(m_pPlayer);

	if (m_pPlayer == nullptr)
		return E_FAIL;

	m_pPaw = dynamic_cast<CUI_Clover_Paw*>(GET_LIST(LEVEL_STATIC, L"Layer_Clover_Paw")->front());
	Safe_AddRef(m_pPaw);

	if (m_pPaw == nullptr)
		return E_FAIL;

	return S_OK;
}

void CNoHead3::PriorityTick(_float fTimeDelta)
{
}

void CNoHead3::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Collider", 0));

	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		/* BabyGolem */
		CAnim_BabyGolem::GOLEM_DESC GolemDesc;
		GolemDesc.vPos = { 48.622f, 22.8921f, 173.232f, 1.f };
		GolemDesc.fRot = -90.f;

		m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_BabyGolem", L"Prototype_GameObject_Anim_BabyGolem", &GolemDesc);
		PLAYONLY(L"Clover_Gained");
		VOLUME(L"Clover_Gained", 50);
		m_pPaw->Set_TextureIndex(1);
		this->Set_Dead(true);
	}
}


void CNoHead3::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CNoHead3::Render()
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

HRESULT CNoHead3::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NoHead3"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc{};

	BoundingDesc.vExtents = _float3(1.f, 1.f, 1.f);
	BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNoHead3::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CNoHead3* CNoHead3::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNoHead3* pInstance = new CNoHead3(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CNoHead3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNoHead3::Clone(void* pArg)
{
	CNoHead3* pInstance = new CNoHead3(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNoHead3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNoHead3::Free()
{
	__super::Free();
	
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	
	if (m_pPlayer)
		Safe_Release(m_pPlayer);

	if (m_pPaw)
		Safe_Release(m_pPaw);
}
