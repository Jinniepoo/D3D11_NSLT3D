#include "stdafx.h"
#include "UI_Lucky.h"

#include "GameInstance.h"

CUI_Lucky::CUI_Lucky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUI_Lucky::CUI_Lucky(const CUI_Lucky& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Lucky::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Lucky::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_UIDesc = *(UI_DESC*)pArg;

	m_eLevel = m_UIDesc.eLevel;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 1821.f / 5.f;
	m_fSizeY = 1024.f / 5.f;

	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.1f;

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI_Lucky::PriorityTick(_float fTimeDelta)
{

}

void CUI_Lucky::Tick(_float fTimeDelta)
{
	//if (m_bAnim)
	//{
	m_fElapsedTime += fTimeDelta;

	const float fAppearDuration = 0.3f;
	const float fStayDuration = 1.f;
	const float fDisappearDuration = 0.2f;
	const float fTotalDuration = fAppearDuration + fStayDuration + fDisappearDuration;

	float fNormalizedTime = m_fElapsedTime / fTotalDuration;
	if (fNormalizedTime > 1.0f) fNormalizedTime = 1.0f;

	_vector vInitPos = XMVectorSet(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.f, 0.f);
	_vector vFinalPos = XMVectorSet(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.1f, 0.f, 0.f);
	_vector vEndingPos = XMVectorSet(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.25f, 0.f, 0.f);

	_vector vInitScale = XMVectorSet(m_fSizeX / 30.f, m_fSizeY / 30.f, 1.f, 0.f);
	_vector vFinalScale = XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f);

	_vector vCurScale;
	_vector vCurPos;

	if (fNormalizedTime <= fAppearDuration / fTotalDuration)
	{
		float phaseNormalizedTime = fNormalizedTime / (fAppearDuration / fTotalDuration);
		vCurScale = XMVectorLerp(vInitScale, vFinalScale, phaseNormalizedTime);
		vCurPos = XMVectorLerp(vInitPos, vFinalPos, phaseNormalizedTime);
	}
	else if (fNormalizedTime <= (fAppearDuration + fStayDuration) / fTotalDuration)
	{
		vCurScale = vFinalScale;
		vCurPos = vFinalPos;
	}
	else
	{
		float phaseNormalizedTime = (fNormalizedTime - (fAppearDuration + fStayDuration) / fTotalDuration) / (fDisappearDuration / fTotalDuration);
		vCurScale = XMVectorLerp(vFinalScale, vInitScale, phaseNormalizedTime);
		vCurPos = XMVectorLerp(vFinalPos, vEndingPos, phaseNormalizedTime);
	}

	_float3 vCurScaleFloat3;
	XMStoreFloat3(&vCurScaleFloat3, vCurScale);
	_float2 vCurPosFloat2;
	XMStoreFloat2(&vCurPosFloat2, vCurPos);

	m_pTransformCom->Set_Scale(vCurScaleFloat3);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vCurPosFloat2.x - g_iWinSizeX * 0.5f, -vCurPosFloat2.y + g_iWinSizeY * 0.5f, 0.f, 1.f));

	if (m_fElapsedTime >= fTotalDuration)
	{
		m_fElapsedTime = 0.0f;
		this->Set_Dead(true);
		//m_bAnim = false;
	}
	//}
}

void CUI_Lucky::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Lucky::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_UIDesc.iTextureIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Lucky::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Lucky"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Lucky::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CUI_Lucky* CUI_Lucky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Lucky* pInstance = new CUI_Lucky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Lucky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_Lucky::Clone(void* pArg)
{
	CUI_Lucky* pInstance = new CUI_Lucky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Lucky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Lucky::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

}
