#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	CGameInstance*				m_pGameInstance = { nullptr };
	class CImGui_Manager*		m_pTool = { nullptr };

private:
#ifdef _DEBUG
	_tchar						m_szFPS[MAX_PATH] = {};
	_float						m_fTimeAcc = { 0.f };
	_uint						m_iNumRender = { 0 };
#endif
	_bool						m_bDebug = { false };
	LEVEL						m_eLevel = { LEVEL_END };

private:
	HRESULT Open_Level(LEVEL eLevelID);
	HRESULT Ready_Prototype_GameObjects();
	HRESULT Ready_Prototoype_Component();
	//HRESULT	Ready_Gara();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END
