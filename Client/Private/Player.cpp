#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "Body_Player.h"
#include "Burrow_Player.h"
#include "GolemHead.h"
#include "Level.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
	, m_pBodyPlayer(nullptr)
	, m_iState(IDLE)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}
HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_PlayerDesc = *(PLAYER_DESC*)pArg;

	CTransform::TRANSFORM_DESC		Desc{};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	m_eLevel = m_PlayerDesc.m_eLevel;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	_float3 vInitialPos;
	if (m_eLevel == LEVEL_GAMEPLAY)
		vInitialPos = _float3(32.23f, m_pNavigationCom->Get_CellHeight(), 16.09f); 
	if (m_eLevel == LEVEL_PUZZLE)
		vInitialPos = _float3(5.f, m_pNavigationCom->Get_CellHeight(), 5.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vInitialPos.x, vInitialPos.y, vInitialPos.z, 1.f));
	m_pNavigationCom->Set_PlayerPos(vInitialPos);

	return S_OK;
}

void CPlayer::PriorityTick(_float fTimeDelta)
{
	for (auto& pPartObj : m_Parts)
		pPartObj->PriorityTick(fTimeDelta);

	Player_KeyInput(fTimeDelta);

	if (m_bJump)
		Player_Jump(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

}

void CPlayer::Tick(_float fTimeDelta)
{
	for (auto& pPartObj : m_Parts)
		pPartObj->Tick(fTimeDelta);

	const _float4x4 TailBoneMatrix = *(m_pModelCom->Get_BoneMatrixPtr("Bone Tail04"));
	_matrix matTailBone = XMLoadFloat4x4(&TailBoneMatrix);

	_matrix TailWorldMatrix = matTailBone * m_pTransformCom->Get_WorldMatrix();
	m_pTailColliderCom->Update(TailWorldMatrix);
}

void CPlayer::LateTick(_float fTimeDelta)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_PlayerPos(vPos);

	if (vPos.y > m_pNavigationCom->Get_CellHeight() + 1)
		m_bLanding = true;

	for (auto& pPartObj : m_Parts)
		pPartObj->LateTick(fTimeDelta);
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

	if (false == m_bJump)
		vPos.y = m_pNavigationCom->Get_CellHeight();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));

	if (KEY_PUSH(DIK_P))
		cout << "[Player Pos]" << " X: " << vPos.x << " Y: " << vPos.y << " Z: " << vPos.z << endl;

	_int iCurIndex;
	iCurIndex = m_pNavigationCom->Get_CurrentCellIndex();

	if (KEY_PUSH(DIK_C))
		cout << "Current Cell Index: " << iCurIndex << endl;

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
	m_pGameInstance->Add_DebugComponent(m_pTailColliderCom);
#endif
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

