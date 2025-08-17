#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

_uint		g_iSizeX = 8192;
_uint		g_iSizeY = 4608;

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pLightDepthDSV);
	Safe_AddRef(m_pDevice); Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	/* 디퍼드 쉐이더를 위한 렌더타겟들을 추가하겠다. */

	/* 원래는 백 버퍼에 그려졌어야할 내용 그대로 다른 렌더타겟에 그리는 작업을 하므로 */
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* For.Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Diffuse", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Normal", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Depth", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_PickedDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_PickedDepth", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_LightDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_LightDepth", g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Shade", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Blur */
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Blur_X", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_FinalBlur */
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Blur_Y", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Result */
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Result", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.MRT_GameObjects ( Diffuse + Normal ) */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_PickedDepth"))))
		return E_FAIL;

	/* For.MRT_LightAcc ( Shade )*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;

	/* For.MRT_ShadowGameObject ( LightDepth )*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowGameObject"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	/* For.MRT_Blur */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;

	/* For.MRT_FinalBlur */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
		return E_FAIL;

	/* For.MRT_Result */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Result"), TEXT("Target_Result"))))
		return E_FAIL;
	/* 화면을 꽉 채워서 직교투영해주기위한 월드행렬 */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL /*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RTDebug(L"Target_Diffuse", 30.f, 200.f, 60.f, 60.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(L"Target_Normal", 30.f, 260.f, 60.f, 60.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(L"Target_Depth", 30.f, 320.f, 60.f, 60.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(L"Target_LightDepth", 30.f, 380.f, 60.f, 60.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(L"Target_Shade", 30.f, 440.f, 60.f, 60.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(L"Target_Blur_X", 30.f, 500.f, 60.f, 60.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(L"Target_Blur_Y", 30.f, 560.f, 60.f, 60.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(L"Target_Result", 30.f, 700.f, 60.f, 60.f)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderObjects(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_Shadow()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	
	//if (FAILED(Render_NonBlend_B()))
	//	return E_FAIL;

	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Blur()))
		return E_FAIL;

	if (FAILED(Render_FinalBlur()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;


#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

void CRenderer::Clear()
{
	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}
}

HRESULT CRenderer::Render_Priority()
{
	m_pGameInstance->Begin_MRT(L"MRT_Result");

	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();
	m_pGameInstance->End_MRT();

	if (KEY_PUSH(DIK_APOSTROPHE))
	{
		if (m_ShowDebug)
			m_ShowDebug = false;

		else if (!m_ShowDebug)
			m_ShowDebug = true;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	m_pGameInstance->Begin_MRT(L"MRT_ShadowGameObject", m_pLightDepthDSV);

	D3D11_VIEWPORT	ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof D3D11_VIEWPORT);

	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iSizeX;
	ViewPortDesc.Width = (_float)g_iSizeX;
	ViewPortDesc.Height = (_float)g_iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOW])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_LightDepth();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_SHADOW].clear();

	m_pGameInstance->End_MRT();

	m_pGameInstance->Clear_DepthStencil_View();

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = 1280.0f;
	ViewPortDesc.Height = 720.0f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal + Depth + Hit 타겟을 바인딩하낟. */
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_GameObjects")))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONBLEND].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend_B()
{
	/* Diffuse + Normal + Depth + Hit 타겟을 바인딩하낟. */
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_GameObjects")))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND_B])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONBLEND_B].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* Shade */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	/* 빛 갯수만큼 사각형을 그리낟. */
	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	m_pGameInstance->Begin_MRT(L"MRT_Result", nullptr, false);

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Diffuse", m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Shade", m_pShader, "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Depth", m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_LightDepth", m_pShader, "g_LightDepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	/* 광원기준으로 표현된 뷰, 투영행렬이다. (카메라기준x) */
	_float4x4		LightViewMatrix, LightProjMatrix;

	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(XMVectorSet(30.f, 50.f, 30.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&LightProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), (_float)1280.0f / 720.0f, 0.1f, 3000.f));

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &LightViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
		return E_FAIL;

	m_pShader->Begin(3);

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	m_pVIBuffer->Render();

	m_pGameInstance->End_MRT();

	return S_OK;
}

HRESULT CRenderer::Render_Blur()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Result"), m_pShader, "g_EffectTexture")))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();

	m_pShader->Begin(5);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Blur_X"), m_pShader, "g_EffectTexture")))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();

	m_pShader->Begin(6);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_FinalBlur()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Blur_Y", m_pShader, "g_BlurTexture")))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Effect"), m_pShader, "g_EffectTexture")))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Result", m_pShader, "g_ResultTexture")))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();

	m_pShader->Begin(7);

	m_pVIBuffer->Render();


	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		if (m_ShowDebug)
			pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (m_ShowDebug)
	{
		if (FAILED(m_pGameInstance->Render_RTDebug(L"MRT_GameObjects", m_pShader, m_pVIBuffer)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Render_RTDebug(L"MRT_LightAcc", m_pShader, m_pVIBuffer)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Render_RTDebug(L"MRT_ShadowGameObject", m_pShader, m_pVIBuffer)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Render_RTDebug(L"MRT_Result", m_pShader, m_pVIBuffer)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Render_RTDebug(L"MRT_Blur_X", m_pShader, m_pVIBuffer)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Render_RTDebug(L"MRT_Blur_Y", m_pShader, m_pVIBuffer)))
			return E_FAIL;
	}

	return S_OK;
}
#endif
CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

#ifdef _DEBUG
	for (auto& pDebugCom : m_DebugComponents)
		Safe_Release(pDebugCom);
	m_DebugComponents.clear();
#endif

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}

	Safe_Release(m_pLightDepthDSV);
	Safe_Release(m_pDevice); Safe_Release(m_pContext);
}
