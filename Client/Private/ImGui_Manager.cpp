#include "stdafx.h"
#include "ImGui_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
#include "Layer.h"
#include "Navigation.h"
#include "Player.h"
#include "Level.h"

using namespace ImGuizmo;

IMPLEMENT_SINGLETON(CImGui_Manager)

map<wstring, int> CImGui_Manager::m_ModelCounter;

CImGui_Manager::CImGui_Manager()
{
}

HRESULT CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pGameInstance = CGameInstance::Get_Instance();

	m_eLevel = GET_CURLEVEL;

	for (auto& pInputLayout : m_pSelectedModels)
		Safe_AddRef(pInputLayout);

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsClassic();

	io.Fonts->AddFontFromFileTTF("../Bin/Resources/Fonts/NotoSansKR-Regular.otf", 18.f, nullptr, io.Fonts->GetGlyphRangesKorean());

	vecModels = Find_FBXModel(L"../Bin/Resources/Models/MapObjects/");
	vecCltbs = Find_CLTBModel(L"../Bin/Resources/Models/Collectibles/");

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);

	return S_OK;
}

void CImGui_Manager::Tick()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (KEY_PUSH(DIK_F9))
	{
		if (m_bShowGui)
		{
			m_bShowGui = false;
			return;
		}
		else
		{
			m_bShowGui = true;
			return;
		}
	}

	if (KEY_PUSH(DIK_1))
		m_tGuizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;

	else if (KEY_PUSH(DIK_2))
		m_tGuizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;

	else if (KEY_PUSH(DIK_3))
		m_tGuizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;

	if (nullptr != m_pDebugCamTransform)
		m_vDebugCamPos = m_pDebugCamTransform->Get_State(CTransform::STATE_POSITION);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (m_bShowGui)
	{
		ImGui_MapTool();
		ImGui::End();
	}
	ImGui::EndFrame();
}

HRESULT CImGui_Manager::Render()
{
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return S_OK;
}

void CImGui_Manager::Move_Mouse(_bool bMove)
{
	if (true == m_bMouseMove)
	{
		if (false == bMove)
		{
			m_bMouseMove = false;
			static_cast<CCamera_Free*>(m_pDebugCam)->OnOffMouseCam(m_bMouseMove);
		}
	}
	else
	{
		if (true == bMove)
		{
			m_bMouseMove = true;
			static_cast<CCamera_Free*>(m_pDebugCam)->OnOffMouseCam(m_bMouseMove);
		}
	}
}

void CImGui_Manager::Change_Camera(_bool bChange)
{

}

wstring CImGui_Manager::CharToWString(_char* szSrc, wstring& strDest)
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

_float3 CImGui_Manager::XMQuaternionToEulerAngles(FXMVECTOR q)
{
	XMFLOAT3 angles;

	// Roll (x-axis rotation)
	float sinr_cosp = 2 * (XMVectorGetX(q) * XMVectorGetY(q) + XMVectorGetZ(q) * XMVectorGetW(q));
	float cosr_cosp = 1 - 2 * (XMVectorGetY(q) * XMVectorGetY(q) + XMVectorGetZ(q) * XMVectorGetZ(q));
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// Pitch (y-axis rotation)
	float sinp = 2 * (XMVectorGetX(q) * XMVectorGetZ(q) - XMVectorGetW(q) * XMVectorGetY(q));
	if (std::abs(sinp) >= 1)
		angles.y = std::copysign(XM_PI / 2, sinp);
	else
		angles.y = std::asin(sinp);

	// Yaw (z-axis rotation)
	float siny_cosp = 2 * (XMVectorGetX(q) * XMVectorGetW(q) + XMVectorGetY(q) * XMVectorGetZ(q));
	float cosy_cosp = 1 - 2 * (XMVectorGetZ(q) * XMVectorGetZ(q) + XMVectorGetW(q) * XMVectorGetW(q));
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}

