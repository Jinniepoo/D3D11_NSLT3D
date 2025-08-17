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

	/* 1. ���̴����� ��ŷ�� �ʿ��� �ȼ����� ���̸� ����Ÿ�ٿ� �������ش�. */
	/* 2. ����Ÿ���ؽ��ķκ��� ���̸� �����ͼ� ������ġ�� ����Ѵ�. ��, ����Ÿ���ؽ��Ŀ������� ���� ����� ���� ������ �� ����.(D3D11_USAGE_DEFAULT)  */
	/* 3. ���� ����Ÿ���� �ٸ� �ؽ��Ŀ� �����ϰ� ������� �ؽ��ĸ� ������ؼ� ���� �����;���. */

	/* 3���� �ش��ϴ� ������� �ؽ��ĸ� �̸� �����Ͽ� �غ��Ѵ�. */
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
	/* ��ŷ�� ���̸� ����س��� ����Ÿ���� ���� �����ִ� �ؽ��Ŀ� �������ش�. */
	/* �ؽ��ĸ� ��� ���̸� ������ �۾��� ���������Ŵ�. */
	if (FAILED(m_pGameInstance->Copy_RT_Resource(TEXT("Target_PickedDepth"), m_pPickedDepthTexture)))
		return;

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);

	/* ����Ʈ ���� ���콺 ��ġ�� ���ߴ�. */
	/* 1. m_pPickedDepthTexture�ؽ��ĸ� ��� ��ŷ�� �ȼ��� ���̸� ������ �۾��� �Ҳ���. */
	/* 2. ���ȼ��� �? (1280 * 720)�� ��ŭ �ִ�.== ������ ����. ���콺Ŀ���� �ִ� ��ġ�� �ȼ� �����ϳ��� ������ʹ�. */
	/* 3. . ���콺Ŀ���� �ִ� ��ġ�� �ȼ� �����ϳ��� ������ʹ�. == 
	_uint		iIndex = ptMouse.y * m_fViewWidth + ptMouse.x;*/
	/* 3. ���� ������� �����趲�ſ�? api��, ���콺 ��ġ�� �ش��ϴ� Ÿ���� �ε��� ���ϴ� ����. */


	/* 1. ���� ���� ����Ʈ ���� ���콺��ġ�� ���� ���彺���̽� ���� ���콺 ��ġ�� ���ϱ����� ������ Ȱ���̵ȴ�. */
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

	/* ��ŷ�� ���̸� �����س��� �ױ��ĸ� ����! */
	m_pContext->Map(m_pPickedDepthTexture, 0, D3D11_MAP_READ, 0, &SubResource);

	_uint		iIndex = _uint(ptMouse.y * m_fViewWidth + ptMouse.x);

	/* ���콺 �ִ� ��ġ�� �ȼ� ������ ������. */
	/* Out.vPickedDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f); */

	_float4		vDepth = ((_float4*)SubResource.pData)[iIndex];

	m_pContext->Unmap(m_pPickedDepthTexture, 0);

	if (1.f == vDepth.w)
		return;

	/* ptMouse.x, ptMouse.y => ����Ʈ ���� ��ġ��. ������� ������ ������ �س������Ѵ�. */

	/* ����Ʈ���� ��ġ(0, 0) ~ (wsx, wsy) -> ���������̽� ���� ��ġ(-1, 1) ~ (1, -1)�� �����ϳ�. */	
	m_vPickPos.x = ptMouse.x / (m_fViewWidth * 0.5f) - 1.f;
	m_vPickPos.y = ptMouse.y / (m_fViewHeight * -0.5f) + 1.f;
	m_vPickPos.z = vDepth.x;
	m_vPickPos.w = 1.f;	

	/* ������������ ��ġ -> �佺���̽� ���� ��ġ�� �����ϳ�. */
	/* ����������� �ٷ� ���� ����? -> ���̴����� �ߴٶ�� �ٷ� ����������� ���ϸ� �ȵǾ���. ������ w�� ���ؼ� ���������� �������ִ� �۾��� �ʿ��߾���. */
	/* ���̴����ƴ϶� Ŭ��. TransformCoord�Լ��� �˾Ƽ� w���ꤷ�� ���� ó���� ���ش�. */
	_matrix		ProjMatrixInv = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);
	_vector		vPickPos = XMVector3TransformCoord(XMLoadFloat4(&m_vPickPos), ProjMatrixInv);

	/* �佺���̽����� ��ġ -> ���� ���� ��ġ�� �����ϳ�. */
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
