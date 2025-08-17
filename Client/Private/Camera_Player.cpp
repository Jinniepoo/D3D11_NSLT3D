#include "stdafx.h"
#include "Camera_Player.h"

#include "GameInstance.h"
#include "Level.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& rhs)
	: CCamera( rhs )
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Player::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_eLevel = (LEVEL)pDesc->eLevel;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
 	list<Engine::CGameObject*> pPlayer = *(GET_LIST(m_eLevel, L"Layer_Player"));

	if (0 >= pPlayer.size())
	{
		MSG_BOX(L"PlayerCamera - Failed to Find Layer");
		return E_FAIL;
	}
	m_pPlayer = (CPlayer*)pPlayer.front();
	Safe_AddRef(m_pPlayer);

	SetOriginPos(_float3(0.f, 5.f, -12.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vCamPosOrigin), 1.f)); 
	
	return S_OK;
}

void CCamera_Player::PriorityTick(_float fTimeDelta) 
{
	if (false == m_bPlayerCam)
		return; 

	XMStoreFloat3(&m_vCamPosPrev, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CCamera_Player::Tick(_float fTimeDelta)
{
	if (false == m_bPlayerCam)
		return;

 	XMStoreFloat3(&m_vPlayerPosition, m_pPlayer->Get_Player_Position()); 
 	SetOriginPos(_float3(0.f, 5.f, -12.f));

	_vector vLookAt = XMLoadFloat3(&m_vPlayerPosition) + XMVectorSet(0.f, 5.f, 0.f, 1.f);
	m_pTransformCom->LookAt(vLookAt);
	_vector vLerp = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_vCamPosPrev), 1.f), XMVectorSetW(XMLoadFloat3(&m_vCamPosOrigin), 1.f), fTimeDelta * m_fSpeedPerSec);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerp);

	__super::PriorityTick(fTimeDelta);
}

void CCamera_Player::LateTick(_float fTimeDelta)
{
	if (false == m_bPlayerCam)
		return;
}

HRESULT CCamera_Player::Render()
{
	return S_OK;
}

void CCamera_Player::SetOriginPos(const _float3& vRelativeDistFromPlayer)
{
	_matrix matPlayerWorldMat = m_pPlayer->Get_Player_Transform()->Get_WorldMatrix();

	_vector vRelativePos = XMLoadFloat3(&vRelativeDistFromPlayer);
	_vector vWorldPos = XMVector3TransformCoord(vRelativePos, matPlayerWorldMat);

	XMStoreFloat3(&m_vCamPosOrigin, vWorldPos);
}

CCamera_Player* CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Player* pInstance = new CCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Player::Clone(void* pArg)
{
	CCamera_Player* pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCamera_Player::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