void CImGui_Manager::ImGui_MapTool()
{
	static _float3 vPos = { 0.f, 0.f, 0.f };
	static _float3 vScale = { 0.5f, 0.5f, 0.5f };
	static _float3 fRot = { 0.f, 0.f, 0.f };

	_float4x4 WorldMat, ViewMat, ProjMat;

	_char strConverted[MAX_PATH] = "";
	_char strConverted1[MAX_PATH] = "";

	ImGui::Begin("New Super Lucky's Tale");

	ImGui::SeparatorText("Camera Position");
	ImGui::Text("(%2.2f, %2.2f, %2.2f)", XMVectorGetX(m_vDebugCamPos), XMVectorGetY(m_vDebugCamPos), XMVectorGetZ(m_vDebugCamPos));

	ImGui::Checkbox("Move Camera", &m_bMoveCamera); // Move Camera

	if (m_pDebugCam != nullptr)
	{
		if (MOUSE_HOLD(DIMK_WHEEL))
		{
			Move_Mouse(true);
			m_bMoveCamera = true;
		}
		else if (MOUSE_AWAY(DIMK_WHEEL))
		{
			Move_Mouse(false);
			m_bMoveCamera = false;
		}
	}

	ImGui::SameLine(150.f);

	if (ImGui::Checkbox("Debug Camera", &m_bChangeCamera)) // Debug Camera
	{
		if (true == m_bChangeCamera)
		{
			/* Camera_Free[ON] */
			list<CGameObject*> CameraFree = *(GET_LIST(LEVEL_STATIC2, L"Layer_CameraFree"));
			m_pDebugCam = CameraFree.front();
			Safe_AddRef(m_pDebugCam);

			static_cast<CCamera_Free*>(m_pDebugCam)->OnOffDebugCam(true);
			m_pDebugCamTransform = FIND_TRANSFORM(m_pDebugCam, L"Com_Transform");
			Safe_AddRef(m_pDebugCamTransform);

			/* Camera_Player[OFF] */
			list<CGameObject*> CameraPlayer = *(GET_LIST(LEVEL_GAMEPLAY, L"Layer_CameraPlayer"));
			m_pPlayerCam = CameraPlayer.front();
			Safe_AddRef(m_pPlayerCam);
			static_cast<CCamera_Player*>(m_pPlayerCam)->ActivatePlayerCam(false);

			m_pPlayerCamTransform = FIND_TRANSFORM(m_pPlayerCam, L"Com_Transform");
			Safe_AddRef(m_pPlayerCamTransform);
		}

		else
		{
			/* Camera_Free[OFF] */
			static_cast<CCamera_Free*>(m_pDebugCam)->OnOffDebugCam(false);
			Safe_Release(m_pDebugCam);
			Safe_Release(m_pDebugCamTransform);

			/* Camera_Player[ON] */
			static_cast<CCamera_Player*>(m_pPlayerCam)->ActivatePlayerCam(true);
			Safe_Release(m_pPlayerCam);
			Safe_Release(m_pPlayerCamTransform);

			m_pDebugCam = nullptr;
			m_pPlayerCam = nullptr;
			m_pDebugCamTransform = nullptr;
			m_pPlayerCamTransform = nullptr;
		}
	}

	if (KEY_PUSH(DIK_O))
	{
		if (true == m_bChangeCamera)
			m_bChangeCamera = false;

		else if (false == m_bChangeCamera)
			m_bChangeCamera = true;
	}

	ImGuiTabBarFlags tbf = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("My Tab Bar", tbf))
	{
		/* Non-Anim Models */
		if (ImGui::BeginTabItem("Static Mesh")) {
			ImGui::SeparatorText("Select Model");
			static int iCurIdx = 0;
			static int iCurIdxSelected = 0;
			if (ImGui::BeginListBox(" "))
			{
				for (size_t i = 0; i < vecModels.size(); ++i) {
					const auto& modelFile = vecModels[i];
					const bool isSelected = (iCurIdx == i);
					std::string strTemp(modelFile.begin(), modelFile.end());

					if (ImGui::Selectable(strTemp.c_str(), isSelected)) {
						iCurIdx = (int)i;
						m_strFolderName = modelFile;
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}

			WideCharToMultiByte(CP_ACP, 0, m_strFolderName.c_str(), lstrlen(m_strFolderName.c_str()), strConverted, MAX_PATH, nullptr, nullptr);

			if (nullptr != m_pModelObjects)
			{
				CTransform* pModelTransform = dynamic_cast<CTransform*>(m_pModelObjects->Find_Component(L"Com_Transform"));

				XMStoreFloat4x4(&WorldMat, XMLoadFloat4x4(pModelTransform->Get_WorldFloat4x4()));

				ViewMat = GET_PIPELINE(CPipeLine::D3DTS_VIEW);
				ProjMat = GET_PIPELINE(CPipeLine::D3DTS_PROJ);

				XMStoreFloat3(&vPos, pModelTransform->Get_State(CTransform::STATE_POSITION));
				vScale = pModelTransform->Get_Scaled();

				ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &fRot.x, &vScale.x);
				ImGui::PushItemWidth(200.f);
				ImGui::SeparatorText("Position:");
				ImGui::InputFloat3("T", &vPos.x);

				ImGui::SeparatorText("Rotation:");
				ImGui::PushItemWidth(200.f);
				ImGui::InputFloat3("R", &fRot.x);

				ImGui::SeparatorText("Scale:");
				ImGui::PushItemWidth(200.f);
				ImGui::InputFloat3("S", &vScale.x);
				ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &fRot.x, &vScale.x, &WorldMat._11);

				ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11
					, m_tGuizmoDesc.CurrentGizmoOperation
					, m_tGuizmoDesc.CurrentGizmoMode
					, (_float*)&WorldMat
					, NULL
					, m_tGuizmoDesc.bUseSnap ? &m_tGuizmoDesc.snap[0] : NULL
					, m_tGuizmoDesc.boundSizing ? m_tGuizmoDesc.bounds : NULL
					, m_tGuizmoDesc.boundSizingSnap ? m_tGuizmoDesc.boundsSnap : NULL);

				pModelTransform->Set_WorldMat(WorldMat);
			}

			if (ImGui::Button("Add")) {
				if (find(vecSelectedModels.begin(), vecSelectedModels.end(), m_strFolderName) == vecSelectedModels.end()) {
					Create_Prototype(strConverted);
				}
				else {
					++m_ModelCounter[m_strFolderName];
					Create_Prototype(strConverted);
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Save File"))
				Save_ModelData();

			ImGui::SameLine();
			if (ImGui::Button("Load File"))
				Load_ModelData();

			ImGui::SeparatorText("List of Added Models");

			if (ImGui::BeginListBox("  "))
			{
				for (size_t i = 0; i < vecSelectedModels.size(); ++i) {
					const auto& modelFile = vecSelectedModels[i];
					const bool isSelected = (iCurIdxSelected == i);
					std::string strTemp(modelFile.begin(), modelFile.end());

					if (ImGui::Selectable(strTemp.c_str(), isSelected)) {
						iCurIdxSelected = (_int)i;
						m_strSelectedModel = modelFile;
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}

			if (ImGui::Button("Duplicate"))
				Duplicate_ModelData(iCurIdxSelected);

			ImGui::SameLine();
			if (ImGui::Button("Delete"))
				Delete_ModelData(iCurIdxSelected);

			WideCharToMultiByte(CP_ACP, 0, m_strSelectedModel.c_str(), lstrlen(m_strSelectedModel.c_str()), strConverted1, MAX_PATH, nullptr, nullptr);

			if (!m_strSelectedModel.empty())
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "----- Selected: [ %s ] ", strConverted1);


			ImGui::EndTabItem();

		}
		/* Navigation Mesh */
		if (ImGui::BeginTabItem("Navigation")) {
			ImGui::SeparatorText("List of Point Arrays");

			ImGuiIO& io = ImGui::GetIO(); (void)io;

			if (m_pTerrain == nullptr)
			{
				m_pTerrain = dynamic_cast<CTerrain*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_BackGround")->front());
				Safe_AddRef(m_pTerrain);
			}

			if (m_pNavigation == nullptr)
			{
				m_pNavigation = dynamic_cast<CNavigation*>(m_pTerrain->Find_Component(L"Com_Navigation"));
				Safe_AddRef(m_pNavigation);
			}

			if (m_vecNavigationPoints.empty())
				m_vecNavigationPoints = m_pNavigation->Get_InitialNavData();

			if (false == io.WantCaptureMouse && MOUSE_PUSH(DIMK_RBUTTON))
				Nav_Picking();

			if (ImGui::BeginListBox("    "))
			{
				int pointIndex = 0;
				for (const auto& vecPoint : m_vecNavigationPoints)
				{
					string pointLabel = "Point[" + to_string(pointIndex) + "]: { " +
						to_string(vecPoint.x) + ", " +
						to_string(vecPoint.y) + ", " +
						to_string(vecPoint.z) + " }";

					ImGui::Selectable(pointLabel.c_str());
					++pointIndex;
				}
				ImGui::EndListBox();
			}

			if (ImGui::Button("Save"))
				Save_NavigationData();

			ImGui::SameLine();
			if (ImGui::Button("Load"))
				Load_NavigationData();

			ImGui::EndTabItem();
		}

		/* Collectible Models */
		if (ImGui::BeginTabItem("Collectibles")) {
			ImGui::SeparatorText("Select Model");
			static int iCurIdx = 0;
			static int iCurIdxSelected = 0;
			if (ImGui::BeginListBox(" "))
			{
				for (size_t i = 0; i < vecCltbs.size(); ++i) {
					const auto& modelFile = vecCltbs[i];
					const bool isSelected = (iCurIdx == i);
					std::string strTemp(modelFile.begin(), modelFile.end());

					if (ImGui::Selectable(strTemp.c_str(), isSelected)) {
						iCurIdx = (int)i;
						m_strFolderCltb = modelFile;
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}

			WideCharToMultiByte(CP_ACP, 0, m_strFolderCltb.c_str(), lstrlen(m_strFolderCltb.c_str()), strConverted, MAX_PATH, nullptr, nullptr);

			if (nullptr != m_pCollectibles)
			{
				CTransform* pModelTransform = dynamic_cast<CTransform*>(m_pCollectibles->Find_Component(L"Com_Transform"));

				XMStoreFloat4x4(&WorldMat, XMLoadFloat4x4(pModelTransform->Get_WorldFloat4x4()));

				ViewMat = GET_PIPELINE(CPipeLine::D3DTS_VIEW);
				ProjMat = GET_PIPELINE(CPipeLine::D3DTS_PROJ);

				XMStoreFloat3(&vPos, pModelTransform->Get_State(CTransform::STATE_POSITION));
				vScale = pModelTransform->Get_Scaled();

				ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &fRot.x, &vScale.x);
				ImGui::PushItemWidth(200.f);
				ImGui::SeparatorText("Position:");
				ImGui::InputFloat3("T", &vPos.x);

				ImGui::SeparatorText("Rotation:");
				ImGui::PushItemWidth(200.f);
				ImGui::InputFloat3("R", &fRot.x);

				ImGui::SeparatorText("Scale:");
				ImGui::PushItemWidth(200.f);
				ImGui::InputFloat3("S", &vScale.x);
				ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &fRot.x, &vScale.x, &WorldMat._11);

				ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11
					, m_tGuizmoDesc.CurrentGizmoOperation
					, m_tGuizmoDesc.CurrentGizmoMode
					, (_float*)&WorldMat
					, NULL
					, m_tGuizmoDesc.bUseSnap ? &m_tGuizmoDesc.snap[0] : NULL
					, m_tGuizmoDesc.boundSizing ? m_tGuizmoDesc.bounds : NULL
					, m_tGuizmoDesc.boundSizingSnap ? m_tGuizmoDesc.boundsSnap : NULL);

				pModelTransform->Set_WorldMat(WorldMat);
			}

			if (ImGui::Button("Add")) {
				if (find(vecSelectedCltbs.begin(), vecSelectedCltbs.end(), m_strFolderCltb) == vecSelectedCltbs.end()) {
					Create_CollectiblePrototype(strConverted);
				}
				else {
					++m_ModelCounter[m_strFolderCltb];
					Create_CollectiblePrototype(strConverted);
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Save File"))
				Save_CollectibleData();

			ImGui::SameLine();
			if (ImGui::Button("Load File"))
				Load_CollectibleData();

			ImGui::SeparatorText("List of Added Models");

			if (ImGui::BeginListBox("  "))
			{
				for (size_t i = 0; i < vecSelectedCltbs.size(); ++i) {
					const auto& modelFile = vecSelectedCltbs[i];
					const bool isSelected = (iCurIdxSelected == i);
					std::string strTemp(modelFile.begin(), modelFile.end());

					if (ImGui::Selectable(strTemp.c_str(), isSelected)) {
						iCurIdxSelected = (_int)i;
						m_strSelectedCltbs = modelFile;
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete"))
				Delete_CollectibleData(iCurIdxSelected);

			WideCharToMultiByte(CP_ACP, 0, m_strSelectedCltbs.c_str(), lstrlen(m_strSelectedCltbs.c_str()), strConverted1, MAX_PATH, nullptr, nullptr);

			if (!m_strSelectedCltbs.empty())
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "----- Selected: [ %s ] ", strConverted1);

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();

	}
}

void CImGui_Manager::Save_ModelData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.hInstance = NULL;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = NULL;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrDefExt = L"dat";
	Ofn.lpstrInitialDir = L"../Bin/Resource/SaveFiles/SkyCastle/";
	Ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

	_int iRet = { 0 };

	iRet = GetSaveFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Save File");
		return;
	}

	_ulong dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Save");
		return;
	}

	auto pLayer = GET_LAYER(LEVEL_GAMEPLAY, L"Layer_ModelObjects");
	if (pLayer)
	{
		auto* pList = pLayer->Get_List();
		m_pSelectedModels.insert(m_pSelectedModels.end(), pList->begin(), pList->end());
	}

	size_t iModelCnt = m_pSelectedModels.size();
	WriteFile(hFile, &iModelCnt, sizeof size_t, &dwByte, nullptr);

	if (!m_pSelectedModels.empty())
	{
		for (auto& pObject : m_pSelectedModels)
		{
			if (pObject)
				dynamic_cast<CModelObjects*>(pObject)->Save_ModelData(hFile, dwByte);
		}
	}

	CloseHandle(hFile);

	return;
}

void CImGui_Manager::Load_ModelData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0fbx 파일\0*.fbx";

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = filter;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrInitialDir = L"../Bin/Resource/SaveFiles/SkyCastle/";

	_int iRet = { 0 };
	iRet = GetOpenFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Load File");
		return;
	}

	DWORD dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Load");
		return;
	}

	for (auto& iter : m_ModelDesc)
		Safe_Delete(iter.second);
	m_ModelDesc.clear();
	m_ModelCounter.clear();

	size_t iModelCnt = {};
	ReadFile(hFile, &iModelCnt, sizeof size_t, &dwByte, nullptr);

	while (iModelCnt)
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

		CModelObjects::MODEL_DESC* pModelDesc = new CModelObjects::MODEL_DESC;

		ReadFile(hFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);

		DWORD iLen1 = 0;
		DWORD iLen2 = 0;
		ReadFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
		ReadFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);

		ReadFile(hFile, pModelDesc->strName, iLen1, &dwByte, nullptr);
		ReadFile(hFile, pModelDesc->strNumbered, iLen2, &dwByte, nullptr);
		pModelDesc->strName[iLen1] = '\0';
		pModelDesc->strNumbered[iLen2] = '\0';

		pModelDesc->matModel = WorldMatrix;

		wstring modelName;
		CharToWString(pModelDesc->strNumbered, modelName);
		m_ModelDesc.emplace(modelName, pModelDesc);

		string modelNameStr = pModelDesc->strName;
		wstring modelNameWStr(modelNameStr.begin(), modelNameStr.end());
		m_ModelCounter[modelNameWStr]++;

		--iModelCnt;
	}

	CloseHandle(hFile);

	//m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, L"Layer_ModelObjects");

	for (auto& pModelDesc : m_ModelDesc)
	{
		_char szConvert[MAX_PATH] = { };

		_char szTmp[256];
		strcpy(szTmp, pModelDesc.second->strName);
		wstring strTemp;
		CharToWString(szTmp, strTemp);

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

		/* Clone */
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_ModelObjects", L"Prototype_GameObject_" + strTemp, pModelDesc.second)))
		{
			MSG_BOX(L"CImGui_Manager - Load Failed!");
			return;
		}
	}

	vecSelectedModels.clear();
	for (const auto& pModel : m_ModelDesc)
		vecSelectedModels.push_back(pModel.first);

	return;
}

