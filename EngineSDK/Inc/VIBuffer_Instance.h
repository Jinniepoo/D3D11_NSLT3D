#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	typedef struct
	{
		_float2		vScale{};
		_float2		vSpeed{};
		_float2		vLifeTime{}; 
		_float3		vPivot{}; 
		_float3		vCenter{}; 
		_float3		vRange{};
		_bool		isLoop{}; 
		_uint		iNumInstance{};
	}INSTANCE_DESC;

protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;

public:
	virtual void Update_Spread(_float fTimeDelta);
	virtual void Update_Drop(_float fTimeDelta);

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	_uint						m_iNumInstance = {};
	_uint						m_iInstanceVertexStride = {};
	_uint						m_iNumIndexPerInstance = {};
	_float* m_pSpeeds = { nullptr };
	_float3						m_vPivot = {};
	_bool						m_isLoop = { false };
	_float4* m_pInstanceVertexPos = { nullptr };

protected:
	/* 0.0f ~ 1.f */
	_float Compute_RandomNumber() {
		return rand() / (_float)RAND_MAX;
	}

	_float Compute_RandomNumber(_float fMin, _float fMax) {
		return fMin + Compute_RandomNumber() * (fMax - fMin);
	}


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
