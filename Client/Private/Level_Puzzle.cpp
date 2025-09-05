#include "stdafx.h"
#include "..\Public\Level_Puzzle.h"
#include "GameInstance.h"

#include "Puzzle_Defines.h"

CLevel_Puzzle::CLevel_Puzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_hModelFile{}
{
}

HRESULT CLevel_Puzzle::Initialize()
{
	m_iLevelID = LEVEL_PUZZLE;

	m_pTool = CImGui_Manager::Get_Instance();
	Safe_AddRef(m_pTool);

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Models(L"Layer_Models")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Collectibles(L"Layer_Collectibles")))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_CameraPlayer"))))
		return E_FAIL;

	list<CGameObject*>* pPlayer = GET_LIST(LEVEL_PUZZLE, L"Layer_Player");

	if (0 >= pPlayer->size())
	{
		MSG_BOX(L"PlayerCamera - Failed to Find Layer");
		return E_FAIL;
	}
	m_pPlayer = (CPlayer*)pPlayer->front();
	Safe_AddRef(m_pPlayer);

	m_pCoin = dynamic_cast<CUI_Coins*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_Coins")->front());
	Safe_AddRef(m_pCoin);

	m_pLives = dynamic_cast<CUI_Lives*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_Lives")->front());
	Safe_AddRef(m_pLives);

	m_pQuestion = dynamic_cast<CUI_Clover_Question*>(GET_LIST(LEVEL_STATIC, L"Layer_Clover_Question")->front());
	Safe_AddRef(m_pLives);

	PLAYONLY(L"BGM_Puzzle");
	VOLUME(L"BGM_Puzzle", 40);

	return S_OK;
}

