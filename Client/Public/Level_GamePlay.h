#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "ImGui_Manager.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CImGui_Manager*		m_pTool = { nullptr };
	class CUI_Coins*			m_pCoin = { nullptr };
	class CAnim_Foxhole*		m_pHole = { nullptr };
	class CUI_Lives*			m_pLives = { nullptr };

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

private:
	wstring CharToWString(_char* szSrc, wstring& strDest);

private:
	HANDLE  m_hModelFile;
	map<wstring,CModelObjects::MODEL_DESC*> m_ModelDesc;
	map<wstring, CCollectible_Coin::CLTB_DESC*> m_CltbDesc;

	_tchar	m_szPlayerState[MAX_PATH] = {};

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
