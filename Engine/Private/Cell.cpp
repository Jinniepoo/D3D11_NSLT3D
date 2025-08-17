#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_float CCell::Compute_Height(_float3& vPlayerPos)
{
	_vector vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[0]), XMLoadFloat3(&m_vPoints[1]), XMLoadFloat3(&m_vPoints[2]));

	_float fY = (XMVectorGetX(vPlane) * vPlayerPos.x  * -1.f) + (XMVectorGetZ(vPlane) * vPlayerPos.z * -1.f) + (XMVectorGetW(vPlane) * -1.f);

	return fY / XMVectorGetY(vPlane);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::isIn(_fvector vLocalPos, _int* pNeighborIndex)
{
	_vector		vDir, vNormal;

	for (size_t i = 0; i < LINE_END; i++)
	{
		vDir = vLocalPos - XMLoadFloat3(&m_vPoints[i]);
		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % LINE_END]) - XMLoadFloat3(&m_vPoints[i]);
		vNormal = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);

		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(vNormal))))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}

	}

	return true;
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDest))
			return true;
	}

	return false;
}

#ifdef _DEBUG

HRESULT CCell::Render()
{
	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
