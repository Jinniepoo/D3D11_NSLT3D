#pragma once

#include "Base.h"

/* 화면에 그려져야할 객체들을 그리는 순서대로 모아서 저장한다. */
/* 저장하고 있는 객체들의 Render함수를 호출한다. */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	/* 그리는 순서대로 열거체를 정의했다. */
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOW, RENDER_NONBLEND, RENDER_NONBLEND_B, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderObjects(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw();
	void	Clear();

#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pComponent)
	{
		m_DebugComponents.emplace_back(pComponent);
		Safe_AddRef(pComponent);
		return S_OK;
	}
#endif

private:
	ID3D11Device*			 m_pDevice = { nullptr };
	ID3D11DeviceContext*	 m_pContext = { nullptr };
	list<class CGameObject*> m_RenderObjects[RENDER_END];

	class CGameInstance*	m_pGameInstance = { nullptr };
	class CVIBuffer_Rect*	m_pVIBuffer = { nullptr };

	/* 렌더타겟 디버깅용 패스 (0), 빛연산을 위한 패스 (1) */
	class CShader*			m_pShader = { nullptr };

	ID3D11DepthStencilView* m_pLightDepthDSV = { nullptr };

private:
	/* m_WorldMatrix : 화면을 꽉 채우는 직교투영이 빈번히 활용되기때문에 저장해둘거야 .*/
	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

#ifdef _DEBUG
private:
	list<class CComponent*>     m_DebugComponents;

#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_NonBlend_B();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

private:
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();
	HRESULT	Render_Blur();
	HRESULT Render_FinalBlur();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif

private:
	_bool	m_ShowDebug = {false};

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END