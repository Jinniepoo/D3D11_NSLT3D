#include "stdafx.h"
#include "..\Public\MazeBell.h"

#include "GameInstance.h"
#include "Player.h"

CMazeBell::CMazeBell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMazeBell::CMazeBell(const CMazeBell& rhs)
	: CGameObject( rhs )
{
}

HRESULT CMazeBell::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMazeBell::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(89.5f, 29.5f, 16.5f, 1.f));
	m_pTransformCom->Set_Scale(_float3(2.5f, 2.5f, 2.5f));

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player")->front());
	Safe_AddRef(m_pPlayer);

	if (m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void CMazeBell::PriorityTick(_float fTimeDelta)
{
}

void CMazeBell::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_TailCollider", 0));

	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		PLAYONLY(L"BellRing");
		VOLUME(L"BellRing", 50);
		
		m_bBellCollision = true;
	}

	if (m_bBellCollision)
	{
		static float fElapsedTime = 0.0f;
		fElapsedTime += fTimeDelta;

		float fAngle = sin(fElapsedTime * 5.0f) * XMConvertToRadians(45.0f) * exp(-fElapsedTime * 0.5f);
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), fAngle);

		if (abs(fAngle) < 0.01f)
		{
			m_bBellCollision = false;
			fElapsedTime = 0.0f;
			m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 0.0f);
		}
	}
}


void CMazeBell::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CMazeBell::Render()
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

HRESULT CMazeBell::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MazeBell"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc{};

	BoundingDesc.vExtents = _float3(0.5f, 3.f, 0.5f);
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMazeBell::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

_bool CMazeBell::Near_Player()
{
	_vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPosition = m_pPlayer->Get_Player_Position();

	_vector vDistance = XMVector3Length(vPlayerPosition - vMonsterPosition);

	return XMVectorGetX(vDistance) < 5.0f;
}

CMazeBell* CMazeBell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMazeBell* pInstance = new CMazeBell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMazeBell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMazeBell::Clone(void* pArg)
{
	CMazeBell* pInstance = new CMazeBell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMazeBell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMazeBell::Free()
{
	__super::Free();
	
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	
	if (m_pPlayer)
		Safe_Release(m_pPlayer);
}
