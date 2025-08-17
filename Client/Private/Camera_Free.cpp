#include "stdafx.h"
#include "..\Public\Camera_Free.h"

#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::PriorityTick(_float fTimeDelta)
{
	if (false == m_bDebugCam)
		return;

	Camera_KeyInput(fTimeDelta);
	Camera_MouseInput(fTimeDelta);

	__super::PriorityTick(fTimeDelta);
}

void CCamera_Free::Tick(_float fTimeDelta)
{
	if (false == m_bDebugCam)
		return;
}

void CCamera_Free::LateTick(_float fTimeDelta)
{
	if (false == m_bDebugCam)
		return;
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::Camera_KeyInput(_float fTimeDelta)
{
	if (KEY_HOLD(DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta);

	if (KEY_HOLD(DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta);

	if (KEY_HOLD(DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta);

	if (KEY_HOLD(DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta);
}

void CCamera_Free::Camera_MouseInput(_float fTimeDelta)
{
	if (true == m_bMouseCam)
	{
		_long lMouseMovement = {};
		if (MOUSE_MOVE_X(lMouseMovement))
 			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * lMouseMovement * m_fMouseSensor);

		if (MOUSE_MOVE_Y(lMouseMovement))
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * lMouseMovement * m_fMouseSensor);
	}
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCamera_Free::Free()
{
	__super::Free();

}
