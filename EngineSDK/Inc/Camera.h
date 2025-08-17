#pragma once

#include "GameObject.h"
#include "Level.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	using CAMERA_DESC = struct tagCameraDesc : public CTransform::TRANSFORM_DESC
	{
		_float			fMouseSensor = {};
		_float4			vEye = {};
		_float4			vAt = {};
		_float			fFovy = {};
		_float			fAspect = {};
		_float			fNear = {};
		_float			fFar = {};
		_uint			eLevel = { 7 };
	};

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs, CTransform::TRANSFORM_DESC* pArg);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	CAMERA_DESC       m_CameraDesc;

public:
	void SetShaking(_bool bShake, _float fIntensity)
	{
		m_fIntensity = fIntensity;
		m_bIsShaking = bShake;
	}

protected:
	_float					m_fMouseSensor = {};
	_float					m_fFovy = {};
	_float					m_fAspect = {};
	_float					m_fNear = {};
	_float					m_fFar = {};

	void CameraShaking(_float fIntensity, _float fDuration, _float fTimeDelta);
	_float Get_RandomFloat(_float fMin, _float fMax)
	{
		return fMin + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (fMax - fMin));
	}
	_float			  m_fIntensity = { 0.f };
	_bool			  m_bIsShaking = { false };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
