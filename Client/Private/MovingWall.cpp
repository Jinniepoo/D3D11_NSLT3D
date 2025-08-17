#include "stdafx.h"
#include "..\Public\MovingWall.h"

#include "GameInstance.h"
#include "Player.h"

CMovingWall::CMovingWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMovingWall::CMovingWall(const CMovingWall& rhs)
	: CGameObject( rhs )
{
}

HRESULT CMovingWall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMovingWall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(87.413f, 21.079f, 10.714f, 1.f));
	m_pTransformCom->Set_Scale(_float3(1.f, 1.659f, 1.462f));

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player")->front());
	Safe_AddRef(m_pPlayer);

	if (m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void CMovingWall::PriorityTick(_float fTimeDelta)
{
}

void CMovingWall::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_TailCollider", 0));

	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		m_bBellCollision = true;
		PLAYONLY(L"MovingWall");
		VOLUME(L"MovingWall", 150);
	}

	if (m_bBellCollision)
	{
		static float fElapsedTime = 0.0f;
		static _vector vInitialPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		fElapsedTime += fTimeDelta;

		float fShakeAmplitude = 0.2f; 
		float fShakeFrequency = 40.0f;
		float fDownwardSpeed = 5.f;  

		float fShakingX = vInitialPosition.m128_f32[0] + (fShakeAmplitude * sin(fShakeFrequency * fElapsedTime));
		float fDownwardY = vInitialPosition.m128_f32[1] - (fElapsedTime * fDownwardSpeed);

		_vector vNewPosition = XMVectorSet(fShakingX, fDownwardY, vInitialPosition.m128_f32[2], 1.0f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPosition);

		if (fDownwardY < (vInitialPosition.m128_f32[1] - 50.0f)) 
		{
			m_bBellCollision = false;
			fElapsedTime = 0.0f;
			this->Set_Dead(true);
		}
	}
}


void CMovingWall::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CMovingWall::Render()
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

HRESULT CMovingWall::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MovingWall"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc{};

	BoundingDesc.vExtents = _float3(1.f, 1.f, 1.f);
	BoundingDesc.vCenter = _float3(1.f, 1.5f, 3.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMovingWall::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CMovingWall* CMovingWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMovingWall* pInstance = new CMovingWall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMovingWall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMovingWall::Clone(void* pArg)
{
	CMovingWall* pInstance = new CMovingWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMovingWall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMovingWall::Free()
{
	__super::Free();
	
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	
	if (m_pPlayer)
		Safe_Release(m_pPlayer);
}