void CPlayer::Player_Jump(_float fTimeDelta)
{
	_float3 vPos;

	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fTime += fTimeDelta * 0.8f;

	vPos.y += (m_fJumpPower * m_fTime) - (m_fGravity * m_fTime * m_fTime * 0.4f);
	m_pNavigationCom->Set_PlayerPos(vPos);

	if (!m_bBeforeJump)
		vPos.y += 0.2f;

	if (0.4f >= m_fTime)
		m_bBeforeJump = true;

	if (vPos.y <= m_pNavigationCom->Get_CellHeight())
	{
		vPos.y = m_pNavigationCom->Get_CellHeight();
		m_fTime = 0.f;
		m_bBeforeJump = false;
		m_bJump = false;
		m_bLanding = false;
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
}

void CPlayer::Player_KeyInput(_float fTimeDelta)
{
	auto process_turn = [&](float fDelta) {
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fDelta);
		};

	if (KEY_HOLD(DIK_DOWN))
	{
		m_pTransformCom->Go_Backward(fTimeDelta * 0.9f, m_pNavigationCom);
		m_iState |= WALK;
		m_iState &= ~IDLE;

		if (KEY_HOLD(DIK_LEFT)) process_turn(-fTimeDelta);
		if (KEY_HOLD(DIK_RIGHT)) process_turn(fTimeDelta);
	}
	else if (KEY_HOLD(DIK_UP) && !m_bJump)
	{
		PLAYONLY(L"Player_FootStep");
		VOLUME(L"Player_FootStep", 50);
		m_fJumpPower = 1.4f;
		_float3 vPos;
		XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Go_Straight(fTimeDelta * 0.9f, m_pNavigationCom);
		m_iState |= WALK;
		m_iState &= ~IDLE;

		if (KEY_HOLD(DIK_LEFT)) process_turn(-fTimeDelta * 0.8f);
		if (KEY_HOLD(DIK_RIGHT)) process_turn(fTimeDelta * 0.8f);
	}
	else
	{
		if (KEY_HOLD(DIK_LEFT))
		{
			process_turn(-fTimeDelta * 0.8f);
			Player_KeyState(WALK);
		}
		else if (KEY_HOLD(DIK_RIGHT))
		{
			process_turn(fTimeDelta * 0.8f);
			Player_KeyState(WALK);
		}
		else
			m_iState = IDLE;
	}

	if (KEY_HOLD(DIK_X))
	{
		PLAYONLY(L"Player_PickGolem");
		VOLUME(L"Player_PickGolem", 50);

		m_iState = PICKUP;
		if (KEY_HOLD(DIK_UP) || KEY_HOLD(DIK_DOWN) || KEY_HOLD(DIK_LEFT) || KEY_HOLD(DIK_RIGHT))
		{
			PLAYONLY(L"Player_CarryingGolem");
			VOLUME(L"Player_CarryingGolem", 50);
			m_iState = CARRY;
		}
	}

	if (KEY_HOLD(DIK_Z))
	{
		m_iState = DIVE;
	}

	if (KEY_HOLD(DIK_LCONTROL))
	{
		m_iState = BURROW;
		if (KEY_HOLD(DIK_UP)) m_iState = BURROWWALK;
	}

	if (KEY_HOLD(DIK_LSHIFT))
	{
		if (!m_bSoundAtt)
		{
			Sound_RandomAtt();
			m_bSoundAtt = true;
		}

		m_iState = ATT;
	}
	else
		m_bSoundAtt = false;

	if (KEY_HOLD(DIK_SPACE))
	{
		if (!m_bSound)
		{
			Sound_RandomJump();
			m_bSound = true;
		}

		m_bJump = true;
		m_iState |= JUMP;

		if (KEY_HOLD(DIK_UP)) m_pTransformCom->Go_Straight(fTimeDelta * 0.75f, m_pNavigationCom);
		m_iState &= ~(WALK | IDLE);
		if (KEY_HOLD(DIK_LSHIFT))
		{
			PLAYONLY(L"Player_IntenseAttack");
			VOLUME(L"Player_IntenseAttack", 80);
			m_iState ^= JUMPATT;
		}
	}
	else
		m_bSound = false;
}

void CPlayer::Player_KeyState(const unsigned int iState)
{
	m_iState |= iState;

	if (m_iState & IDLE && iState != IDLE)
		m_iState ^= IDLE;

	else if (m_iState & WALK && iState != WALK)
		m_iState ^= WALK;

	else if (m_iState & TURN_L && iState != TURN_L)
		m_iState ^= TURN_L;

	else if (m_iState & TURN_R && iState != TURN_R)
		m_iState ^= TURN_R;

	else if (m_iState & ATT && iState != ATT)
		m_iState ^= ATT;

	else if (m_iState & BURROW && iState != BURROW)
		m_iState ^= BURROW;

	else if (m_iState & BURROWWALK && iState != BURROWWALK)
		m_iState ^= BURROWWALK;

	else if (m_iState & JUMP && iState != JUMP)
		m_iState ^= JUMP;

	else if (m_iState & CARRY && iState != CARRY)
		m_iState ^= CARRY;

	else if (m_iState & JUMPATT && iState != JUMPATT)
		m_iState ^= JUMPATT;
}

void CPlayer::Sound_RandomJump()
{
	int iRandNum = rand() % 3;

	switch (iRandNum)
	{
	case 0:
		PLAYONLY(L"Player_Jump");
		VOLUME(L"Player_Jump", 70);
		break;
	case 1:
		PLAYONLY(L"Player_JumpAgain");
		VOLUME(L"Player_JumpAgain", 70);
		break;
	case 2:
		PLAYONLY(L"Player_JumpAir");
		VOLUME(L"Player_JumpAir", 70);
		break;
	}
}

