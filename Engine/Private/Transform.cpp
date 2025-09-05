#include "..\Public\Transform.h"
#include "Shader.h"

#include "Navigation.h"
#include "GameInstance.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix{ rhs.m_WorldMatrix }
{

}

_float3 CTransform::Get_Scaled()
{
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	return _float3(XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_RIGHT])),
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_UP])),
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOOK])));
}

void CTransform::Set_Scale(const _float3& vScale)
{
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	//for (size_t i = 0; i < STATE_POSITION; i++)
	//	Set_State(STATE(i), XMVector3Normalize(WorldMatrix.r[i]) * ((_float*)&vScale)[i]);

	Set_State(STATE_RIGHT, XMVector3Normalize(WorldMatrix.r[STATE_RIGHT]) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(WorldMatrix.r[STATE_UP]) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(WorldMatrix.r[STATE_LOOK]) * vScale.z);

}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pTransformDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pTransformDesc->fSpeedPerSec;
	m_fRotationPerSec = pTransformDesc->fRotationPerSec;

	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_High(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Low(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vUp = Get_State(STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

/* Radian = XMConvertToRadians(Degree) */
void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(STATE_POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Move(_fvector vDirection, _float fDistance)
{
	_vector vPosition = Get_State(STATE_POSITION);
	vPosition += XMVector3Normalize(vDirection) * fDistance;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Set_BillBoard()
{
	_float4x4 float4x4ViewMatrix;
	float4x4ViewMatrix = *(m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));

	_matrix ViewMatrix = XMLoadFloat4x4(&float4x4ViewMatrix);

	_matrix InverseViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	_float3 vScale = Get_Scaled();
	_vector vRight = XMVectorSet(InverseViewMatrix.r[0].m128_f32[0], InverseViewMatrix.r[0].m128_f32[1], InverseViewMatrix.r[0].m128_f32[2], 0.0f);
	_vector vUp = XMVectorSet(InverseViewMatrix.r[1].m128_f32[0], InverseViewMatrix.r[1].m128_f32[1], InverseViewMatrix.r[1].m128_f32[2], 0.0f);
	_vector vLook = XMVectorSet(InverseViewMatrix.r[2].m128_f32[0], InverseViewMatrix.r[2].m128_f32[1], InverseViewMatrix.r[2].m128_f32[2], 0.0f);

	vRight = XMVectorScale(vRight, vScale.x);
	vLook = XMVectorScale(vLook, vScale.z);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_LOOK, vLook);
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CTransform::Clone(void* pArg)
{
	return nullptr;
}

void CTransform::Free()
{
	__super::Free();
}
