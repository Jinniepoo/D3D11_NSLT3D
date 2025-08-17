
#include "stdafx.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Main.h"

#include "GameObject.h"
#include "ImGui_Manager.h"
#include "Camera_Free.h"

#include "Loading_BG.h"
#include "Loading_Gear.h"
#include "Loading_Logo.h"

#include "UI_Lives.h"
#include "UI_Clover_Check.h"
#include "UI_Clover_Question.h"
#include "UI_Clover_Paw.h"
#include "UI_Clover_L.h"
#include "UI_Coins.h"
#include "UI_Heart_Outline.h"
#include "UI_Heart_Filling.h"
#include "UI_Lucky.h"

#include "CoinEffect.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_pDevice{ nullptr }
	, m_pContext{ nullptr }
	, m_pTool{ nullptr }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pTool);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;


	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObjects()))
		return E_FAIL;

	m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/Bangers.spritefont"));

	if (FAILED(Ready_Prototoype_Component()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_MAIN)))
		return E_FAIL;

	m_eLevel = GET_CURLEVEL;

	m_pTool = CImGui_Manager::Get_Instance();

	if (nullptr == m_pTool)
		return E_FAIL;

	if (FAILED(m_pTool->Initialize(m_pDevice, m_pContext, g_hWnd)))
		return E_FAIL;


	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_eLevel = GET_CURLEVEL;
	m_pGameInstance->Tick_Engine(fTimeDelta);

	m_pTool->Tick();

	if (KEY_PUSH(DIK_APOSTROPHE))
	{
		if (m_bDebug)
			m_bDebug = false;
		else if (!m_bDebug)
			m_bDebug = true;
	}

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif

}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.0f, 0.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();

	++m_iNumRender;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS: %d", m_iNumRender);
		m_fTimeAcc = 0.f;
		m_iNumRender = 0;
	}

	if (m_bDebug)
		m_pGameInstance->Render_Font(L"Font_Default", m_szFPS, _float2(1170.f, 680.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));

	m_pTool->Render();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MAIN))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObjects()
{
	/* -- LOADING -- */

	/* Prototype_GameObject_Loading_BG */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_BG"), CLoading_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Loading_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_Logo"), CLoading_Logo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Loading_Gear */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_Gear"), CNSLT_Gear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Debug_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"), CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Effects */

	/* For.Prototype_GameObject_Effect_CoinEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_CoinEffect"),
		CCoinEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* UI */

	/* Prototype_GameObject_UI_Lives*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Lives"),
		CUI_Lives::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Clover_Check */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Clover_Check"),
		CUI_Clover_Check::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Clover_Question */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Clover_Question"),
		CUI_Clover_Question::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Clover_Paw */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Clover_Paw"),
		CUI_Clover_Paw::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Clover_L */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Clover_L"),
		CUI_Clover_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Coins */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Coins"),
		CUI_Coins::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Heart_Outline */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Heart_Outline"),
		CUI_Heart_Outline::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Heart_Filling */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Heart_Filling"),
		CUI_Heart_Filling::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Heart_Lucky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Lucky"),
		CUI_Lucky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	

	/* Camera */
	CCamera::CAMERA_DESC CameraDesc;

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(65.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC2, L"Layer_CameraFree", L"Prototype_GameObject_Camera_Free", &CameraDesc)))
	{
		MSG_BOX(L"CLevel_GamePlay - Ready_Layer_Camera_Free Failed");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Ready_Prototoype_Component()
{
	/* For. Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Transform", CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* -- LOADING -- */

	/* Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_Main/TitleScreen_BG%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_NSLT_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NSLT_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_Main/Loading_Gear_NSLT_Logo%d.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_NSLT_Gear */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NSLT_Gear"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_Main/Loading_Gear%d.dds"), 1))))
		return E_FAIL;

	/* Effects */

	/* For.Prototype_Component_Effect_CoinLoot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Effect_CoinLoot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect_CoinLoot/CoinRing%d.dds"), 28))))
		return E_FAIL;

	/* UI */

	/* For.Prototype_Component_UI_Lives */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Lives"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_GamePlay/UI_LuckyLives%d.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Clover_Check */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_UI_Clover_Check"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_GamePlay/UI_CloverCheck%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Clover_Question */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_UI_Clover_Question"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_GamePlay/UI_CloverQuestion%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Clover_Paw */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_UI_Clover_Paw"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_GamePlay/UI_CloverFoot%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Clover_L */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_UI_Clover_L"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_GamePlay/UI_CloverL%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Coins */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_UI_Coins"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_GamePlay/UI_Coins%d.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Heart_Outline */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_UI_Heart_Outline"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_GamePlay/UI_Heart_Outline%d.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Heart_Filling */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_UI_Heart_Filling"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_GamePlay/UI_Heart%d.dds"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Lucky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Lucky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_GamePlay/UI_Lucky_Outline%d.dds"), 6))))
		return E_FAIL;


	/* Collider */

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pTool);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}

