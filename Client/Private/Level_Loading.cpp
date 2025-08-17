#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"
#include "Loading_BG.h"
#include "Level_Main.h"
#include "Level_GamePlay.h"
#include "Level_Boss.h"
#include "Level_Puzzle.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext } 
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_iLevelID = LEVEL_LOADING;
	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	Safe_AddRef(m_pLoader);

	if (nullptr == m_pLoader)
		return E_FAIL;	

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_NSLT(TEXT("Layer_NSLT"))))
		return E_FAIL;


	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			CLevel*		pLevel = { nullptr };

			switch (m_eNextLevelID)
			{
			case LEVEL_MAIN:
				pLevel = CLevel_Main::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_GAMEPLAY:
				pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_PUZZLE:
				pLevel = CLevel_Puzzle::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_BOSS:
				pLevel = CLevel_Boss::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pLevel)
				return;

			if (FAILED(m_pGameInstance->Change_Level(pLevel)))
				return;
		}		
	}	
}

HRESULT CLevel_Loading::Render()
{	
	m_pLoader->Show_LoadingText();
	
	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	CLoading_BG::BG_DESC BackDesc;

	BackDesc.iSizeX = g_iWinSizeX;
	BackDesc.iSizeY = g_iWinSizeY;
	BackDesc.fX = g_iWinSizeX >> 1;
	BackDesc.fY = g_iWinSizeY >> 1;
	BackDesc.eObjType = CLoading_BG::LOADING_BG;

	/* Loading - BG */
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LOADING, strLayerTag, TEXT("Prototype_GameObject_Loading_BG"), &BackDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_NSLT(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LOADING, strLayerTag, TEXT("Prototype_GameObject_Loading_Gear"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LOADING, strLayerTag, TEXT("Prototype_GameObject_Loading_Logo"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
