#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CLoading_BG final : public CGameObject
{
public:
	typedef enum ObjType
	{
		MAIN_BG, MAIN_LOGO, MAIN_TEXT, MAIN_BOOK,
		LOADING_BG, LOADING_NSLT, LOADING_GEAR, OBJ_END
	}OBJTYPE;

	using BG_DESC = struct tagBackGroundDesc
	{
		_uint	iSizeX;
		_uint	iSizeY;
		_float	fX;
		_float	fY;
		OBJTYPE eObjType;
	};
private:
	CLoading_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoading_BG(const CLoading_BG& rhs);
	virtual ~CLoading_BG() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };		
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };

private:
	BG_DESC						m_BackDesc;

private:
	_float						m_fX = {};
	_float						m_fY = {};
	_float						m_fSizeX = {};
	_float						m_fSizeY = {};
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

private:
	HRESULT Ready_Components();

public:
	static CLoading_BG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END