void CImGui_Manager::Delete_ModelData(_uint iIdx) {
	if (iIdx >= vecSelectedModels.size())
		return;

	auto vecIter = vecSelectedModels.begin();
	std::advance(vecIter, iIdx);
	wstring modelName = *vecIter;

	vecSelectedModels.erase(vecIter);

	wstring strName = CutOffNumbers(modelName);

	auto counterIter = m_ModelCounter.find(strName);
	if (counterIter != m_ModelCounter.end()) {
		if (--counterIter->second <= 0) {
			m_ModelCounter.erase(counterIter);
		}
	}

	list<CGameObject*> ModelList = *(GET_LIST(LEVEL_GAMEPLAY, L"Layer_ModelObjects"));
	if (iIdx >= ModelList.size())
		return;

	auto listIter = ModelList.begin();
	std::advance(listIter, iIdx);
	(*listIter)->Set_Dead(true);

	Safe_Release(m_pModelObjects);
}

void CImGui_Manager::Duplicate_ModelData(_uint iIdx)
{
	if (iIdx >= vecSelectedModels.size())
		return;

	_char szConvert[MAX_PATH] = { };

	_char szTmp[256];
	wstring strTemp;
	CharToWString(szTmp, strTemp);

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

	CModelObjects::MODEL_DESC* pModelDesc = new CModelObjects::MODEL_DESC;

	/* Clone */
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_ModelObjects", L"Prototype_GameObject_" + strTemp, pModelDesc)))
	{
		MSG_BOX(L"CImGui_Manager - Load Failed!");
		return;
	}
}

