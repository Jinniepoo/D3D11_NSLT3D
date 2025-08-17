#include "stdafx.h"
#include "..\Public\Monster_CrumbleB.h"

#include "GameInstance.h"
#include "Player.h"
#include "Collectible_Coin.h"
#include "Effect_Startle.h"
#include "Effect_Dizzy.h"
#include "Level.h"
#include "UI_Heart_Filling.h"

CMonster_CrumbleB::CMonster_CrumbleB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
	, m_vVelocity{ XMVectorZero() }
	, m_fGravity{ -9.8f }
	, m_bIsAirborne{ false }
	, m_eLevel{ LEVEL_END }
	, m_fAttackTime{ 0.f }
{
}

CMonster_CrumbleB::CMonster_CrumbleB(const CMonster_CrumbleB& rhs)
	: CGameObject(rhs)
	, m_vVelocity{ rhs.m_vVelocity }
	, m_fGravity{ rhs.m_fGravity }
	, m_bIsAirborne{ rhs.m_bIsAirborne }
	, m_eLevel{ rhs.m_eLevel }
	, m_fAttackTime{ rhs.m_fAttackTime }
{
}

HRESULT CMonster_CrumbleB::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_CrumbleB::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_MonsterDesc = *(MONSTER_DESC*)pArg;

	m_eLevel = m_MonsterDesc.m_eLevel;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_MonsterDesc.vMonsterPos);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_MonsterDesc.fRotation));

	m_pModelCom->Set_AnimIndex(m_MonsterDesc.iInitialAnim, true);

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(m_eLevel, L"Layer_Player")->front());
	Safe_AddRef(m_pPlayer);

	m_pUIHeart = dynamic_cast<CUI_Heart_Filling*>(GET_LIST(LEVEL_STATIC, L"Layer_Heart_Filling")->front());
	Safe_AddRef(m_pUIHeart);

	m_vVelocity = XMVectorZero();
	m_fGravity = -9.8f;
	m_bIsAirborne = false;

	return S_OK;
}

void CMonster_CrumbleB::PriorityTick(_float fTimeDelta)
{
}

void CMonster_CrumbleB::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	_float3 vPlayerPos;
	XMStoreFloat3(&vPlayerPos, m_pPlayer->Get_Player_Transform()->Get_State(CTransform::STATE_POSITION));

	_float3 vMonsterPos;
	XMStoreFloat3(&vMonsterPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float fDistanceToPlayer = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPlayerPos) - XMLoadFloat3(&vMonsterPos)));

	if (fDistanceToPlayer < 10.f)
		m_bChasePlayer = true;
	else if (fDistanceToPlayer > 11.f)
		m_bChasePlayer = false;

	if (m_bIsAirborne)
	{
		if (m_bAirborneSound)
		{
			PLAYONLY(L"Monster_Airborne");
			VOLUME(L"Monster_Airborne", 50);
			m_bAirborneSound = false;
		}
		m_pModelCom->Set_AnimIndex(6, false);

		m_vVelocity = XMVectorSetY(m_vVelocity, XMVectorGetY(m_vVelocity) + m_fGravity * fTimeDelta);
		_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vCurrentPos += (m_vVelocity * fTimeDelta);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);

		_float fMonsterY = XMVectorGetY(m_MonsterDesc.vMonsterPos);

		if (XMVectorGetY(vCurrentPos) <= fMonsterY)
		{
			m_bIsAirborne = false;
			m_vVelocity = XMVectorZero();
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vCurrentPos, fMonsterY));
			Drop_Coins(vCurrentPos);
			this->Set_Dead(true);
		}
	}

	if (m_bChasePlayer)
	{
		if (m_bStartle)
		{
			PLAYONLY(L"Monster_Hurt");
			VOLUME(L"Monster_Hurt", 80);
			Startle_Effect(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pModelCom->Set_AnimIndex(5, false);
		}

		m_bStartle = false;
		Chase_Player(fTimeDelta);
	}
	else
		Move_Randomly(fTimeDelta);

	if (!m_bIsAirborne && m_bIsAttacked)
	{
		if (m_bChase)
		{
			PLAYONLY(L"Monster_Hurt");
			VOLUME(L"Monster_Hurt", 80);
			Dizzy_Effect(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pModelCom->Set_AnimIndex(3, false);
			m_bChase = false;
		}
	}

	if (m_bIsAttacked)
	{
		if (m_pModelCom->Get_Finished())
			m_bChasePlayer = true;
	}
	
	CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(m_eLevel, L"Layer_Player", L"Com_TailCollider", 0));
	CCollider* pPlayerBody = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(m_eLevel, L"Layer_Player", L"Com_Collider", 0));

	if (m_pColliderCom->Intersect(pPlayerBody))
	{
		if (!m_bPlayerDamaged)
		{
			m_pUIHeart->Decrease_Health();
			m_bPlayerDamaged = true;
		}
	}

	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		if (!m_bIsAttacked)
		{
			m_bIsAttacked = true;
		}
		else if (m_bIsAttacked == true)
		{
			m_bIsAirborne = true;
			m_vVelocity = XMVectorSet(2.0f, 10.0f, 2.0f, 0.0f);
		}
	}
}

void CMonster_CrumbleB::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CMonster_CrumbleB::Render()
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

