#include "stdafx.h"
#include "ModelObjects.h"

#include "GameInstance.h"
#include "Camera_Player.h"

CModelObjects::CModelObjects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CModelObjects::CModelObjects(const CModelObjects& rhs)
	: CGameObject(rhs)
	, m_ModelDesc{ rhs.m_ModelDesc }
{
	lstrcpy(m_strComponentTag, rhs.m_strComponentTag);
}


HRESULT CModelObjects::Initialize_Prototype(const _tchar* strComponentTag)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	lstrcpy(m_strComponentTag, strComponentTag);

	return S_OK;
}

HRESULT CModelObjects::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_ModelDesc = *(MODEL_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Set_WorldMat(m_ModelDesc.matModel);

	return S_OK;
}

void CModelObjects::PriorityTick(_float fTimeDelta)
{

}

void CModelObjects::Tick(_float fTimeDelta)
{
	if (m_pCamera != nullptr)
		m_vCameraPlayerPos = m_pCamera->Get_State(CTransform::STATE_POSITION);

	_float fDistanceX;
	fDistanceX = XMVectorGetX(XMVector3Length(m_vCameraPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (fDistanceX <= 5.f)
		m_bNearCam = true;
	else
		m_bNearCam = false;
}

void CModelObjects::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CModelObjects::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
		//	return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pModelCom->Bind_Buffers(i);
		m_pModelCom->Render(i);
		
	}

	return S_OK;
}

HRESULT CModelObjects::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelObjects::Bind_ShaderResources()
{
	if (nullptr == m_pCameraPlayer)
	{
		m_pCameraPlayer = dynamic_cast<CCamera_Player*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_CameraPlayer")->front());
		Safe_AddRef(m_pCameraPlayer);
	}

	if (nullptr == m_pCamera)
	{
		m_pCamera = FIND_TRANSFORM(m_pCameraPlayer, L"Com_Transform");
		Safe_AddRef(m_pCamera);
	}

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsNear", &m_bNearCam, sizeof _bool)))
	//	return E_FAIL;

	return S_OK;
}

void CModelObjects::Save_ModelData(HANDLE hFile, _ulong& dwByte)
{
	const _float4x4 WorldMatrix = *(m_pTransformCom->Get_WorldFloat4x4());

	WriteFile(hFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);

	DWORD iLen1 = static_cast<DWORD>(strlen(m_ModelDesc.strName));
	DWORD iLen2 = static_cast<DWORD>(strlen(m_ModelDesc.strNumbered));
	WriteFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, &m_ModelDesc.strName, iLen1, &dwByte, nullptr);
	WriteFile(hFile, &m_ModelDesc.strNumbered, iLen2, &dwByte, nullptr);
}

wstring CModelObjects::CharToWString(_char* szSrc, wstring& strDest)
{
	if (szSrc == nullptr)
	{
		strDest.clear();
		return strDest;
	}

	int size_needed = MultiByteToWideChar(CP_ACP, 0, szSrc, (int)strlen(szSrc), NULL, 0);

	strDest.resize(size_needed);

	MultiByteToWideChar(CP_ACP, 0, szSrc, (int)strlen(szSrc), &strDest[0], size_needed);

	return strDest;
}

CModelObjects* CModelObjects::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* strComponentTag)
{
	CModelObjects* pInstance = new CModelObjects(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strComponentTag)))
	{
		MSG_BOX(TEXT("Failed to Created : CModelObjects"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CModelObjects::Clone(void* pArg)
{
	CModelObjects* pInstance = new CModelObjects(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CModelObjects"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModelObjects::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pCamera);
}