void CLevel_Puzzle::Tick(_float fTimeDelta)
{
	if (KEY_PUSH(DIK_APOSTROPHE))
	{
		if (m_bDebug)
			m_bDebug = false;

		else if (!m_bDebug)
			m_bDebug = true;
	}

	const float fElementDuration = 1.0f; /

	m_fElapsedTime += fTimeDelta;

	if (KEY_HOLD(DIK_Y))
	{
		auto AddUIElement = [this](int textureIdx, const std::wstring& layer, const std::wstring& prototype) {
			CUI_LuckyComplete::UI_DESC UIDesc;
			UIDesc.iTextureIdx = textureIdx;
			m_pGameInstance->Add_Clone(LEVEL_PUZZLE, layer.c_str(), prototype.c_str(), &UIDesc);
			};

		if (m_fElapsedTime >= 0.0f && m_fElapsedTime < fElementDuration)
		{
			AddUIElement(0, L"Layer_LuckyComplete", L"Prototype_GameObject_LuckyComplete"); // 1 Blue Board
		}
		else if (m_fElapsedTime >= fElementDuration && m_fElapsedTime < 2 * fElementDuration)
		{
			AddUIElement(1, L"Layer_LuckyComplete", L"Prototype_GameObject_LuckyComplete"); // 2 Board Top
		}
		else if (m_fElapsedTime >= 2 * fElementDuration && m_fElapsedTime < 3 * fElementDuration)
		{
			AddUIElement(2, L"Layer_LuckyComplete", L"Prototype_GameObject_LuckyComplete"); // 3 Coins
		}
		else if (m_fElapsedTime >= 3 * fElementDuration && m_fElapsedTime < 4 * fElementDuration)
		{
			AddUIElement(0, L"Layer_LuckyLetters", L"Prototype_GameObject_LuckyLetters"); 
		}
		else if (m_fElapsedTime >= 4 * fElementDuration && m_fElapsedTime < 5 * fElementDuration)
		{
			AddUIElement(2, L"Layer_LuckyLetters", L"Prototype_GameObject_LuckyLetters"); 
		}
		else if (m_fElapsedTime >= 5 * fElementDuration && m_fElapsedTime < 6 * fElementDuration)
		{
			m_pGameInstance->Render_FontOutlined(L"Font_Default", to_wstring(m_pCoin->Coin_Gained()) + L"Coins", _float2(110.f, 55.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}
		else if (m_fElapsedTime >= 6 * fElementDuration && m_fElapsedTime < 7 * fElementDuration)
		{
			AddUIElement(3, L"Layer_LuckyComplete", L"Prototype_GameObject_LuckyComplete"); 
		}
		else if (m_fElapsedTime >= 7 * fElementDuration && m_fElapsedTime < 8 * fElementDuration)
		{
			AddUIElement(4, L"Layer_LuckyComplete", L"Prototype_GameObject_LuckyComplete"); 
		}

		else if (m_fElapsedTime >= 8 * fElementDuration && m_fElapsedTime < 9 * fElementDuration)
		{
			m_pGameInstance->Render_FontOutlined(L"Font_Default", L"Hidden Map", _float2(110.f, 55.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}
		else if (m_fElapsedTime >= 9 * fElementDuration && m_fElapsedTime < 10 * fElementDuration)
		{
			AddUIElement(4, L"Layer_LuckyComplete", L"Prototype_GameObject_LuckyComplete"); // 8 Lucky
		}
		else if (m_fElapsedTime >= 10 * fElementDuration && m_fElapsedTime < 11 * fElementDuration)
		{
			AddUIElement(6, L"Layer_ LuckyLetters", L"Prototype_GameObject_LuckyLetters"); // 5 Coin Colored Icon
		}
		else if (m_fElapsedTime >= 11 * fElementDuration && m_fElapsedTime < 12 * fElementDuration)
		{
			AddUIElement(8, L"Layer_ LuckyLetters", L"Prototype_GameObject_LuckyLetters"); 
		}
		else if (m_fElapsedTime >= 12 * fElementDuration && m_fElapsedTime < 13 * fElementDuration)
		{
			AddUIElement(10, L"Layer_ LuckyLetters", L"Prototype_GameObject_LuckyLetters"); 
		}
		else if (m_fElapsedTime >= 13 * fElementDuration && m_fElapsedTime < 14 * fElementDuration)
		{
			AddUIElement(12, L"Layer_ LuckyLetters", L"Prototype_GameObject_LuckyLetters"); 
		}
		else if (m_fElapsedTime >= 14 * fElementDuration && m_fElapsedTime < 15 * fElementDuration)
		{
			AddUIElement(14, L"Layer_ LuckyLetters", L"Prototype_GameObject_LuckyLetters"); 
		}
		else if (m_fElapsedTime >= 15 * fElementDuration && m_fElapsedTime < 16 * fElementDuration)
		{
			AddUIElement(16, L"Layer_ LuckyLetters", L"Prototype_GameObject_LuckyLetters"); 
		}
		if (m_fElapsedTime >= 16 * fElementDuration)
		{
			m_fElapsedTime = 0.0f;
		}
		/*	m_bShow = false;
			}*/
	}
}

HRESULT CLevel_Puzzle::Render()
{
	SetWindowText(g_hWnd, L"New Super Lucky's Tale: Puzzle Stage");

	m_pGameInstance->Render_FontOutlined(L"Font_Default", to_wstring(m_pLives->Get_PlayerLife()), _float2(110.f, 55.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pGameInstance->Render_FontOutlined(L"Font_Default", L"x " + to_wstring(m_pCoin->Coin_Gained()), _float2(117.f, 115.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	if (m_bDebug)
		Render_PlayerState();

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(0.5f, -1.f, 0.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::LIGHT_POINT;
	LightDesc.vPosition = _float4(10.f, 3.f, 5.f, 1.f);
	LightDesc.fRange = 5.f;

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_UI(const wstring& strLayerTag)
{
	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Clover_Check", TEXT("Prototype_GameObject_UI_Clover_Check"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_UI_Lives", TEXT("Prototype_GameObject_UI_Lives"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Clover_Question", TEXT("Prototype_GameObject_UI_Clover_Question"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Clover_Paw", TEXT("Prototype_GameObject_UI_Clover_Paw"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Clover_Lucky", TEXT("Prototype_GameObject_UI_Clover_L"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_UI_Coins", TEXT("Prototype_GameObject_UI_Coins"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Heart_Outline", TEXT("Prototype_GameObject_UI_Heart_Outline"));

	CUI_Heart_Filling::HEART_DESC L_HeartDesc;
	L_HeartDesc.fX = g_iWinSizeX * 0.175f;
	L_HeartDesc.fY = g_iWinSizeY * 0.08f;

	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Heart_Filling", TEXT("Prototype_GameObject_UI_Heart_Filling"), &L_HeartDesc);

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_Models(const wstring& strLayerTag)
{
	_ulong dwByte = { 0 };
	m_hModelFile = CreateFile(
		L"C:/NSLT_3D/Client/Bin/Resources/SaveFiles/SkyCastle/PuzzleMap.dat",
		GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == m_hModelFile)
	{
		MSG_BOX(L"[ImGui] Failed to Create .dat file");
		return E_FAIL;
	}

	CImGui_Manager::m_ModelCounter.clear();

	size_t iModelCnt = {};
	ReadFile(m_hModelFile, &iModelCnt, sizeof size_t, &dwByte, nullptr);

	while (iModelCnt)
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

		CModelObjects_Boss::MODEL_DESC* pModelDesc = new CModelObjects_Boss::MODEL_DESC;

		ReadFile(m_hModelFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);

		DWORD iLen1 = 0;
		DWORD iLen2 = 0;
		ReadFile(m_hModelFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
		ReadFile(m_hModelFile, &iLen2, sizeof DWORD, &dwByte, nullptr);

		ReadFile(m_hModelFile, pModelDesc->strName, iLen1, &dwByte, nullptr);
		ReadFile(m_hModelFile, pModelDesc->strNumbered, iLen2, &dwByte, nullptr);
		pModelDesc->strName[iLen1] = '\0';
		pModelDesc->strNumbered[iLen2] = '\0';

		pModelDesc->matModel = WorldMatrix;

		wstring modelName;
		CharToWString(pModelDesc->strNumbered, modelName);
		m_ModelDesc.emplace(modelName, pModelDesc);

		--iModelCnt;

		string modelNameStr = pModelDesc->strName;
		wstring modelNameWStr(modelNameStr.begin(), modelNameStr.end());

		CImGui_Manager::m_ModelCounter[modelNameWStr]++;
	}

	CloseHandle(m_hModelFile);

	for (auto& pModelDesc : m_ModelDesc)
	{
		_char szConvert[MAX_PATH] = { };

		_char szTmp[256];
		_char szNumbered[256];
		strcpy(szTmp, pModelDesc.second->strName);
		strcpy(szNumbered, pModelDesc.second->strNumbered);
		wstring strTemp, strNumbered;
		CharToWString(szTmp, strTemp);
		CharToWString(szNumbered, strNumbered);

		wstring strFullPath = L"C:/NSLT_3D/Client/Bin/Resources/Models/MapObjects/" + strTemp + L"/Visuals.fbx";
		WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), -1, szConvert, MAX_PATH, nullptr, nullptr);

		_matrix LocalTransformMatrix = XMMatrixIdentity();

		wstring strPrototypeComponent = L"Prototype_Component_Model_" + strTemp;

		HRESULT hr;
		hr = m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, strPrototypeComponent.c_str(),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szConvert, LocalTransformMatrix));

		/* Create Prototype GameObject */
		wstring strPrototypeGameObject = L"Prototype_GameObject_" + strTemp;
		hr = m_pGameInstance->Add_Prototype(strPrototypeGameObject,
			CModelObjects_Boss::Create(m_pDevice, m_pContext, strPrototypeComponent.c_str()));

		// Clone 
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, L"Layer_ModelObjects", L"Prototype_GameObject_" + strTemp, pModelDesc.second)))
		{
			MSG_BOX(L"CImGui_Manager - Load Failed!");
			return E_FAIL;
		}
		m_pTool->Add_SelectedModels(strNumbered);
	}

	for (auto& pModelDesc : m_ModelDesc)
		Safe_Delete(pModelDesc.second);

	m_ModelDesc.clear();

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_Collectibles(const wstring& strLayerTag)
{
	_ulong dwByte = { 0 };
	m_hModelFile = CreateFile(
		L"C:/NSLT_3D/Client/Bin/Resources/SaveFiles/SkyCastle/Puzzle_Collectibles1.dat",
		GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == m_hModelFile)
	{
		MSG_BOX(L"[ImGui] Failed to Create .dat file");
		return E_FAIL;
	}

	CImGui_Manager::m_ModelCounter.clear();

	size_t iModelCnt = {};
	ReadFile(m_hModelFile, &iModelCnt, sizeof size_t, &dwByte, nullptr);


	while (iModelCnt)
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

		CCollectibles_Boss::CLTB_DESC* pModelDesc = new CCollectibles_Boss::CLTB_DESC;

		ReadFile(m_hModelFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);

		DWORD iLen1 = 0;
		DWORD iLen2 = 0;
		ReadFile(m_hModelFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
		ReadFile(m_hModelFile, &iLen2, sizeof DWORD, &dwByte, nullptr);

		ReadFile(m_hModelFile, pModelDesc->strName, iLen1, &dwByte, nullptr);
		ReadFile(m_hModelFile, pModelDesc->strNumbered, iLen2, &dwByte, nullptr);
		pModelDesc->strName[iLen1] = '\0';
		pModelDesc->strNumbered[iLen2] = '\0';

		pModelDesc->matModel = WorldMatrix;

		wstring modelName;
		CharToWString(pModelDesc->strNumbered, modelName);
		m_CltbDesc.emplace(modelName, pModelDesc);

		--iModelCnt;

		string modelNameStr = pModelDesc->strName;
		wstring modelNameWStr(modelNameStr.begin(), modelNameStr.end());

		CImGui_Manager::m_ModelCounter[modelNameWStr]++;

	}

	CloseHandle(m_hModelFile);

	for (auto& pModelDesc : m_CltbDesc)
	{
		_char szConvert[MAX_PATH] = { };

		_char szTmp[256];
		_char szNumbered[256];
		strcpy(szTmp, pModelDesc.second->strName);
		strcpy(szNumbered, pModelDesc.second->strNumbered);
		wstring strTemp, strNumbered;
		CharToWString(szTmp, strTemp);
		CharToWString(szNumbered, strNumbered);

		wstring strFullPath = L"C:/NSLT_3D/Client/Bin/Resources/Models/Collectibles/" + strTemp + L"/Visuals.fbx";
		WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), -1, szConvert, MAX_PATH, nullptr, nullptr);

		_matrix LocalTransformMatrix = XMMatrixIdentity();

		wstring strPrototypeComponent = L"Prototype_Component_Model_" + strTemp;

		HRESULT hr;
		hr = m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, strPrototypeComponent.c_str(),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szConvert, LocalTransformMatrix));

		/* Create Prototype GameObject */
		wstring strPrototypeGameObject = L"Prototype_GameObject_" + strTemp;
		hr = m_pGameInstance->Add_Prototype(strPrototypeGameObject,
			CCollectibles_Boss::Create(m_pDevice, m_pContext, strPrototypeComponent.c_str()));

		// Clone 
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, L"Layer_Collectibles", L"Prototype_GameObject_" + strTemp, pModelDesc.second)))
		{
			MSG_BOX(L"CImGui_Manager - Load Failed!");
			return E_FAIL;
		}
		m_pTool->Add_SelectedCltbs(strNumbered);
	}

	for (auto& pCltbDesc : m_CltbDesc)
		Safe_Delete(pCltbDesc.second);

	m_CltbDesc.clear();

	return S_OK;
}

void CLevel_Puzzle::Render_PlayerState()
{
	_uint iPlayerState = m_pPlayer->Get_PlayerState();
	const TCHAR* stateName = TEXT(" ");

	switch (iPlayerState)
	{
	case IDLE:
		stateName = TEXT("IDLE");
		break;
	case WALK:
		stateName = TEXT("WALK");
		break;
	case ATT:
		stateName = TEXT("ATT");
		break;
	case BURROW:
		stateName = TEXT("BURROW");
		break;
	case BURROWWALK:
		stateName = TEXT("BURROWWALK");
		break;
	case JUMP:
		stateName = TEXT("JUMP");
		break;
	case JUMPING:
		stateName = TEXT("JUMPING");
		break;
	case DIVE:
		stateName = TEXT("DIVE");
		break;
	case JUMPATT:
		stateName = TEXT("JUMPATT");
		break;
	case TURN_L:
		stateName = TEXT("TURN_L");
		break;
	case TURN_R:
		stateName = TEXT("TURN_R");
		break;
	case CARRY:
		stateName = TEXT("CARRY");
		break;
	case PICKUP:
		stateName = TEXT("PICKUP");
		break;
	default:
		stateName = TEXT(" ");
		break;
	}

	wsprintf(m_szPlayerState, TEXT("Player State: %s"), stateName);
	m_pGameInstance->Render_Font(L"Font_Default", m_szPlayerState, _float2(10.f, 680.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));

	_float3 vPlayerPos;
	_tchar szPlayerPos[256];
	XMStoreFloat3(&vPlayerPos, m_pPlayer->Get_Player_Position());
	swprintf_s(szPlayerPos, sizeof(szPlayerPos) / sizeof(_tchar), TEXT("Position: (%.2f, %.2f, %.2f)"), vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);
	m_pGameInstance->Render_Font(L"Font_Default", szPlayerPos, _float2(10.f, 650.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
}

HRESULT CLevel_Puzzle::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	CTerrain::TERRAIN_DESC TerrainDesc;
	TerrainDesc.eLevel = LEVEL_PUZZLE;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, strLayerTag, TEXT("Prototype_GameObject_Terrain"), &TerrainDesc)))
		return E_FAIL;

	CSky::SKY_DESC SkyDesc;
	SkyDesc.eLevel = LEVEL_PUZZLE;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, strLayerTag, TEXT("Prototype_GameObject_Sky"), &SkyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_Effect(const wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_LandObjects()
{
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_MapObjects(const wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Animated(const wstring& strLayerTag)
{
	/* FlowerBoss */
	CAnim_Boss::BOSS_DESC BossDesc;
	BossDesc.vPos = { 50.f, 17.98f, 50.f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, strLayerTag, L"Prototype_GameObject_Anim_Boss", &BossDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_Monster(const wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_NPCs(const wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_Player(const wstring& strLayerTag)
{
	CPlayer::PLAYER_DESC PlayerDesc;
	PlayerDesc.m_eLevel = LEVEL_PUZZLE;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, strLayerTag, TEXT("Prototype_GameObject_Player"), &PlayerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera::CAMERA_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(65.f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.eLevel = LEVEL_PUZZLE;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, strLayerTag, TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

wstring CLevel_Puzzle::CharToWString(_char* szSrc, wstring& strDest)
{
	if (szSrc == nullptr)
	{
		strDest.clear();
		return strDest;
	}

	int size_needed = MultiByteToWideChar(CP_ACP, 0, szSrc, (int)strlen(szSrc), NULL, 0);

	strDest.resize(size_needed);

	MultiByteToWideChar(CP_ACP, 0, szSrc, (int)strlen(szSrc), &strDest[0], size_needed);

	return strDest;
}

CLevel_Puzzle* CLevel_Puzzle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Puzzle* pInstance = new CLevel_Puzzle(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Puzzle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Puzzle::Free()
{
	__super::Free();

	for (auto& Pair : m_ModelDesc)
		Safe_Delete(Pair.second);
	m_ModelDesc.clear();

	for (auto& Cltb : m_CltbDesc)
		Safe_Delete(Cltb.second);
	m_CltbDesc.clear();

	if (m_pPlayer)
		Safe_Release(m_pPlayer);

	if (m_pCoin)
		Safe_Release(m_pCoin);

	if (m_pLives)
		Safe_Release(m_pLives);

	if (m_pQuestion)
		Safe_Release(m_pQuestion);

	Safe_Release(m_pTool);
}
