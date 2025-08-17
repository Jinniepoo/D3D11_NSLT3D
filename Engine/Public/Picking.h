#pragma once
#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPicking(const CPicking& rhs);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void	Tick();
	void	Compute_LocalRayInfo(_vector* pRayDir, _vector* pRayPos, const _matrix& WorldMatrix);

private:
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CGameInstance*		 m_pGameInstance = { nullptr };
	HWND			     m_hWnd;
	_uint			     m_iWinSizeX, m_iWinSizeY;
	_vector			     m_vRayDir;
	_vector			     m_vRayPos;

	_float4x4			 m_ViewMatrix = {};
	_float4x4			 m_ProjMatrix = {};

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;

};

END