void CImGui_Manager::Nav_Picking()
{
	_float3 vPoints = {};

	_vector vPickedPos = XMVectorSetW(XMLoadFloat4(&m_pGameInstance->Get_PickedPos()), 1.f);
	XMStoreFloat3(&vPoints, vPickedPos);

	vPoints = Snap(vPoints, m_vecNavigationPoints);

	m_vecPoints.push_back(vPoints);

	if (!m_vecPoints.empty() && m_vecPoints.size() % 3 == 0)
	{
		ClockWise(m_vecPoints);
		m_pNavigation->Tool_DrawNav(m_vecPoints);

		for (size_t i = 0; i < 3; i++)
			m_vecNavigationPoints.pop_back();

		for (size_t i = 0; i < 3; i++)
			m_vecNavigationPoints.push_back(m_vecPoints[i]);

		m_vecPoints.clear();
	}
}

_float3 CImGui_Manager::Snap(_float3& vPointsToCompare, vector<_float3>& vecPoints)
{
	// 내가 피킹한 위치가 이미 찍혀있는 네비게이션의 특정점의 근처라고 계산된다면 -> 그 점의 위치로 설정되도록
	// 거리 계산 -> 피킹 : { x, 0, z } 특정점 : { x, 0, z } 두 벡터 사이의 거리 계산 
	// 찍어놓은 점들 순회하면서 피킹한 점이랑 비교
	// 새로운 점과의 거리가 0.5보다 작으면 새로운 점을 그 비슷한 점의 값으로 채워준다
	_float fTemp = {};
	_bool bIsSnapped = { false };
	for (int i = 0; i < vecPoints.size(); i++)
	{
		fTemp = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPointsToCompare) - XMLoadFloat3(&vecPoints[i])));

		if (fTemp < 0.3f)
		{
			vPointsToCompare = vecPoints[i];
			vecPoints.push_back(vPointsToCompare);
			bIsSnapped = true;
			break;
		}
	}
	if (false == bIsSnapped)
		vecPoints.push_back(vPointsToCompare);

	return vPointsToCompare;
}

