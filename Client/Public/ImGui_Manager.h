#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "ModelObjects.h"
#include "ModelObjects_Boss.h"
#include "Collectible_Coin.h"
#include "Collectibles_Boss.h"
#include "Terrain.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CTransform;
class CGameInstance;
class CGameObject;
class CShader;
class CModel;
END

BEGIN(Client)

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager);

	using GUIZMODESC = struct tagGuizmoDesc
	{
		ImGuizmo::MODE CurrentGizmoMode = ImGuizmo::WORLD;
		ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;
		bool  bUseSnap = false;
		bool  boundSizing = false;
		bool  boundSizingSnap = false;
		float snap[3] = { 0.1f, 0.1f, 0.1f };
		float bounds[6] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
		float boundsSnap[3] = { 0.1f, 0.1f, 0.1f };
	};

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT		Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
	void		Tick();
	HRESULT		Render();

public:
	void		Add_SelectedModels(const wstring& strModel);
	void		Add_SelectedCltbs(const wstring& strModel);
	void		ImGui_MapTool();

private:
	/* Models */
	void		Save_ModelData();
	void		Load_ModelData();
	void		Delete_ModelData(_uint iIdx);
	void		Duplicate_ModelData(_uint iIdx);
	void		Create_Prototype(_char szConverted[MAX_PATH]);

	wstring		CutOffNumbers(const wstring& strModelName);
	wstring		GenerateNextFolderName(const wstring& baseName, const vector<wstring>& selectedModels);

	/* Navigation */
	void				Nav_Picking();
	_float3				Snap(_float3& vPointsToCompare, vector<_float3>& vecPoints);
	vector<_float3>&	ClockWise(vector<_float3>& vecPoints);
	bool				IsClockwise(const _float3& p1, const _float3& p2, const _float3& p3);
	HRESULT				Save_NavigationData();
	HRESULT				Load_NavigationData();

	/* Collectibles */
	void				Save_CollectibleData();
	void				Load_CollectibleData();
	void				Delete_CollectibleData(_uint iIdx);
	void				Create_CollectiblePrototype(_char szConverted[MAX_PATH]);

	/* Tool */
	void				Move_Mouse(_bool bMove);
	void				Change_Camera(_bool bChange);
	
	/* º¯È¯ */
	wstring				CharToWString(_char* szSrc, wstring& strDest);
	_float3				XMQuaternionToEulerAngles(FXMVECTOR q);

private:
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CGameInstance*		m_pGameInstance = { nullptr };
	CGameObject*		m_pDebugCam = { nullptr };
	CGameObject*		m_pPlayerCam = { nullptr };
	CTransform*			m_pDebugCamTransform = { nullptr };
	CTransform*			m_pPlayerCamTransform = { nullptr };
	class CPlayer*		m_pPlayer = { nullptr };

	GUIZMODESC			m_tGuizmoDesc = {};
	LEVEL				m_eLevel = { LEVEL_END };

private:
	/* Models */
	CModelObjects*		m_pModelObjects = { nullptr };
	list<CGameObject*>	m_pSelectedModels;
	map<wstring, CModelObjects::MODEL_DESC*> m_ModelDesc;
	
	vector<wstring> vecModels;
	vector<wstring> vecSelectedModels;
	vector<wstring> Find_FBXModel(const wstring& strFilePath);

	/* Collectibles */
	CCollectible_Coin*	m_pCollectibles = { nullptr };
	list<CGameObject*>	m_pSelectedCltbs;
	map<wstring, CCollectible_Coin::CLTB_DESC*> m_CltbDesc;

	vector<wstring>		vecCltbs;
	vector<wstring>		vecSelectedCltbs;
	vector<wstring>		Find_CLTBModel(const wstring& strFilePath);

public:
	static map<wstring, int>	m_ModelCounter;

private:
	_tchar				m_szPlayerState[MAX_PATH] = {};

private:
	/* Navigation */
	vector<_float3>		m_vecPoints;
	vector<_float3>		m_vecNavigationPoints;
	CTerrain*			m_pTerrain = { nullptr };
	CNavigation*		m_pNavigation = { nullptr };

private:
	/* Tool */
	_vector			m_vDebugCamPos = { 0.f, 0.f, 0.f };
	_bool			m_bShowGui = { false };
	_bool			m_bChangeCamera = { false };
	_bool			m_bChangeCam = { false };
	_bool			m_bMoveCamera = { false };
	_bool			m_bMouseMove = { false };
	_bool			m_bUpdateSelected = { false };
	_float4x4		m_WorldMat = _float4x4{};

public:
	/* Find */
	wstring			m_FileName;

	wstring			m_CltbFileName;
	/* Save */
	wstring			m_strFolderName;
	wstring			m_strFilePath;
	wstring			m_strFileName;

	wstring			m_strFolderCltb;
	wstring			m_strPathCltb;
	wstring			m_strFileNameCltb;

	/* Delete */
	wstring			m_strSelectedModel;
	wstring			m_strSelectedCltbs;

	HANDLE          m_hModelFile;

public:
	virtual void Free() override;

};

END