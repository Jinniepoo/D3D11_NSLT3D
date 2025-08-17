#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Main final : public CLevel
{
private:
	CLevel_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Main() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_NSLT(const wstring& strLayerTag);

public:
	static CLevel_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END