vector<_float3>& CImGui_Manager::ClockWise(vector<_float3>& vecPoints)
{
	if (vecPoints.size() != 3)
		return vecPoints;

	if (!IsClockwise(vecPoints[0], vecPoints[1], vecPoints[2]))
		std::swap(vecPoints[1], vecPoints[2]);

	return vecPoints;
}

bool CImGui_Manager::IsClockwise(const _float3& p1, const _float3& p2, const _float3& p3)
{
	_vector vecP1 = XMLoadFloat3(&p1);
	_vector vecP2 = XMLoadFloat3(&p2);
	_vector vecP3 = XMLoadFloat3(&p3);

	_vector vec1 = vecP2 - vecP1;
	_vector vec2 = vecP3 - vecP1;

	_vector vCross = XMVector3Cross(vec1, vec2);

	float fYComponent = XMVectorGetY(vCross);

	return fYComponent > 0;
}


HRESULT CImGui_Manager::Save_NavigationData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.hInstance = NULL;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = NULL;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrDefExt = L"dat";
	Ofn.lpstrInitialDir = L"../Bin/DataFiles/";
	Ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

	_int iRet = { 0 };

	iRet = GetSaveFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Save File");
		return E_FAIL;
	}

	_ulong dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Save");
		return E_FAIL;
	}

	_float3 vPoints[3] = {};

	for (int i = 0; i < m_vecNavigationPoints.size(); i++) {
		vPoints[i % 3] = m_vecNavigationPoints[i];

		if ((i + 1) % 3 == 0)
			WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
	}

	if (m_vecNavigationPoints.size() % 3 != 0)
		WriteFile(hFile, vPoints, sizeof(_float3) * (m_vecNavigationPoints.size() % 3), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CImGui_Manager::Load_NavigationData()
{
	m_pNavigation->Tool_LoadNav(L"../Bin/DataFiles/Navigation.dat");

	return S_OK;
}

void CImGui_Manager::Save_CollectibleData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.hInstance = NULL;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = NULL;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrDefExt = L"dat";
	Ofn.lpstrInitialDir = L"../Bin/Resource/SaveFiles/SkyCastle/";
	Ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

	_int iRet = { 0 };

	iRet = GetSaveFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Save File");
		return;
	}

	_ulong dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Save");
		return;
	}

	auto pLayer = GET_LAYER(LEVEL_GAMEPLAY, L"Layer_Collectibles");
	if (pLayer)
	{
		auto* pList = pLayer->Get_List();
		m_pSelectedCltbs.insert(m_pSelectedCltbs.end(), pList->begin(), pList->end());
	}

	size_t iModelCnt = m_pSelectedCltbs.size();
	WriteFile(hFile, &iModelCnt, sizeof size_t, &dwByte, nullptr);

	if (!m_pSelectedCltbs.empty())
	{
		for (auto& pObject : m_pSelectedCltbs)
		{
			if (pObject)
				dynamic_cast<CCollectible_Coin*>(pObject)->Save_ModelData(hFile, dwByte);
		}
	}

	CloseHandle(hFile);

	return;
}

