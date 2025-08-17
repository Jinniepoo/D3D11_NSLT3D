#include "stdafx.h"
#include "..\Public\Level_Boss.h"
#include "GameInstance.h"

#include "Gameplay_Defines.h"

CLevel_Boss::CLevel_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_hModelFile{}
{
}

HRESULT CLevel_Boss::Initialize()
{
	m_iLevelID = LEVEL_BOSS;

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

	//if (FAILED(Ready_Layer_UI(L"Layer_UI")))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;
	
	if (FAILED(Ready_MapObjects(L"Layer_MapObjects")))
		return E_FAIL;

	if (FAILED(Ready_Animated(L"Layer_Animated")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_CameraPlayer"))))
		return E_FAIL;

	list<CGameObject*>* pPlayer = GET_LIST(LEVEL_BOSS, L"Layer_Player");

	if (0 >= pPlayer->size())
	{
		MSG_BOX(L"PlayerCamera - Failed to Find Layer");
		return E_FAIL;
	}
	m_pPlayer = (CPlayer*)pPlayer->front();
	Safe_AddRef(m_pPlayer);

	//m_pCoin = dynamic_cast<CUI_Coins*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_Coins")->front());
	//Safe_AddRef(m_pCoin);

	return S_OK;
}

void CLevel_Boss::Tick(_float fTimeDelta)
{
	if (KEY_PUSH(DIK_APOSTROPHE))
	{
		if (m_bDebug)
			m_bDebug = false;

		else if (!m_bDebug)
			m_bDebug = true;
	}
}

HRESULT CLevel_Boss::Render()
{
	SetWindowText(g_hWnd, L"New Super Lucky's Tale: Boss Stage");

	//m_pGameInstance->Render_FontOutlined(L"Font_Default", to_wstring(m_pPlayer->Get_PlayerLife()), _float2(110.f, 55.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//m_pGameInstance->Render_FontOutlined(L"Font_Default", L"x " + to_wstring(m_pCoin->Coin_Gained()), _float2(117.f, 115.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	if (m_bDebug)
		Render_PlayerState();

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::LIGHT_POINT;
	LightDesc.vPosition = _float4(10.f, 3.f, 10.f, 1.f);
	LightDesc.fRange = 10.f;

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_UI(const wstring& strLayerTag)
{
	m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Lives"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Clover_Check"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Clover_Question"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Clover_Paw"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Clover_L"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_UI_Coins", TEXT("Prototype_GameObject_UI_Coins"));
	m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_Heart_Outline"));

	CUI_Heart_Filling::HEART_DESC L_HeartDesc;
	L_HeartDesc.fX = g_iWinSizeX * 0.13f;
	L_HeartDesc.fY = g_iWinSizeY * 0.08f;

	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_UI_Heart_Filling"), &L_HeartDesc);

	CUI_Heart_Filling::HEART_DESC C_HeartDesc;
	C_HeartDesc.fX = g_iWinSizeX * 0.1742f;
	C_HeartDesc.fY = g_iWinSizeY * 0.08f;

	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_UI_Heart_Filling"), &C_HeartDesc);

	CUI_Heart_Filling::HEART_DESC R_HeartDesc;
	R_HeartDesc.fX = g_iWinSizeX * 0.2192f;
	R_HeartDesc.fY = g_iWinSizeY * 0.08f;

	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_UI_Heart_Filling"), &R_HeartDesc);

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Models(const wstring& strLayerTag)
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

HRESULT CLevel_Boss::Ready_Layer_Collectibles(const wstring& strLayerTag)
{
	_ulong dwByte = { 0 };
	m_hModelFile = CreateFile(
		//L"C:/NSLT_3D/Client/Bin/Resources/SaveFiles/SkyCastle/Test.dat",
		L"C:/NSLT_3D/Client/Bin/Resources/SaveFiles/SkyCastle/Collectibles11.dat",
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

void CLevel_Boss::Render_PlayerState()
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

HRESULT CLevel_Boss::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Effect(const wstring& strLayerTag)   
{
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Log"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Boss::Ready_LandObjects()
{
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Boss::Ready_MapObjects(const wstring& strLayerTag)
{
	/* MazeDoor, MazeBell, MovingWall */
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MazeDoorL"));
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MazeDoorR"));
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MazeBell"));
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MovingWall"));
	
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


	return S_OK;
}

HRESULT CLevel_Boss::Ready_Animated(const wstring& strLayerTag)
{
	/* Checkpoint */
	CCheckpoint::CHECK_DESC CheckDesc;
	CheckDesc.vCheckPos = { 57.5f, 11.80f, 104.5f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_Anim_Checkpoint", &CheckDesc)))
		return E_FAIL;

	/* GolemHead1 */
	CAnim_GolemHead::GOLEM_DESC GolemDesc;
	GolemDesc.vPos = { 50.12f, 17.98f, 126.56f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_Anim_GolemHead", &GolemDesc)))
		return E_FAIL;

	/* FoxHole */
	CAnim_Foxhole::HOLE_DESC HoleDesc;
	HoleDesc.vPos = { 50.12f, 17.98f, 126.56f, 1.f };
	//HoleDesc.vPos = { 55.12f, 17.98f, 129.56f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, L"Prototype_GameObject_Anim_FoxHole", &HoleDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const wstring& strLayerTag)
{
	CMonster::MONSTER_DESC MonsterDesc;
	MonsterDesc.vMonsterPos = { 72.f, 10.f, 35.3f, 1.f };
	MonsterDesc.iCellIndex = 94;
	MonsterDesc.fRotation = 90;
	
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc)))
		return E_FAIL;

	CMonster::MONSTER_DESC MonsterDesc1;
	MonsterDesc1.vMonsterPos = { 62.5f, 10.f, 49.1f, 1.f };
	MonsterDesc1.iCellIndex = 91;
	MonsterDesc1.fRotation = 210;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc1)))
		return E_FAIL;

	/*CMonster::MONSTER_DESC MonsterDesc2;
	MonsterDesc.vMonsterPos = { 40.5f, 10.4f, 92.1f, 1.f };
	MonsterDesc.iCellIndex = 94;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &MonsterDesc2)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Boss::Ready_NPCs(const wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Player(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Camera(const wstring& strLayerTag)
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

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

wstring CLevel_Boss::CharToWString(_char* szSrc, wstring& strDest)
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

CLevel_Boss* CLevel_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Boss* pInstance = new CLevel_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Boss::Free()
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

	Safe_Release(m_pTool);
}
