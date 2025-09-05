#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct
	{
		_int		iCurrentCellIndex;
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _wstring& strNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

public:
	void	Update(_fmatrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	_bool	isMove(_fvector vPosition);

	/* ImGui Tool */
	vector<_float3>	Get_InitialNavData() { return m_vecNavPoints; }

	HRESULT	Tool_DrawNav(vector<_float3>& vecPoints);
	HRESULT Tool_LoadNav(const _wstring& strNavigationDataFile);

	void	Set_PlayerPos(_float3& vPlayerPos) { m_vPlayerPos = vPlayerPos; }
	_float	Get_CellHeight();
	_int	Get_CurrentCellIndex() { return m_iCurrentIndex; }

private:
	/* Cell */
	vector<class CCell*>		m_Cells;
	_int						m_iCurrentIndex = { -1 };
	static _float4x4			m_WorldMatrix;

private:
	vector<_float3>				m_vecNavPoints = {};

	_float3						m_vPlayerPos = { };
#ifdef _DEBUG

private:
	class CShader* m_pShader = { nullptr };

#endif

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END 
