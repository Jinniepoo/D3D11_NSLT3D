#include "stdafx.h"
#include "UI_Clover_Check.h"

#include "GameInstance.h"
#include "Level.h"

CUI_Clover_Check::CUI_Clover_Check(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUI_Clover_Check::CUI_Clover_Check(const CUI_Clover_Check& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Clover_Check::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Clover_Check::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_UIDesc = *(UI_DESC*)pArg;

	m_eLevel = m_UIDesc.eLevel;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 125.f / 1.8f;
	m_fSizeY = 125.f / 1.8f;

	m_fX = g_iWinSizeX * 0.8f;
	m_fY = g_iWinSizeY * 0.075f;

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI_Clover_Check::PriorityTick(_float fTimeDelta)
{
}

void CUI_Clover_Check::Tick(_float fTimeDelta)
{
	if (m_iTextureIdx == 1 && m_bAnim)
	{
		if (!m_bAnimating)
		{
			m_bAnimating = true;
			m_fAnimationTime = 0.f; 
		}

		m_fAnimationTime += fTimeDelta;

		float phaseDuration = 0.3f; 
		float totalDuration = 0.6f; 

		float t = m_fAnimationTime / totalDuration;

		t = std::min(1.f, t);

		float scaleFactor;
		if (t < 0.5f) 
		{
			scaleFactor = t / 0.3f; 
			m_fSizeX = 125.f / 1.8f + 20.f * scaleFactor; 
			m_fSizeY = 125.f / 1.8f - 20.f * scaleFactor; 
		}
		else 
		{
			scaleFactor = (t - 0.5f) / 0.3f;
			m_fSizeX = 125.f / 1.8f + 20.f * (1.f - scaleFactor); 
			m_fSizeY = 125.f / 1.8f - 20.f * (1.f - scaleFactor); 
		}

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));

		if (m_fAnimationTime >= totalDuration)
		{
			m_bAnimating = false;
			m_pTransformCom->Set_Scale(_float3(125.f / 1.8f, 125.f / 1.8f, 1.f)); 
			m_bAnim = false;
		}
	}
	else
	{
		if (m_bAnimating)
		{
			m_bAnimating = false;
			m_pTransformCom->Set_Scale(_float3(125.f / 1.8f, 125.f / 1.8f, 1.f)); 
		}
	}
}


void CUI_Clover_Check::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Clover_Check::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Clover_Check::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Clover_Check::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UI_Clover_Check"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Clover_Check* CUI_Clover_Check::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Clover_Check* pInstance = new CUI_Clover_Check(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Clover_Check"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_Clover_Check::Clone(void* pArg)
{
	CUI_Clover_Check* pInstance = new CUI_Clover_Check(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Clover_Check"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Clover_Check::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