void CImGui_Manager::Load_CollectibleData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0fbx 파일\0*.fbx";

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = filter;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrInitialDir = L"../Bin/Resource/SaveFiles/SkyCastle/";

	_int iRet = { 0 };
	iRet = GetOpenFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Load File");
		return;
	}

	DWORD dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Load");
		return;
	}

	for (auto& iter : m_CltbDesc)
		Safe_Delete(iter.second);
	m_CltbDesc.clear();
	m_ModelCounter.clear();

	size_t iModelCnt = {};
	ReadFile(hFile, &iModelCnt, sizeof size_t, &dwByte, nullptr);

	while (iModelCnt)
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

		CCollectible_Coin::CLTB_DESC* pModelDesc = new CCollectible_Coin::CLTB_DESC;

		ReadFile(hFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);

		DWORD iLen1 = 0;
		DWORD iLen2 = 0;
		ReadFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
		ReadFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);

		ReadFile(hFile, pModelDesc->strName, iLen1, &dwByte, nullptr);
		ReadFile(hFile, pModelDesc->strNumbered, iLen2, &dwByte, nullptr);
		pModelDesc->strName[iLen1] = '\0';
		pModelDesc->strNumbered[iLen2] = '\0';

		pModelDesc->matModel = WorldMatrix;

		wstring modelName;
		CharToWString(pModelDesc->strNumbered, modelName);
		m_CltbDesc.emplace(modelName, pModelDesc);

		string modelNameStr = pModelDesc->strName;
		wstring modelNameWStr(modelNameStr.begin(), modelNameStr.end());
		m_ModelCounter[modelNameWStr]++;

		--iModelCnt;
	}

	CloseHandle(hFile);

	//m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, L"Layer_ModelObjects");

	for (auto& pModelDesc : m_CltbDesc)
	{
		_char szConvert[MAX_PATH] = { };

		_char szTmp[256];
		strcpy(szTmp, pModelDesc.second->strName);
		wstring strTemp;
		CharToWString(szTmp, strTemp);

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

		/* Clone */
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_Collectibles", L"Prototype_GameObject_" + strTemp, pModelDesc.second)))
		{
			MSG_BOX(L"CImGui_Manager - Load Failed!");
			return;
		}
	}
}

