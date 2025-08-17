#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual _bool Picking(class CTransform* pTransform, _float3* pOut);
	bool CheckTriangleIntersection(const _vector& vRayPos, const _vector& vRayDir, const _matrix& WorldMatrix, _uint iIndices[4], _float3* pOut);
	void StorePickedPosition(const _vector& vRayPos, const _vector& vRayDir, float fDist, const _matrix& WorldMatrix, _float3* pOut);
	_vector LoadFloat3WithW(const _float3& float3);
public:
	virtual HRESULT Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Culling(_fmatrix matWorldMatrix);

private:
	_uint				m_iNumVerticesX = {};
	_uint				m_iNumVerticesZ = {};

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iNumVerticesX, _uint iNumVerticesZ);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END