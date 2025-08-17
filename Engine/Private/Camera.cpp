#include "Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCamera::CCamera(const CCamera& rhs, CTransform::TRANSFORM_DESC* pArg)
	: CGameObject( rhs )
{
}

HRESULT CCamera::Initialize_Prototype()
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	/*if (nullptr == pArg)
	{
		MSG_BOX(L"Need CAMERA_DESC!");
		return E_FAIL;
	}*/

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	m_fMouseSensor = pDesc->fMouseSensor;
	m_fFovy = pDesc->fFovy;
	m_fAspect = pDesc->fAspect;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

 	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

	//m_Components.emplace("Com_Transform", m_pTransformCom);

	return S_OK;
}

void CCamera::PriorityTick(_float fTimeDelta)
{
	/* 카메라 월드행렬의 역행렬을 뷰행렬로서 파이파이라인에 저장해주낟. */
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix()));
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Tick(_float fTimeDelta)
{

}

void CCamera::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

float GetRandomFloat(float min, float max) {
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void CCamera::CameraShaking(_float fIntensity, _float fDuration, _float fTimeDelta)
{
	_float3 vPos = {};
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_float3 vShake = { GetRandomFloat(-1.f, 1.f), GetRandomFloat(-1.f, 1.f), GetRandomFloat(-1.f, 1.f) };

	float fShaking = sinf(fTimeDelta * 2 * XM_PI / fDuration);

	vPos.x += fShaking + fIntensity * vShake.x;
	vPos.y += fShaking + fIntensity * vShake.y;
	vPos.z += fShaking + fIntensity * vShake.z;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
}

void CCamera::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGameInstance);
}
