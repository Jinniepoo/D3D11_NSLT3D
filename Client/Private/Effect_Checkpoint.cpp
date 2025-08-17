#include "stdafx.h"
#include "..\Public\Effect_Checkpoint.h"

#include "GameInstance.h"

CEffect_Checkpoint::CEffect_Checkpoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffect_Checkpoint::CEffect_Checkpoint(const CEffect_Checkpoint& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Checkpoint::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Checkpoint::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_CPEffectDesc = *(CPEFFECT_DESC*)pArg;

	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_CPEffectDesc.vPos);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 10), 2.0f, _float(rand() % 10), 1.f));
	m_pTransformCom->Set_Scale(_float3(40.f, 40.f, 40.f));

	return S_OK;
}

void CEffect_Checkpoint::PriorityTick(_float fTimeDelta)
{
}

void CEffect_Checkpoint::Tick(_float fTimeDelta)
{
	if (m_bNoLoop)
		return;

	m_fFrame += 20.f * fTimeDelta;

	if (m_fFrame >= 20.f) 
	{
		m_fFrame = 20.f;
		m_bNoLoop = true;
	}
}

void CEffect_Checkpoint::LateTick(_float fTimeDelta)
{
	m_pTransformCom->Set_BillBoard();
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_BLEND, this);
}

HRESULT CEffect_Checkpoint::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (_uint)m_fFrame)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Checkpoint::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Checkpoint"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_Checkpoint* CEffect_Checkpoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Checkpoint* pInstance = new CEffect_Checkpoint(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Checkpoint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CEffect_Checkpoint::Clone(void* pArg)
{
	CEffect_Checkpoint* pInstance = new CEffect_Checkpoint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Checkpoint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Checkpoint::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
