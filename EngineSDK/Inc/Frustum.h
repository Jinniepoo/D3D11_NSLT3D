#pragma once

#include "Base.h"

/* 절두체를 구성하기위한 초기 투영스페이스 상의 여덟개점을 정의하고 보관하낟.*/
/* Updata함수 안에서 점 여덟개를 월드까지 변환하고 평며을 만들어서 저장한다. */
/* 비교판단하기위한 객체의 월드위치를 받아와서 내가 가지고 잇는 여섯개평면(절두체)안에 있나없나? */

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