void CImGui_Manager::Delete_CollectibleData(_uint iIdx)
{
	if (iIdx >= vecSelectedCltbs.size())
		return;

	auto vecIter = vecSelectedCltbs.begin();
	std::advance(vecIter, iIdx);
	wstring modelName = *vecIter;

	vecSelectedCltbs.erase(vecIter);

	wstring strName = CutOffNumbers(modelName);

	auto counterIter = m_ModelCounter.find(strName);
	if (counterIter != m_ModelCounter.end()) {
		if (--counterIter->second <= 0) {
			m_ModelCounter.erase(counterIter);
		}
	}

	list<CGameObject*> ModelList = *(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Collectibles"));
	if (iIdx >= ModelList.size())
		return;

	auto listIter = ModelList.begin();
	std::advance(listIter, iIdx);
	(*listIter)->Set_Dead(true);

	Safe_Release(m_pCollectibles);
}

void CImGui_Manager::Create_CollectiblePrototype(_char szConverted[MAX_PATH])
{
	if (m_ModelCounter.find(m_strFolderCltb) == m_ModelCounter.end()) {
		m_ModelCounter[m_strFolderCltb] = 0;
	}

	int& iNum = ++m_ModelCounter[m_strFolderCltb];

	/* Create Prototype Component */
	wstring strFullPath = L"C:/NSLT_3D/Client/Bin/Resources/Models/Collectibles/" + m_strFolderCltb + L"/Visuals.fbx";

	WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), lstrlenW(strFullPath.c_str()), szConverted, MAX_PATH, nullptr, nullptr);

	XMMATRIX LocalTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	wchar_t prototypeComponentName[256];
	swprintf(prototypeComponentName, 256, L"Prototype_Component_Model_%s%d", m_strFolderCltb.c_str(), iNum);

	HRESULT hr;
	hr = m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, prototypeComponentName,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szConverted, LocalTransformMatrix));

	/* Create Prototype GameObject */
	wstring strPrototypeGameObject = L"Prototype_GameObject_" + m_strFolderCltb + to_wstring(iNum);
	hr = m_pGameInstance->Add_Prototype(strPrototypeGameObject,
		CCollectible_Coin::Create(m_pDevice, m_pContext, prototypeComponentName));

	wstring strNumberedName;
	strNumberedName = m_strFolderCltb + to_wstring(iNum);
	_char szFileName[256] = { 0 };
	_char szFileNumbered[256] = { 0 };

	CCollectible_Coin::CLTB_DESC* CltbDesc = new CCollectible_Coin::CLTB_DESC;
	WideCharToMultiByte(CP_ACP, 0, m_strFolderCltb.c_str(), lstrlenW(m_strFolderCltb.c_str()), szFileName, MAX_PATH, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, strNumberedName.c_str(), lstrlenW(strNumberedName.c_str()), szFileNumbered, MAX_PATH, nullptr, nullptr);

	XMFLOAT4X4 WorldMat;
	XMStoreFloat4x4(&WorldMat, XMMatrixTranslation(0.5f, 0.5f, 0.5f) * XMMatrixIdentity());
	CltbDesc->matModel = WorldMat;
	strcpy(CltbDesc->strName, szFileName);
	strcpy(CltbDesc->strNumbered, szFileNumbered);

	wstring strLayer = L"Layer_Collectibles";
	if (!m_strFolderCltb.empty())
		hr = m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayer, strPrototypeGameObject.c_str(), CltbDesc);

	m_pCollectibles = dynamic_cast<CCollectible_Coin*>(GET_LIST(LEVEL_GAMEPLAY, strLayer)->back());
	Add_SelectedCltbs(strNumberedName);
	m_CltbDesc.emplace(strNumberedName, CltbDesc);
}

void CImGui_Manager::Create_Prototype(_char szConverted[MAX_PATH])
{
	if (m_ModelCounter.find(m_strFolderName) == m_ModelCounter.end()) {
		m_ModelCounter[m_strFolderName] = 0;
	}

	int& iNum = ++m_ModelCounter[m_strFolderName];

	/* Create Prototype Component */
	wstring strFullPath = L"C:/NSLT_3D/Client/Bin/Resources/Models/MapObjects/" + m_strFolderName + L"/" + m_FileName;

	WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), lstrlenW(strFullPath.c_str()), szConverted, MAX_PATH, nullptr, nullptr);

	XMMATRIX LocalTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	wchar_t prototypeComponentName[256];
	swprintf(prototypeComponentName, 256, L"Prototype_Component_Model_%s%d", m_strFolderName.c_str(), iNum);

	m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, prototypeComponentName,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szConverted, LocalTransformMatrix));

	/* Create Prototype GameObject */
	wstring strPrototypeGameObject = L"Prototype_GameObject_" + m_strFolderName + to_wstring(iNum);
	m_pGameInstance->Add_Prototype(strPrototypeGameObject,
		CModelObjects::Create(m_pDevice, m_pContext, prototypeComponentName));

	wstring strNumberedName;
	strNumberedName = m_strFolderName + to_wstring(iNum);
	_char szFileName[256] = { 0 };
	_char szFileNumbered[256] = { 0 };

	CModelObjects::MODEL_DESC* ModelDesc = new CModelObjects::MODEL_DESC;
	WideCharToMultiByte(CP_ACP, 0, m_strFolderName.c_str(), lstrlenW(m_strFolderName.c_str()), szFileName, MAX_PATH, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, strNumberedName.c_str(), lstrlenW(strNumberedName.c_str()), szFileNumbered, MAX_PATH, nullptr, nullptr);

	XMFLOAT4X4 WorldMat;
	XMStoreFloat4x4(&WorldMat, XMMatrixTranslation(0.5f, 0.5f, 0.5f) * XMMatrixIdentity());
	ModelDesc->matModel = WorldMat;
	strcpy(ModelDesc->strName, szFileName);
	strcpy(ModelDesc->strNumbered, szFileNumbered);

	wstring strLayer = L"Layer_ModelObjects";
	if (!m_strFolderName.empty())
		m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayer, strPrototypeGameObject.c_str(), ModelDesc);

	m_pModelObjects = dynamic_cast<CModelObjects*>(GET_LIST(LEVEL_GAMEPLAY, strLayer)->back());
	Add_SelectedModels(strNumberedName);
	m_ModelDesc.emplace(strNumberedName, ModelDesc);
}

