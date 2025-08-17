#include "VIBuffer_Terrain.h"
#include "Picking.h"
#include "GameInstance.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer( rhs )
	, m_iNumVerticesX (rhs.m_iNumVerticesX)
	, m_iNumVerticesZ (rhs.m_iNumVerticesZ)
{
}

_bool CVIBuffer_Terrain::Picking(CTransform* pTransform, _float3* pOut)
{
	CPicking* pPicking = m_pGameInstance->Get_Picking();

	_matrix WorldMatrix = pTransform->Get_WorldMatrix();
	_matrix WorldMatrixInverse = XMLoadFloat4x4(&pTransform->Get_WorldMatrixInverse());
	_vector vRayDir, vRayPos;

	pPicking->Compute_LocalRayInfo(&vRayDir, &vRayPos, WorldMatrix);

	// 방향 -> Normalize
	vRayDir = XMVector3Normalize(vRayDir);
	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[] = {
			   iIndex + m_iNumVerticesX,
			   iIndex + m_iNumVerticesX + 1,
			   iIndex + 1,
			   iIndex
			};

			if (CheckTriangleIntersection(vRayPos, vRayDir, WorldMatrix, iIndices, pOut))
			{
				return true;
			}
		}
	}

	return false;
}

bool CVIBuffer_Terrain::CheckTriangleIntersection(const _vector& vRayPos, const _vector& vRayDir, const _matrix& WorldMatrix, _uint iIndices[4], _float3* pOut)
{
	_vector v0 = LoadFloat3WithW(m_pVertexPositions[iIndices[0]]);
	_vector v1 = LoadFloat3WithW(m_pVertexPositions[iIndices[1]]);
	_vector v2 = LoadFloat3WithW(m_pVertexPositions[iIndices[2]]);

	float fDist;
	if (TriangleTests::Intersects(vRayPos, vRayDir, v0, v1, v2, fDist))
	{
		StorePickedPosition(vRayPos, vRayDir, fDist, WorldMatrix, pOut);
		return true;
	}

	v1 = LoadFloat3WithW(m_pVertexPositions[iIndices[2]]);
	v2 = LoadFloat3WithW(m_pVertexPositions[iIndices[3]]);

	if (TriangleTests::Intersects(vRayPos, vRayDir, v0, v1, v2, fDist))
	{
		StorePickedPosition(vRayPos, vRayDir, fDist, WorldMatrix, pOut);
		return true;
	}

	return false;
}

void CVIBuffer_Terrain::StorePickedPosition(const _vector& vRayPos, const _vector& vRayDir, float fDist, const _matrix& WorldMatrix, _float3* pOut)
{
	_vector vPickPos = XMVectorAdd(vRayPos, XMVectorScale(vRayDir, fDist));
	vPickPos = XMVector3TransformCoord(vPickPos, WorldMatrix);
	XMStoreFloat3(pOut, vPickPos);
}

_vector CVIBuffer_Terrain::LoadFloat3WithW(const _float3& float3)
{
	return XMVectorSet(float3.x, float3.y, float3.z, 1.0f);
}
	
HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iVertexStride = sizeof(VTXNORTEX);
	m_iIndexStride = 4;	
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	
	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVertexPositions[iIndex] = _float3((_float)j, 0.f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), (_float)i / (m_iNumVerticesZ - 1.f));
		}
	}

#pragma endregion


#pragma region INDEX_BUFFER	
	
	_uint*		pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);	

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = _uint(i * m_iNumVerticesX + j);

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX, 
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1, 
				iIndex
			};		

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
		}
	}


#pragma endregion

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);


	return S_OK;
}


HRESULT CVIBuffer_Terrain::Initialize(void * pArg)
{
	return S_OK;
}

void CVIBuffer_Terrain::Culling(_fmatrix matWorldMatrix)
{
	m_pGameInstance->Transform_Frustum_To_LocalSpace(matWorldMatrix);

	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	_uint iNumIndices = {};

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX -1; j++)
		{
			_uint iIndex = _uint(i * m_iNumVerticesX + j);

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_bool bIsIn[4] = {
				m_pGameInstance->IsIn_Frustum_LocalSpace(XMLoadFloat3(&m_pVertexPositions[iIndices[0]]), 0.f),
				m_pGameInstance->IsIn_Frustum_LocalSpace(XMLoadFloat3(&m_pVertexPositions[iIndices[1]]), 0.f),
				m_pGameInstance->IsIn_Frustum_LocalSpace(XMLoadFloat3(&m_pVertexPositions[iIndices[2]]), 0.f),
				m_pGameInstance->IsIn_Frustum_LocalSpace(XMLoadFloat3(&m_pVertexPositions[iIndices[3]]), 0.f),
			};

			if (true == bIsIn[0] &&
				true == bIsIn[1] &&
				true == bIsIn[2])
			{
				((_uint*)SubResource.pData)[m_iNumIndices++] = iIndices[0];
				((_uint*)SubResource.pData)[m_iNumIndices++] = iIndices[1];
				((_uint*)SubResource.pData)[m_iNumIndices++] = iIndices[2];
			}

			if (true == bIsIn[0] &&
				true == bIsIn[2] &&
				true == bIsIn[3])
			{
				((_uint*)SubResource.pData)[m_iNumIndices++] = iIndices[0];
				((_uint*)SubResource.pData)[m_iNumIndices++] = iIndices[2];
				((_uint*)SubResource.pData)[m_iNumIndices++] = iIndices[3];
			}
		}
	}
	m_pContext->Unmap(m_pIB, 0);
	m_iNumIndices = iNumIndices;
}


CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumVerticesX, _uint iNumVerticesZ)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumVerticesX, iNumVerticesZ)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
