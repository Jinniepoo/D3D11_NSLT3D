#pragma once

#include "Component.h"

/* XMFLOAT2, XMFLOAT3, XMFLOAT4 == XMVECTOR */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct
	{
		_float		fSpeedPerSec = {};
		_float		fRotationPerSec = {};
	}TRANSFORM_DESC;

public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	_float3 Get_Scaled();

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	const _float4x4* Get_WorldFloat4x4() const
	{
		return &m_WorldMatrix;
	}

	XMFLOAT4X4 Get_WorldMatrixInverse() {
		XMMATRIX WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
		XMMATRIX WorldMatrixInverse = XMMatrixInverse(nullptr, WorldMatrix);
		XMStoreFloat4x4(&m_WorldMatrixInverse, WorldMatrixInverse);
		return m_WorldMatrixInverse;
	}

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], vState);
	}
	void Set_Scale(const _float3& vScale);

	void Set_WorldMat(_float4x4 vWorldMat){
		m_WorldMatrix = vWorldMat;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override; 

public:
	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Left(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr);;
	void Go_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_High(_float fTimeDelta);
	void Go_Low(_float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void LookAt(_fvector vAt);
	void Move(_fvector vDirection, _float fDistance);
	void Set_BillBoard();

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

private:
	_float4x4			m_WorldMatrix = {};
	_float4x4			m_WorldMatrixInverse = {};
	_float				m_fSpeedPerSec = { 0.f };
	_float				m_fRotationPerSec = { 0.f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END