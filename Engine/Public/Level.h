#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	_uint Get_LevelID() const {	return m_iLevelID; }
	virtual _uint Get_LevelID_For_Init() const { return m_iLevelID;  }

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	_uint						m_iLevelID = { 0 };

public:
	virtual void Free() override;
};

END
