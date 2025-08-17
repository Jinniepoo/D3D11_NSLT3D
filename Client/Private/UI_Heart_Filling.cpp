#include "stdafx.h"
#include "UI_Heart_Filling.h"
#include "UI_Lives.h"

#include "GameInstance.h"

_int CUI_Heart_Filling::m_iTextureIdx = { 0 };

CUI_Heart_Filling::CUI_Heart_Filling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUI_Heart_Filling::CUI_Heart_Filling(const CUI_Heart_Filling& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Heart_Filling::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Heart_Filling::Initialize(void* pArg)
{
	HEART_DESC* pHeartDesc = static_cast<HEART_DESC*>(pArg);

	m_fX = pHeartDesc->fX;
	m_fY = pHeartDesc->fY;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr == pArg)
		return S_OK;

	m_fSizeX = 389.f / 2.3f;
	m_fSizeY = 125.f / 2.3f;

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	m_pLives = dynamic_cast<CUI_Lives*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_Lives")->front());
	Safe_AddRef(m_pLives);

	if (nullptr == m_pLives)
		return E_FAIL;

	return S_OK;
}

void CUI_Heart_Filling::PriorityTick(_float fTimeDelta)
{
}

void CUI_Heart_Filling::Tick(_float fTimeDelta)
{
	if (m_iTextureIdx > 5)
	{
		m_iTextureIdx = 0;
		m_pLives->Decrease_PlayerLife();
	}
}

void CUI_Heart_Filling::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Heart_Filling::Render()
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

HRESULT CUI_Heart_Filling::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Heart_Filling::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UI_Heart_Filling"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Heart_Filling* CUI_Heart_Filling::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Heart_Filling* pInstance = new CUI_Heart_Filling(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Heart_Filling"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_Heart_Filling::Clone(void* pArg)
{
	CUI_Heart_Filling* pInstance = new CUI_Heart_Filling(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Heart_Filling"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Heart_Filling::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
