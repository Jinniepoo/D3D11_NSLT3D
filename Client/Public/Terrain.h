#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CNavigation;
class CVIBuffer_Terrain;
END


BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	using TEXTURE = enum tagTextureType { TEXTURE_DIFFUSE, TEXTURE_MASK, TEXTURE_BRUSH, TEXTURE_END };

	using TERRAIN_DESC = struct tagTerrainDesc
	{
		LEVEL eLevel = { LEVEL_END };
	};

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*			 m_pShaderCom = { nullptr };
	CTexture*			 m_pTextureCom[TEXTURE_END] = { nullptr };
	CVIBuffer_Terrain*	 m_pVIBufferCom = { nullptr };
	CNavigation*		 m_pNavigationCom = { nullptr };

public:
	_float3		Terrain_Picking();
	_bool		Terrain_IsPicked() { return m_bTerrainPicking; }

	_bool		m_bTerrainPicking = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	LEVEL	m_eLevel = { LEVEL_END };

	TERRAIN_DESC m_TerrainDesc;

private:
	//_float3						m_vPickingPos = {};

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END