#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMazeDoor final : public CGameObject
{
private:
	CMazeDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMazeDoor(const CMazeDoor& rhs);
	virtual ~CMazeDoor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
private:
	_bool	Near_Player();

	_vector vInitialPosition = { 43.6f, 4.f, 50.36f, 1.f };
	_bool	m_bIsOpening = false;
	_float	m_fElapsedTime = 0.0f;
	_float	m_fCurrentAngle = 0.0f;
	const _float m_fRotationSpeed = 1.0f;

public:
	static CMazeDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END