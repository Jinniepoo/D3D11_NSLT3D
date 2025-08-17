#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "ImGui_Manager.h"

BEGIN(Client)

class CLevel_Puzzle final : public CLevel
{
private:
	CLevel_Puzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Puzzle() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CImGui_Manager*	   m_pTool = { nullptr };
	class CUI_Coins*		   m_pCoin = { nullptr };
	class CUI_Lives*		   m_pLives = { nullptr };
	class CUI_Clover_Question* m_pQuestion = { nullptr };

private:
	HRESULT Ready_Lights(); 

	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const wstring& strLayerTag);

	HRESULT Ready_LandObjects();

	HRESULT	Ready_MapObjects(const wstring& strLayerTag);
	HRESULT	Ready_Animated(const wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_NPCs(const wstring& strLayerTag);

	HRESULT Ready_Layer_Models(const wstring& strLayerTag);
	HRESULT Ready_Layer_Collectibles(const wstring& strLayerTag);

private:
	/* Player */
	class	CPlayer* m_pPlayer = { nullptr };
	void	Render_PlayerState();

	_bool	m_bDebug = { false };
	_bool   m_bShow = { true };

private:
	wstring CharToWString(_char* szSrc, wstring& strDest);

private:
	HANDLE  m_hModelFile;
	map<wstring,CModelObjects_Boss::MODEL_DESC*> m_ModelDesc;
	map<wstring, CCollectibles_Boss::CLTB_DESC*> m_CltbDesc;

	_tchar	m_szPlayerState[MAX_PATH] = {};
	_float  m_fElapsedTime = {};

public:
	static CLevel_Puzzle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
