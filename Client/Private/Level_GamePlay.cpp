#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"

#include "Gameplay_Defines.h"
#include "Level_Loading.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_hModelFile{}
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	m_iLevelID = LEVEL_GAMEPLAY;

	m_pTool = CImGui_Manager::Get_Instance();
	Safe_AddRef(m_pTool);

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(L"Layer_UI")))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Collectibles(L"Layer_Collectibles")))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_MapObjects(L"Layer_MapObjects")))
		return E_FAIL;

	if (FAILED(Ready_Animated(L"Layer_Animated")))
		return E_FAIL;

	if (FAILED(Ready_NPCs(L"Layer_NPCs")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Models(L"Layer_Models")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_CameraPlayer"))))
		return E_FAIL;

	list<CGameObject*>* pPlayer = GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player");

	if (0 >= pPlayer->size())
	{
		MSG_BOX(L"PlayerCamera - Failed to Find Layer");
		return E_FAIL;
	}
	m_pPlayer = (CPlayer*)pPlayer->front();
	Safe_AddRef(m_pPlayer);

	m_pCoin = dynamic_cast<CUI_Coins*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_Coins")->front());
	Safe_AddRef(m_pCoin);

	m_pHole = dynamic_cast<CAnim_Foxhole*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_FoxHole")->front());
	Safe_AddRef(m_pHole);

	m_pLives = dynamic_cast<CUI_Lives*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_Lives")->front());
	Safe_AddRef(m_pLives);

	/* Sound */
	PLAYTOBGM(L"BGM_SkyCastle", true);
	VOLUME(L"BGM_SkyCastle", 50);

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (KEY_PUSH(DIK_APOSTROPHE))
	{
		if (m_bDebug)
			m_bDebug = false;

		else if (!m_bDebug)
			m_bDebug = true;
	}

	if (true == m_pHole->ToPuzzle())
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_PUZZLE))))
			return;
	}	
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, L"New Super Lucky's Tale: Game Play");

	//m_pGameInstance->Render_FontOutlined(L"Font_Default", to_wstring(m_pPlayer->Get_PlayerLife()), _float2(110.f, 55.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pGameInstance->Render_FontOutlined(L"Font_Default", to_wstring(m_pLives->Get_PlayerLife()), _float2(110.f, 55.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pGameInstance->Render_FontOutlined(L"Font_Default", L"x " + to_wstring(m_pCoin->Coin_Gained()), _float2(117.f, 115.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	if (m_bDebug)
		Render_PlayerState();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::LIGHT_POINT;
	LightDesc.vPosition = _float4(5.f, 3.f, 50.f, 1.f);
	LightDesc.fRange = 10.f;

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 3.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const wstring& strLayerTag)
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

HRESULT CLevel_GamePlay::Ready_Layer_Models(const wstring& strLayerTag)
{
	_ulong dwByte = { 0 };
	m_hModelFile = CreateFile(
		//L"C:/NSLT_3D/Client/Bin/Resources/SaveFiles/SkyCastle/SkyCastle_Updated.dat",
		L"C:/NSLT_3D/Client/Bin/Resources/SaveFiles/SkyCastle/CompletedTest.dat",
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

		CModelObjects::MODEL_DESC* pModelDesc = new CModelObjects::MODEL_DESC;

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

	//m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, L"Layer_ModelObjects");

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
		hr = m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeComponent.c_str(),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szConvert, LocalTransformMatrix));

		/* Create Prototype GameObject */
		wstring strPrototypeGameObject = L"Prototype_GameObject_" + strTemp;
		hr = m_pGameInstance->Add_Prototype(strPrototypeGameObject,
			CModelObjects::Create(m_pDevice, m_pContext, strPrototypeComponent.c_str()));

		// Clone 
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_ModelObjects", L"Prototype_GameObject_" + strTemp, pModelDesc.second)))
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

