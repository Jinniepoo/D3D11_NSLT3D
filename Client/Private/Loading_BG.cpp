#include "stdafx.h"
#include "Loading_BG.h"

#include "GameInstance.h"

CLoading_BG::CLoading_BG(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLoading_BG::CLoading_BG(const CLoading_BG & rhs)
	: CGameObject( rhs )
{
}

HRESULT CLoading_BG::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_BG::Initialize(void * pArg)
{
	if (nullptr != pArg)
		m_BackDesc = *(BG_DESC*)pArg;

	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3((_float)m_BackDesc.iSizeX, (_float)m_BackDesc.iSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_BackDesc.fX - g_iWinSizeX * 0.5f, - m_BackDesc.fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	
	return S_OK;
}

void CLoading_BG::PriorityTick(_float fTimeDelta)
{
}

void CLoading_BG::Tick(_float fTimeDelta)
{
}

void CLoading_BG::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CLoading_BG::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (LOADING_BG == m_BackDesc.eObjType)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}

	if (MAIN_BG == m_BackDesc.eObjType)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_BG::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (LOADING_BG == m_BackDesc.eObjType)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGround"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}

	if (MAIN_BG == m_BackDesc.eObjType)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_MAIN, TEXT("Prototype_Component_Texture_BackGround"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}



	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CLoading_BG * CLoading_BG::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLoading_BG*		pInstance = new CLoading_BG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLoading_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CLoading_BG::Clone(void * pArg)
{
	CLoading_BG*		pInstance = new CLoading_BG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLoading_BG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading_BG::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
