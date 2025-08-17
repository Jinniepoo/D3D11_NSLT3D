#include "stdafx.h"
#include "..\Public\Level_Main.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Loading_BG.h"

CLevel_Main::CLevel_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext } 
{

}

HRESULT CLevel_Main::Initialize()
{
	m_iLevelID = LEVEL_MAIN;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_NSLT(TEXT("Layer_NSLT_Logo"))))
		return E_FAIL;

	/* Sound */
	PLAYTOBGM(L"BGM_Main", true);
	VOLUME(L"BGM_Main", 50);

	return S_OK;
}

void CLevel_Main::Tick(_float fTimeDelta)
{
	if (KEY_PUSH(DIK_SPACE))
	{
 		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;
	}
}

HRESULT CLevel_Main::Render()
{
	SetWindowText(g_hWnd, TEXT("New Super Lucky's Tale: Main"));

	return S_OK;
}

HRESULT CLevel_Main::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	CLoading_BG::BG_DESC BackDesc;

	BackDesc.iSizeX = g_iWinSizeX;
	BackDesc.iSizeY = g_iWinSizeY;
	BackDesc.fX = g_iWinSizeX >> 1;
	BackDesc.fY = g_iWinSizeY >> 1;
	BackDesc.eObjType = CLoading_BG::MAIN_BG;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_MAIN, strLayerTag, TEXT("Prototype_GameObject_BackGround"), &BackDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Main::Ready_Layer_NSLT(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_MAIN, strLayerTag, TEXT("Prototype_GameObject_Main_NSLT"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Main * CLevel_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Main*	pInstance = new CLevel_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Main"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Main::Free()
{
	STOP(L"BGM_Main");

	__super::Free();
}
