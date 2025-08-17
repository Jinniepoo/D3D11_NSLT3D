#include "stdafx.h"
#include "Spring_Mushroom.h"

#include "GameInstance.h"
#include "Player.h"

CSpring_Mushroom::CSpring_Mushroom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSpring_Mushroom::CSpring_Mushroom(const CSpring_Mushroom& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpring_Mushroom::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpring_Mushroom::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_SpringDesc = *(SPRING_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_vScale = { 0.3f, 0.3f, 0.3f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_SpringDesc.vPos);
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Set_Scale(m_vScale);

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player")->front());
	Safe_AddRef(m_pPlayer);

	if (m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void CSpring_Mushroom::PriorityTick(_float fTimeDelta)
{
}

void CSpring_Mushroom::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (!m_bSpring)
	{
		CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Collider", 0));
		if (m_pColliderCom->Intersect(pTargetCollider))
		{
			m_bSpring = true;
			PLAYONLY(L"Spring");
			VOLUME(L"Spring", 80);
			m_pPlayer->Set_PlayerJumpPower(3.5f);
			m_fTime = 0.f;
		}
	}

	if (m_bSpring)
	{
		if (m_fTime < 0.15f)
		{
			float fTime = m_fTime / 0.15f;
			m_vScale.x = 0.3f + fTime * 0.1f;
			m_vScale.y = 0.3f - fTime * 0.1f;
			m_vScale.z = 0.3f - fTime * 0.1f;
		}
		else if (m_fTime < 0.3f)
		{
			float fTime = (m_fTime - 0.15f) / 0.15f;
			m_vScale.x = 0.4f - fTime * 0.15f;
			m_vScale.y = 0.2f + fTime * 0.05f;
			m_vScale.z = 0.2f + fTime * 0.1f;
		}
		else if (m_fTime < 0.5f)
		{
			float fTime = (m_fTime - 0.3f) / 0.2f;
			m_vScale.x = 0.25f + fTime * 0.05f;
			m_vScale.y = 0.25f + fTime * 0.05f;
			m_vScale.z = 0.3f;
		}
		else
		{
			m_bSpring = false;
			m_vScale = { 0.3f, 0.3f, 0.3f };
		}
	}
	m_pTransformCom->Set_Scale(m_vScale);
}

void CSpring_Mushroom::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CSpring_Mushroom::Render()
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

HRESULT CSpring_Mushroom::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Spring_Mushroom"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc{};

	BoundingDesc.vExtents = { 2.f, 1.f, 2.f };
	BoundingDesc.vCenter = { 0.f, BoundingDesc.vExtents.y, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpring_Mushroom::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

_bool CSpring_Mushroom::Near_Player()
{
	_vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPosition = m_pPlayer->Get_Player_Position();

	_vector vDistance = XMVector3Length(vPlayerPosition - vMonsterPosition);

	return XMVectorGetX(vDistance) < 5.0f;
}

CSpring_Mushroom* CSpring_Mushroom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpring_Mushroom* pInstance = new CSpring_Mushroom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpring_Mushroom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpring_Mushroom::Clone(void* pArg)
{
	CSpring_Mushroom* pInstance = new CSpring_Mushroom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpring_Mushroom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpring_Mushroom::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	if (m_pPlayer)
		Safe_Release(m_pPlayer);
}
