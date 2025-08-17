#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster_CrumbleR final : public CGameObject
{
public:
	using MONSTER_DESC = struct tagMonsterDesc
	{
		_vector vMonsterPos = {};
		_float  fRotation = {};
		_int    iCellIndex = {};
		_int	iInitialAnim = {};
		LEVEL	m_eLevel = { LEVEL_END };
	};

private:
	CMonster_CrumbleR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_CrumbleR(const CMonster_CrumbleR& rhs);
	virtual ~CMonster_CrumbleR() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };

	class CPlayer*	m_pPlayer = { nullptr };
	class CUI_Heart_Filling* m_pUIHeart = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool	Near_Player();
	void	Chase_Player(_float fTimeDelta);
	void	Move_Randomly(_float fTimeDelta);
	HRESULT	Drop_Coins(_vector vMonsterPos);
	HRESULT	Startle_Effect(_vector vMonsterPos);
	HRESULT	Dizzy_Effect(_vector vMonsterPos);

private:
	_bool					m_bInitialAtt = { false };
	_bool					m_bIsAttacked = { false };
	_bool					m_bIsAttacked2 = { false };

	_bool					m_bChasePlayer = { false };

private:
	_vector		 m_vVelocity;      // Velocity vector
	_float		 m_fGravity;        // Gravity acceleration
	_bool		 m_bIsAirborne;
	_float		 m_fAttackTime;
	MONSTER_DESC m_MonsterDesc;
	LEVEL		 m_eLevel = { LEVEL_END };

	_bool		m_bStartle = { true };
	_bool		m_bChase = { true };
	_bool		m_bAirborneSound = { true };

	_bool		m_bPlayerDamaged = { false };

public:
	static CMonster_CrumbleR* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END