#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)
class CCollectibles_Boss : public CGameObject
{
public:
	using CLTB_STATE = enum tagCltbState
	{
		CLTB_DROP, CLTB_LOOT, CLTB_END
	};

	using CLTB_TYPE = enum tagCltbType
	{
		TYPE_COIN, TYPE_CLOVER, TYPE_HEART, TYPE_KEY, TYPE_LIFE, 
		TYPE_BURROWCOIN, TYPE_BURROWDIRT,
		TYPE_L, TYPE_U, TYPE_C, TYPE_K, TYPE_Y, TYPE_END
	};

	using CLTB_DESC = struct tagCltbDesc
	{
		_float4x4	matModel = {};
		_char		strName[256] = {};
		_char		strNumbered[256] = {};
		CLTB_STATE  m_CltbState = { CLTB_END };
		CLTB_TYPE   m_CltbType = { TYPE_END };
		LEVEL		m_eLevel = { LEVEL_END };
	};

private:
	CCollectibles_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollectibles_Boss(const CCollectibles_Boss& rhs);
	virtual ~CCollectibles_Boss() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* strComponentTag);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	//virtual HRESULT Render_LightDepth() override;

	void	Save_ModelData(HANDLE hFile, _ulong& dwByte); 
	void	LootEffect();
	void	DropEffect();

	void	UIBoardUpdate();

	/* Convert */
	wstring CCollectibles_Boss::CharToWString(_char* szSrc, wstring& strDest);

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	class CPlayer*  m_pPlayer = { nullptr };

	class CUI_Clover_Check*		m_pClover = { nullptr };
	class CUI_Clover_L*			m_pLucky = { nullptr };
	class CUI_Clover_Question*	m_pQuestion = { nullptr };
	class CUI_Coins*			m_pUICoin = { nullptr };
	class CUI_Heart_Filling*	m_pHeart = { nullptr };
	class CUI_Lives*			m_pLives = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	CLTB_DESC	m_CltbDesc;
	CLTB_STATE  m_CltbState;

	_tchar		m_strComponentTag[MAX_PATH] = {L""};
	_float		m_fCoinRot = {};
	_float		m_fTimer = { 0.f };
	_float		m_fTotalBounceTime = { 0.f };
	_float		m_fDropTime = { 0.f };
	_bool		m_bLoot = { false };
	_bool		m_bLootTimer = { true };
	_float		m_fInitialYPos = { 0.f };
	_float      m_fInitialX = { 0.f };
	_float		m_fInitialY = { 0.f };
	_float		m_fInitialVelocityY = { 0.f };
	_float      m_fInitialVelocityX = { 0.f };
	_bool		m_bDropEffect = { true };
	_bool       m_bLootCoin = { true };

public:
	static CCollectibles_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* strComponentTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
