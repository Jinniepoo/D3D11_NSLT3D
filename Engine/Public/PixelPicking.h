#pragma once

#include "Base.h"

BEGIN(Engine)

class CPixelPicking final : public CBase
{
private:
	CPixelPicking(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPixelPicking() = default;

public:
	_float4 Get_PickedPos() const {
		return m_vPickPos;
	}

	_bool	Pixel_IsPicked() const {
		return m_bPixelPicked;
	}

public:
	HRESULT Initialize(HWND hWnd);
	void Update();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	ID3D11Texture2D*		m_pPickedDepthTexture = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	HWND					m_hWnd{};

	_float					m_fViewWidth{}, m_fViewHeight{};
	_float4					m_vPickPos{};
	_bool					m_bPixelPicked = { false };


public:
	static CPixelPicking* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
	virtual void Free() override;
};

END
