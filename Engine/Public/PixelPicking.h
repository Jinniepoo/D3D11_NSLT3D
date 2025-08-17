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
	/* 렌터타겟에 기록된 피킹용 깊이를 복제받아오기위한 텍스쳐를 만들자. */
	/* why? : 렌더타겟용 텍스쳐를 동적으로 생성할 수 없기 때문에. 동적인 텍스쳐를 직접생성하여 렌더타겟을 복제받아와야 텍스쳐의 픽셀을 클라에서 꺼내올 수 있다. */
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