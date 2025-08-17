#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag);

	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool isClear = true);
	HRESULT End_MRT();

	HRESULT Bind_RT_SRV(const _wstring& strRenderTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_Resource(const _wstring& strRenderTargetTag, ID3D11Texture2D* pResource);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif


private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	ID3D11RenderTargetView*			m_pOldRTV = { nullptr };	
	ID3D11DepthStencilView*			m_pDSV = { nullptr };

private:
	map<const _wstring, class CRenderTarget*>			m_RenderTargets;

private:
	map<const _wstring, list<class CRenderTarget*>>		m_MRTs;

private:
	class CRenderTarget* Find_RenderTarget(const _wstring& strRenderTargetTag);
	list<class CRenderTarget*>* Find_MRT(const _wstring& strMRTTag);
public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
