#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	class CComponent* Find_Component(const wstring& strComponentTag, _uint iIndex);

private:
	list<class CGameObject*>			m_GameObjects;

public:
	list <class CGameObject*>* Get_List() { return &m_GameObjects; }

public:
	static CLayer* Create();
	virtual void Free() override;
};

END
