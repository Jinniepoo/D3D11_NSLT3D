#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBody_Player final : public Engine::CPartObject
{
public:
	enum PLAYER_STATE {PLAYER_IDLE, PLAYER_WALK, PLAYER_RUN, PLAYER_JUMP, 
					   PLAYER_LEFT, PLAYER_RIGHT, 
					   PLAYER_END };

	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_uint*		pState;
		LEVEL		eLevel;
	}BODY_DESC;

private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& rhs);
	virtual ~CBody_Player() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	PriorityTick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	_uint*					m_pState = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	LEVEL	m_eLevel = { LEVEL_END };

public:
	void Set_Animation(const unsigned int pState, _uint pIndex, _bool bAnim);

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END