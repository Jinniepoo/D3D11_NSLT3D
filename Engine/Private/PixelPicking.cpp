#include "..\Public\PixelPicking.h"
#include "GameInstance.h"



CPixelPicking::CPixelPicking(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPixelPicking::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;
	
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	D3D11_TEXTURE2D_DESC		TextureDesc{};

	TextureDesc.Width = (_uint)ViewportDesc.Width;
	TextureDesc.Height = (_uint)ViewportDesc.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_STAGING;
	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pPickedDepthTexture)))
		return E_FAIL;

	m_fViewWidth = ViewportDesc.Width;
	m_fViewHeight = ViewportDesc.Height;

	return S_OK;
}

void CPixelPicking::Update()
{	
	if (FAILED(m_pGameInstance->Copy_RT_Resource(TEXT("Target_PickedDepth"), m_pPickedDepthTexture)))
		return;

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse);

	if (ptMouse.x < 0)
		ptMouse.x = 0;
	if (ptMouse.x >= (_long)m_fViewWidth)
		ptMouse.x = (_long)m_fViewWidth - 1;

	if (ptMouse.y < 0)
		ptMouse.y = 0;
	if (ptMouse.y >= (_long)m_fViewHeight)
		ptMouse.y = (_long)m_fViewHeight - 1;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pPickedDepthTexture, 0, D3D11_MAP_READ, 0, &SubResource);

	_uint		iIndex = _uint(ptMouse.y * m_fViewWidth + ptMouse.x);

	_float4		vDepth = ((_float4*)SubResource.pData)[iIndex];

	m_pContext->Unmap(m_pPickedDepthTexture, 0);

	if (1.f == vDepth.w)
		return;
	
	m_vPickPos.x = ptMouse.x / (m_fViewWidth * 0.5f) - 1.f;
	m_vPickPos.y = ptMouse.y / (m_fViewHeight * -0.5f) + 1.f;
	m_vPickPos.z = vDepth.x;
	m_vPickPos.w = 1.f;	

	_matrix		ProjMatrixInv = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);
	_vector		vPickPos = XMVector3TransformCoord(XMLoadFloat4(&m_vPickPos), ProjMatrixInv);

	_matrix		ViewMatrixInv = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	vPickPos = XMVector3TransformCoord(vPickPos, ViewMatrixInv);
	
	m_bPixelPicked = true;

	XMStoreFloat4(&m_vPickPos, vPickPos);
}

CPixelPicking* CPixelPicking::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, HWND hWnd)
{
	CPixelPicking*	pInstance = new CPixelPicking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CPixelPicking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CPixelPicking::Free()
{
	__super::Free();

	Safe_Release(m_pPickedDepthTexture);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

}
