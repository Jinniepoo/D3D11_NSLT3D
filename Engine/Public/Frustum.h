#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();

	void Transform_ToLocalSpace(_fmatrix WorldMatrix);

	_bool isIn_WorldSpace(_fvector vWorldPos, _float fRange);
	_bool isIn_LocalSpace(_fvector vLocalPos, _float fRange);

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

private:
	_float3				m_vPoints[8] = {};
	_float3				m_vWorldPoints[8] = {};
	_float4				m_vWorldPlanes[6] = {};
	_float4				m_vLocalPlanes[6] = {};

private:
	void Make_Planes(const _float3* pPoints, _float4* pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

END