void CPlayer::Sound_RandomAtt()
{
	int iRandNum = rand() % 4;

	switch (iRandNum)
	{
	case 0:
		PLAYONLY(L"Player_Attack");
		VOLUME(L"Player_Attack", 80);
		break;
	case 1:
		PLAYONLY(L"Player_Attack2");
		VOLUME(L"Player_Attack2", 80);
		break;
	case 2:
		PLAYONLY(L"Player_TailSwoosh");
		VOLUME(L"Player_TailSwoosh", 80);
		break;
	case 3:
		PLAYONLY(L"Player_TailSwoosh2");
		VOLUME(L"Player_TailSwoosh2", 80);
		break;
	}
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* Body_Player */
	CBody_Player::BODY_DESC		BodyDesc{};

	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.pState = &m_iState;
	BodyDesc.eLevel = m_eLevel;

	CPartObject* pBody = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Body_Player", &BodyDesc));
	if (nullptr == pBody)
		return E_FAIL;
	m_Parts.emplace_back(pBody);

	m_pBodyPlayer = dynamic_cast<CBody_Player*>(pBody);
	m_pModelCom = dynamic_cast<CModel*>(pBody->Find_Component(L"Com_Model"));

	/* Burrow_Mode */
	CBurrow_Player::BURROW_DESC BurrowDesc{};
	BurrowDesc.pBoneMatrix = dynamic_cast<CBody_Player*>(pBody)->Get_BoneMatrixPtr("Player");
	BurrowDesc.pParentTransform = m_pTransformCom;
	BurrowDesc.fSpeedPerSec = 0.f;
	BurrowDesc.fRotationPerSec = 0.f;
	BurrowDesc.pState = &m_iState;
	BurrowDesc.eLevel = m_eLevel;

	CPartObject* pBurrow = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Burrow_Player", &BurrowDesc));
	if (nullptr == pBurrow)
		return E_FAIL;
	m_Parts.emplace_back(pBurrow);

	/* GolemHead */
	CGolemHead::GOLEM_DESC GolemDesc{};
	GolemDesc.pBoneMatrix = dynamic_cast<CBody_Player*>(pBody)->Get_BoneMatrixPtr("Bone Attachpoint01");
	GolemDesc.pParentTransform = m_pTransformCom;
	GolemDesc.fSpeedPerSec = 0.f;
	GolemDesc.fRotationPerSec = 0.f;
	GolemDesc.pState = &m_iState;
	GolemDesc.eLevel = m_eLevel;

	CPartObject* pGolem = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_GolemHead", &GolemDesc));

	if (nullptr == pGolem)
		return E_FAIL;
	m_Parts.emplace_back(pGolem);

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	if (LEVEL_GAMEPLAY == m_eLevel)
	{	/* For.Com_Navigation */
		CNavigation::NAVIGATION_DESC		NavigationDesc{};
		NavigationDesc.iCurrentCellIndex = 54; 

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}
	if (LEVEL_PUZZLE == m_eLevel)
	{	/* For.Com_Navigation */
		CNavigation::NAVIGATION_DESC		NavigationDesc{};
		NavigationDesc.iCurrentCellIndex = 0;

		if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;

	}
	if (LEVEL_BOSS == m_eLevel)
	{	/* For.Com_Navigation */
		CNavigation::NAVIGATION_DESC		NavigationDesc{};
		NavigationDesc.iCurrentCellIndex = 0;
		//NavigationDesc.iCurrentCellIndex = 198;

		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingDesc{};

	BoundingDesc.vExtents = _float3(0.6f, 1.5f, 0.6f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC TailBoundingDesc{};

	/* For.Com_TailCollider */
	TailBoundingDesc.vExtents = _float3(0.25f, 0.5f, 0.25f);
	TailBoundingDesc.vCenter = _float3(0.25f, 0.f, 0.f);
	TailBoundingDesc.vRotation = _float3(0.f, 0.f, -5.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_TailCollider"), reinterpret_cast<CComponent**>(&m_pTailColliderCom), &TailBoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTailColliderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBodyPlayer);

	for (auto& pPartObj : m_Parts)
		Safe_Release(pPartObj);
}