HRESULT CMonster_CrumbleB::Ready_Components()
{
	if (LEVEL_GAMEPLAY == m_eLevel)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIGATION_DESC NavigationDesc{};
		NavigationDesc.iCurrentCellIndex = m_MonsterDesc.iCellIndex;

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;

		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_CrumbleB"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	if (LEVEL_PUZZLE == m_eLevel)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIGATION_DESC NavigationDesc{};
		NavigationDesc.iCurrentCellIndex = m_MonsterDesc.iCellIndex;

		if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;

		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Model_Monster_CrumbleB"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

	}
	if (LEVEL_BOSS == m_eLevel)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIGATION_DESC NavigationDesc{};
		NavigationDesc.iCurrentCellIndex = m_MonsterDesc.iCellIndex;

		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;

		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_Monster_CrumbleB"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	
	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc{};

	BoundingDesc.fRadius = 0.9f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_CrumbleB::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

_bool CMonster_CrumbleB::Near_Player()
{
	_vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPosition = m_pPlayer->Get_Player_Position();

	_vector vDistance = XMVector3Length(vPlayerPosition - vMonsterPosition);

	return XMVectorGetX(vDistance) < 5.0f;
}

void CMonster_CrumbleB::Chase_Player(_float fTimeDelta)
{
	_float3 vPlayerPos;
	_float3 vMonsterPos;

	XMStoreFloat3(&vPlayerPos, m_pPlayer->Get_Player_Position());
	XMStoreFloat3(&vMonsterPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_vector vDirection = XMVector3Normalize(XMLoadFloat3(&vPlayerPos) - XMLoadFloat3(&vMonsterPos));

	if (!m_bIsAirborne)
	{
		_vector vPlayerPosition = m_pPlayer->Get_Player_Position();
		m_pTransformCom->LookAt(vPlayerPosition);
		m_pTransformCom->Move(vDirection, fTimeDelta * 0.9f);

		if (m_pModelCom->Get_Finished())
			m_pModelCom->Set_AnimIndex(7, true);
	}
}

void CMonster_CrumbleB::Move_Randomly(_float fTimeDelta)
{
	static _float fTimeAccum = 0.f;
	fTimeAccum += fTimeDelta * 0.05;

	if (fTimeAccum > 2.f)
	{
		fTimeAccum = 0.f;

		_float fRandX = (rand() % 2001 / 100.0f - 10.0f);
		_float fRandZ = (rand() % 2001 / 100.0f - 10.0f);

		_float3 vCurrentPos;
		XMStoreFloat3(&vCurrentPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		_float3 vNewPos = { vCurrentPos.x + fRandX, vCurrentPos.y, vCurrentPos.z + fRandZ };

		if (vNewPos.x >= 59.5f && vNewPos.x <= 74.5f && vNewPos.z >= 29.0f && vNewPos.z <= 50.6f)
		{
			_vector vMoveDirection = XMVector3Normalize(XMVectorSet(fRandX, 0.f, fRandZ, 0.f));
			//cout << "RandomX: " << fRandX << " Random Z : " << fRandZ << endl;
			m_pTransformCom->Move(vMoveDirection, fTimeDelta * 0.9f);
			m_pModelCom->Set_AnimIndex(4, true);
		}
	}
}

HRESULT CMonster_CrumbleB::Drop_Coins(_vector vMonsterPos)
{
	_float4x4 matPos;
	_matrix matMonsterPos = XMMatrixIdentity();

	matMonsterPos.r[3] = vMonsterPos;
	XMStoreFloat4x4(&matPos, matMonsterPos);

	CCollectible_Coin::CLTB_DESC CltbDesc;
	CltbDesc.matModel = matPos;
	CltbDesc.m_CltbState = CCollectible_Coin::CLTB_DROP;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_Coins", L"Prototype_GameObject_Collectible_Coin", &CltbDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_CrumbleB::Startle_Effect(_vector vMonsterPos)
{
	CEffect_Startle::EFFECT_DESC StartleDesc;

	vMonsterPos = XMVectorAdd(vMonsterPos, XMVectorSet(0.f, 2.f, 0.f, 0.f));
	StartleDesc.vPos = vMonsterPos;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_Startle", L"Prototype_GameObject_Effect_Startle", &StartleDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_CrumbleB::Dizzy_Effect(_vector vMonsterPos)
{
	CEffect_Dizzy::EFFECT_DESC DizzyDesc;

	vMonsterPos = XMVectorAdd(vMonsterPos, XMVectorSet(0.f, 1.4f, 0.f, 0.f));
	DizzyDesc.vPos = vMonsterPos;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_Startle", L"Prototype_GameObject_Effect_Dizzy", &DizzyDesc)))
		return E_FAIL;

	return S_OK;
}

CMonster_CrumbleB* CMonster_CrumbleB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_CrumbleB* pInstance = new CMonster_CrumbleB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_CrumbleB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_CrumbleB::Clone(void* pArg)
{
	CMonster_CrumbleB* pInstance = new CMonster_CrumbleB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_CrumbleB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_CrumbleB::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	if (m_pPlayer)
		Safe_Release(m_pPlayer);

	if (m_pUIHeart)
		Safe_Release(m_pUIHeart);
}