wstring CImGui_Manager::CutOffNumbers(const wstring& modelName)
{
	size_t pos = modelName.find_last_not_of(L"0123456789");
	if (pos != wstring::npos) {
		return modelName.substr(0, pos + 1);
	}
	return modelName;
}

wstring CImGui_Manager::GenerateNextFolderName(const wstring& baseName, const vector<wstring>& selectedModels)
{
	int counter = 1;
	wstring newFolderName;

	while (true) {
		newFolderName = baseName + to_wstring(counter);
		if (find(selectedModels.begin(), selectedModels.end(), newFolderName) == selectedModels.end()) {
			break;
		}
		counter++;
	}

	return newFolderName;
}

void CImGui_Manager::Add_SelectedModels(const wstring& strModel)
{
	vecSelectedModels.push_back(strModel);
}

void CImGui_Manager::Add_SelectedCltbs(const wstring& strModel)
{
	vecSelectedCltbs.push_back(strModel);
}

vector<wstring> CImGui_Manager::Find_FBXModel(const wstring& strFilePath)
{
	vector<wstring> vecFileName;
	wstring m_strFile, m_strFolder;
	wstring strPath = strFilePath + L"\\*.*";
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(strPath.c_str(), &findFileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			m_FileName = findFileData.cFileName;
			if (m_FileName != L"." && m_FileName != L"..") {
				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					m_strFile = strFilePath + L"\\" + m_FileName;

					vector<wstring> vecSubFiles = Find_FBXModel(m_strFile);
					vecFileName.insert(vecFileName.end(), vecSubFiles.begin(), vecSubFiles.end());
				}
				else if (m_FileName.substr(m_FileName.find_last_of(L".") + 1) == L"fbx") {
					m_strFolder = strFilePath.substr(strFilePath.find_last_of(L"\\") + 1);
					vecFileName.push_back(m_strFolder);
					break;
				}
			}
		} while (FindNextFileW(hFind, &findFileData) != 0);

		FindClose(hFind);
	}
	return vecFileName;
}

vector<wstring> CImGui_Manager::Find_CLTBModel(const wstring& strFilePath)
{
	vector<wstring> vecFileName;
	wstring m_strFile, m_strFolder;
	wstring strPath = strFilePath + L"\\*.*";
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(strPath.c_str(), &findFileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			m_CltbFileName = findFileData.cFileName;
			if (m_CltbFileName != L"." && m_CltbFileName != L"..") {
				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					m_strFile = strFilePath + L"\\" + m_CltbFileName;

					vector<wstring> vecSubFiles = Find_FBXModel(m_strFile);
					vecFileName.insert(vecFileName.end(), vecSubFiles.begin(), vecSubFiles.end());
				}
				else if (m_CltbFileName.substr(m_CltbFileName.find_last_of(L".") + 1) == L"fbx") {
					m_strFolder = strFilePath.substr(strFilePath.find_last_of(L"\\") + 1);
					vecFileName.push_back(m_strFolder);
					break;
				}
			}
		} while (FindNextFileW(hFind, &findFileData) != 0);

		FindClose(hFind);
	}
	return vecFileName;
}


void CImGui_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_ModelDesc)
		Safe_Delete(Pair.second);
	m_ModelDesc.clear();

	for (auto& Pair : m_CltbDesc)
		Safe_Delete(Pair.second);
	m_CltbDesc.clear();

	for (auto& SelectedModels : m_pSelectedModels)
		Safe_Release(SelectedModels);
	m_pSelectedModels.clear();

	for (auto& SelectedModels : m_pSelectedCltbs)
		Safe_Release(SelectedModels);
	m_pSelectedCltbs.clear();

	if (m_pDebugCam)
		Safe_Release(m_pDebugCam);

	if (m_pDebugCamTransform)
		Safe_Release(m_pDebugCamTransform);

	if (m_pPlayerCam)
		Safe_Release(m_pPlayerCam);

	if (m_pPlayerCamTransform)
		Safe_Release(m_pPlayerCamTransform);

	Safe_Release(m_pModelObjects);
	Safe_Release(m_pCollectibles);
	Safe_Release(m_pTerrain);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}