#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CPartObject;
class CModel;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	using PLAYER_DESC = struct tagPlayerDesc
	{
		LEVEL m_eLevel;
	};

public:
	enum PARTOBJ { PART_BODY, PART_BURROW, PART_EFFECT, PART_END };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	_vector		Get_Player_Position() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	CTransform* Get_Player_Transform() { return m_pTransformCom; }
	void		Set_PlayerJumpPower(_float fJumpPower) { m_fJumpPower = fJumpPower; }
	void		Player_Jump(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint   Get_PlayerState() { return m_iState; }

private:
	void	Player_KeyInput(_float fTimeDelta);
	void	Player_KeyState(const unsigned int iState);

	void	Sound_RandomJump();
	void	Sound_RandomAtt();

private:
	CNavigation*			m_pNavigationCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CCollider*				m_pTailColliderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	class CBody_Player*		m_pBodyPlayer = { nullptr };

private:
	vector<CPartObject*>	m_Parts;
	_uint					m_iState = { IDLE };
	LEVEL					m_eLevel = { LEVEL_END };

	_bool					m_bBeforeJump = false;
	_bool					m_bJump = false;
	_bool					m_bLanding = false;
	_float					m_fJumpPower = 1.8f;
	_float					m_fGravity = 9.8f;
	_float					m_fTime = 0.f;

	_float3					m_vPlayerPos = {};

	PLAYER_DESC				m_PlayerDesc;

	_bool					m_bSound = { false };
	_bool					m_bSoundAtt = { false };

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