HRESULT CLevel_GamePlay::Ready_Layer_Collectibles(const wstring& strLayerTag)
{
	_ulong dwByte = { 0 };
	m_hModelFile = CreateFile(
		//L"C:/NSLT_3D/Client/Bin/Resources/SaveFiles/SkyCastle/Test.dat",
		L"C:/NSLT_3D/Client/Bin/Resources/SaveFiles/SkyCastle/Collectibles0823.dat",
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

		CCollectible_Coin::CLTB_DESC* pModelDesc = new CCollectible_Coin::CLTB_DESC;

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

	//m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, L"Layer_ModelObjects");

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
		hr = m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeComponent.c_str(),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szConvert, LocalTransformMatrix));

		/* Create Prototype GameObject */
		wstring strPrototypeGameObject = L"Prototype_GameObject_" + strTemp;
		hr = m_pGameInstance->Add_Prototype(strPrototypeGameObject,
			CCollectible_Coin::Create(m_pDevice, m_pContext, strPrototypeComponent.c_str()));

		// Clone 
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_Collectibles", L"Prototype_GameObject_" + strTemp, pModelDesc.second)))
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

void CLevel_GamePlay::Render_PlayerState()
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

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	CTerrain::TERRAIN_DESC TerrainDesc;
	TerrainDesc.eLevel = LEVEL_GAMEPLAY;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"), &TerrainDesc)))
		return E_FAIL;

	CSky::SKY_DESC SkyDesc;
	SkyDesc.eLevel = LEVEL_GAMEPLAY;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"), &SkyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_MapObjects(const wstring& strLayerTag)
{
	/* MazeDoor, MazeBell, MovingWall */
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MazeDoorL"));
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MazeDoorR"));
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MazeBell"));
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MovingWall"));

	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_NoHead1"));
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_NoHead2"));
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_NoHead3"));

	/* Red_Spring */
	CSpring_Mushroom::SPRING_DESC SpringDesc; // 1 After Monster1
	SpringDesc.vPos = { 63.33f, 10.f, 30.68f, 1.f };
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Spring_Mushroom"), &SpringDesc);

	CSpring_Mushroom::SPRING_DESC SpringDesc2; // 2 Before Bell
	SpringDesc2.vPos = { 72.75f, 16.75f, 22.42f, 1.f };
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Spring_Mushroom"), &SpringDesc2);

	CSpring_Mushroom::SPRING_DESC SpringDesc3; // 3 CheckPoint
	SpringDesc3.vPos = { 44.29f, 10.80f, 100.f, 1.f };
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Spring_Mushroom"), &SpringDesc3);

	/* WoodCrates */
	CWoodenCrate::CRATE_DESC CrateDesc;
	CrateDesc.vPos = { 5.f, 0.9f, 5.f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_WoodenCrate_Single", &CrateDesc)))
		return E_FAIL;

	CWoodenCrate_Heart::CRATE_DESC HeartCrateDesc;
	HeartCrateDesc.vPos = { 20.f, 0.9f, 20.f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_WoodenCrate_Heart", &HeartCrateDesc)))
		return E_FAIL;

	CWoodenCrate_Triple::CRATE3_DESC Crate3Desc;
	Crate3Desc.vPos = { 71.28f, 0.9f, 17.43f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_WoodenCrate_Triple", &Crate3Desc)))
		return E_FAIL;

	Crate3Desc.vPos = { 82.21f, 18.88f, 126.33f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_WoodenCrate_Triple", &Crate3Desc)))
		return E_FAIL;

	Crate3Desc.vPos = { 110.54f, 18.84f, 147.05f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_WoodenCrate_Triple", &Crate3Desc)))
		return E_FAIL;

	Crate3Desc.vPos = { 106.74f, 18.84f, 149.09f, 1.f };
	Crate3Desc.fRot = -15.f;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_WoodenCrate_Triple", &Crate3Desc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Animated(const wstring& strLayerTag)
{
	/* Checkpoint */
	CCheckpoint::CHECK_DESC CheckDesc;
	CheckDesc.vCheckPos = { 57.5f, 11.80f, 104.5f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_Anim_Checkpoint", &CheckDesc)))
		return E_FAIL;

	/* GolemHead1 */
	CAnim_GolemHead::GOLEM_DESC GolemDesc;
	GolemDesc.vPos = { 50.12f, 18.2f, 126.56f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_Anim_GolemHead", &GolemDesc)))
		return E_FAIL;

	/* GolemHead2 */
	//GolemDesc.vPos = { 56.37f, 17.98f, 131.26f, 1.f };
	GolemDesc.vPos = { -55.033f, 21.664f, 152.323f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_Anim_GolemHead", &GolemDesc)))
		return E_FAIL;

	/* GolemHead3 */
	GolemDesc.vPos = { 107.57f, 18.2f, 133.16f, 1.f };
	GolemDesc.fRot = 90.f;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_Anim_GolemHead", &GolemDesc)))
		return E_FAIL;

	/* FoxHole */
	CAnim_Foxhole::HOLE_DESC HoleDesc;
	HoleDesc.vPos = { -109.56f, 25.10f, 138.58f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_FoxHole", L"Prototype_GameObject_Anim_FoxHole", &HoleDesc)))
		return E_FAIL;

	///* GiantGolem */
	//CAnim_GiantGolem::GOLEM_DESC GolemDesc1;
	//GolemDesc1.vPos = { 50.12f, 40.98f, 180.f, 1.f };
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_Anim_GiantGolem", &GolemDesc1)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
	CMonster::MONSTER_DESC MonsterDesc;
	MonsterDesc.vMonsterPos = { 72.f, 10.f, 35.3f, 1.f };
	MonsterDesc.iInitialAnim = 4;
	MonsterDesc.iCellIndex = 94;
	MonsterDesc.fRotation = 270.f;
	MonsterDesc.m_eLevel = LEVEL_GAMEPLAY;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vMonsterPos = { 62.5f, 10.f, 49.1f, 1.f };
	MonsterDesc.iInitialAnim = 0;
	MonsterDesc.iCellIndex = 91;
	MonsterDesc.fRotation = 210.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vMonsterPos = { 38.62f, 10.93f, 97.48f, 1.f };
	MonsterDesc.iInitialAnim = 15;
	MonsterDesc.iCellIndex = 190;
	MonsterDesc.fRotation = 180.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vMonsterPos = { -1.35f, 17.97f, 122.38f, 1.f };
	MonsterDesc.iInitialAnim = 4;
	MonsterDesc.iCellIndex = 240;
	MonsterDesc.fRotation = 45.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vMonsterPos = { -14.69f, 17.97f, 121.77f, 1.f };
	MonsterDesc.iInitialAnim = 4;
	MonsterDesc.iCellIndex = 242;
	MonsterDesc.fRotation = 115.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vMonsterPos = { -17.93f, 17.97f, 135.26f, 1.f };
	MonsterDesc.iInitialAnim = 15;
	MonsterDesc.iCellIndex = 242;
	MonsterDesc.fRotation = 60.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;

	MonsterDesc.vMonsterPos = { -46.89f, 21.909f, 142.93f, 1.f };
	MonsterDesc.iInitialAnim = 15;
	MonsterDesc.iCellIndex = 242;
	MonsterDesc.fRotation = 95.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;
	MonsterDesc.vMonsterPos = { -51.78f, 21.09f, 135.34f, 1.f };
	MonsterDesc.iInitialAnim = 15;
	MonsterDesc.iCellIndex = 242;
	MonsterDesc.fRotation = 45.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;
	MonsterDesc.vMonsterPos = { -48.12f, 21.09f, 123.97f, 1.f };
	MonsterDesc.iInitialAnim = 15;
	MonsterDesc.iCellIndex = 242;
	MonsterDesc.fRotation = 165.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;

	CMonster_CrumbleR::MONSTER_DESC CrumbleRDesc;
	CrumbleRDesc.vMonsterPos = { -106.03f, 26.93f, 94.37f, 1.f };
	CrumbleRDesc.iInitialAnim = 0;
	CrumbleRDesc.iCellIndex = 94;
	CrumbleRDesc.fRotation = 270.f;
	CrumbleRDesc.m_eLevel = LEVEL_GAMEPLAY;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_CrumbleR"), &CrumbleRDesc)))
		return E_FAIL;

	CMonster_CrumbleB::MONSTER_DESC CrumbleBDesc;
	CrumbleBDesc.vMonsterPos = { -123.71f, 26.80f, 95.14f, 1.f };
	CrumbleBDesc.iInitialAnim = 4;
	CrumbleBDesc.iCellIndex = 94;
	CrumbleBDesc.fRotation = 80.f;
	CrumbleBDesc.m_eLevel = LEVEL_GAMEPLAY;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_CrumbleB"), &CrumbleBDesc)))
		return E_FAIL;

	CrumbleRDesc.vMonsterPos = { -130.13f, 26.80f, 81.32f, 1.f };
	CrumbleRDesc.iInitialAnim = 0;
	CrumbleRDesc.iCellIndex = 94;
	CrumbleRDesc.fRotation = 90.f;
	CrumbleRDesc.m_eLevel = LEVEL_GAMEPLAY;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_CrumbleR"), &CrumbleRDesc)))
		return E_FAIL;

	CrumbleBDesc.vMonsterPos = { -117.08f, 26.81f, 74.93f, 1.f };
	CrumbleBDesc.iInitialAnim = 4;
	CrumbleBDesc.iCellIndex = 94;
	CrumbleBDesc.fRotation = 45.f;
	CrumbleBDesc.m_eLevel = LEVEL_GAMEPLAY;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_CrumbleB"), &CrumbleBDesc)))
		return E_FAIL;

	//

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;
	MonsterDesc.vMonsterPos = { -81.93f, 23.49f, 130.01f, 1.f };
	MonsterDesc.iInitialAnim = 15;
	MonsterDesc.iCellIndex = 242;
	MonsterDesc.fRotation = 45.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;
	MonsterDesc.vMonsterPos = { -78.97f, 23.49f, 145.38f, 1.f };
	MonsterDesc.iInitialAnim = 4;
	MonsterDesc.iCellIndex = 242;
	MonsterDesc.fRotation = 85.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;
	MonsterDesc.vMonsterPos = { -89.32f, 23.49f, 154.05f, 1.f };
	MonsterDesc.iInitialAnim = 15;
	MonsterDesc.iCellIndex = 242;
	MonsterDesc.fRotation = 45.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;
	MonsterDesc.vMonsterPos = { -98.88f, 23.49f, 140.50f, 1.f };
	MonsterDesc.iInitialAnim = 4;
	MonsterDesc.iCellIndex = 242;
	MonsterDesc.fRotation = 165.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;
	MonsterDesc.vMonsterPos = { -102.f, 23.49f, 125.47f, 1.f };
	MonsterDesc.iInitialAnim = 4;
	MonsterDesc.iCellIndex = 242;
	MonsterDesc.fRotation = 165.f;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_NPCs(const wstring& strLayerTag)
{
	///* NPC - Guard */
	//CAnim_Guard::GUARD_DESC GuardDesc;
	//GuardDesc.vPos = { 32.6684f, 0.0115761f, 35.7767f, 1.f };
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_Anim_NPC_Guard", &GuardDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& strLayerTag)
{
	CPlayer::PLAYER_DESC PlayerDesc;
	PlayerDesc.m_eLevel = LEVEL_GAMEPLAY;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"), &PlayerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& strLayerTag)
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
	CameraDesc.eLevel = LEVEL_GAMEPLAY;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

wstring CLevel_GamePlay::CharToWString(_char* szSrc, wstring& strDest)
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

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	STOP(L"BGM_SkyCastle");

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

	if (m_pHole)
		Safe_Release(m_pHole);

	if (m_pLives)
		Safe_Release(m_pLives);

	Safe_Release(m_pTool);
}
