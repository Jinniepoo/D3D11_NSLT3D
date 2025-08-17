#include "stdafx.h"
#include "Collectible_Coin.h"

#include "GameInstance.h"
#include "Player.h"
#include "CoinEffect.h"
#include "UI_Clover_Check.h"
#include "UI_Clover_L.h"
#include "UI_Clover_Question.h"
#include "UI_Coins.h"
#include "UI_Lucky.h"
#include "UI_Heart_Filling.h"
#include "UI_Lives.h"

CCollectible_Coin::CCollectible_Coin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCollectible_Coin::CCollectible_Coin(const CCollectible_Coin& rhs)
	: CGameObject(rhs)
	, m_CltbDesc{ rhs.m_CltbDesc }
{
	lstrcpy(m_strComponentTag, rhs.m_strComponentTag);
}

HRESULT CCollectible_Coin::Initialize_Prototype(const _tchar* strComponentTag)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	lstrcpy(m_strComponentTag, strComponentTag);

	return S_OK;
}

HRESULT CCollectible_Coin::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_CltbDesc = *(CLTB_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMat(m_CltbDesc.matModel);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fCoinRot));

	if (m_CltbDesc.m_eLevel == LEVEL_GAMEPLAY)
		m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player")->front());
	if (m_CltbDesc.m_eLevel == LEVEL_PUZZLE)
		m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_PUZZLE, L"Layer_Player")->front());
	Safe_AddRef(m_pPlayer);

	m_pClover = dynamic_cast<CUI_Clover_Check*>(GET_LIST(LEVEL_STATIC, L"Layer_Clover_Check")->front());
	Safe_AddRef(m_pClover);

	m_pUICoin = dynamic_cast<CUI_Coins*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_Coins")->front());
	Safe_AddRef(m_pUICoin);

	m_pLucky = dynamic_cast<CUI_Clover_L*>(GET_LIST(LEVEL_STATIC, L"Layer_Clover_Lucky")->front());
	Safe_AddRef(m_pLucky);

	m_pQuestion = dynamic_cast<CUI_Clover_Question*>(GET_LIST(LEVEL_STATIC, L"Layer_Clover_Question")->front());
	Safe_AddRef(m_pQuestion);

	m_pHeart = dynamic_cast<CUI_Heart_Filling*>(GET_LIST(LEVEL_STATIC, L"Layer_Heart_Filling")->front());
	Safe_AddRef(m_pHeart);

	m_pLives = dynamic_cast<CUI_Lives*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_Lives")->front());
	Safe_AddRef(m_pLives);

	return S_OK;
}

void CCollectible_Coin::PriorityTick(_float fTimeDelta)
{
	if (m_CltbDesc.m_CltbType == TYPE_END)
	{
		if (std::strstr(m_CltbDesc.strName, "Clover") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_CLOVER;
		}
		else if (std::strstr(m_CltbDesc.strName, "Heart") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_HEART;
		}
		else if (std::strstr(m_CltbDesc.strName, "Life") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_LIFE;
		}
		else if (std::strstr(m_CltbDesc.strName, "Key") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_KEY;
		}
		else if (std::strstr(m_CltbDesc.strName, "Letter_L") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_L;
		}
		else if (std::strstr(m_CltbDesc.strName, "Letter_U") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_U;
		}
		else if (std::strstr(m_CltbDesc.strName, "Letter_C") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_C;
		}
		else if (std::strstr(m_CltbDesc.strName, "Letter_K") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_K;
		}
		else if (std::strstr(m_CltbDesc.strName, "Letter_Y") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_Y;
		}
		else if (std::strstr(m_CltbDesc.strName, "Burrow_Coin") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_BURROWCOIN;
		}
		else if (std::strstr(m_CltbDesc.strName, "Burrow_Dirt") != nullptr) {
			m_CltbDesc.m_CltbType = TYPE_BURROWDIRT;
		}
		else {
			m_CltbDesc.m_CltbType = TYPE_COIN;
		}
	}
	if (m_CltbDesc.m_CltbType != TYPE_BURROWCOIN &&
		m_CltbDesc.m_CltbType != TYPE_BURROWDIRT)
		m_fCoinRot += 3.f;
}

void CCollectible_Coin::Tick(_float fTimeDelta)
{
	if (m_CltbDesc.m_CltbState != CLTB_DROP && m_CltbDesc.m_CltbType != TYPE_BURROWDIRT)
	{
		m_fTimer += fTimeDelta * 0.1f;

		if (false == m_bLoot)
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fCoinRot));

		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

		CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Collider", 0));
		CCollider* pTailCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_TailCollider", 0));

		if (true == m_pColliderCom->Intersect(pTargetCollider) || true == m_pColliderCom->Intersect(pTailCollider))
			m_bLoot = true;

		if (true == m_bLoot)
		{
			PLAYONLY(L"Coin_Loot2");
			VOLUME(L"Coin_Loot2", 80);
			LootEffect();
		}
	}

	if (m_CltbDesc.m_CltbState == CLTB_DROP && m_CltbDesc.m_CltbType != TYPE_BURROWDIRT)
	{
		const float fGravity = -9.81f;
		const float fInitialVelocity = 10.0f;
		const float fAngle = 45.0f;
		const float fInitialYOffset = 3.f;

		float fAngleRad = XMConvertToRadians(fAngle);

		float fInitialVelocityX = fInitialVelocity * cosf(fAngleRad);
		float fInitialVelocityY = fInitialVelocity * sinf(fAngleRad);

		if (m_fDropTime == 0.0f)
		{
			m_fDropTime = 0.f;
			m_fInitialY = m_CltbDesc.matModel.m[3][1] + fInitialYOffset;
			m_fInitialVelocityX = fInitialVelocityX;
			m_fInitialVelocityY = fInitialVelocityY;
		}

		if (m_bDropEffect)
		{
			if (m_bLootCoin)
			{
				PLAY(L"Coin_Loot", false);
				VOLUME(L"Coin_Loot", 50);
				m_bLootCoin = false;
			}

			_float3 vModelPos(m_CltbDesc.matModel._41, m_CltbDesc.matModel._42, m_CltbDesc.matModel._43);
			vModelPos.y += 3;

			_vector vModelPos1 = XMLoadFloat3(&vModelPos);
			vModelPos1 = XMVectorSetW(vModelPos1, 1.f);

			CCoinEffect::COINEFFECT_DESC CoinEffectDesc;
			CoinEffectDesc.vPos = vModelPos1;

			m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, L"Layer_CoinEffect", L"Prototype_GameObject_Effect_CoinEffect", &CoinEffectDesc);
			m_bDropEffect = false;
		}

		m_fDropTime += fTimeDelta;

		float fYPosition = m_fInitialY + m_fInitialVelocityY * m_fDropTime + 0.5f * fGravity * m_fDropTime * m_fDropTime;
		float fXPosition = m_fInitialX + m_fInitialVelocityX * m_fDropTime;

		if (fYPosition <= m_CltbDesc.matModel.m[3][1] - 0.2f)
		{
			fYPosition = m_CltbDesc.matModel.m[3][1];

			m_bIsDead = true;
			if (m_CltbDesc.m_CltbType == TYPE_COIN || 
				m_CltbDesc.m_CltbType == TYPE_BURROWCOIN)
			{
				m_pUICoin->Set_Anim(true);
				m_pUICoin->Set_CoinScore();
			}
		}

		m_CltbDesc.matModel.m[3][1] = fYPosition;
		m_pTransformCom->Set_WorldMat(m_CltbDesc.matModel);
	}
	if (m_CltbDesc.m_CltbType == TYPE_BURROWDIRT)
	{
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

		CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Collider", 0));

		if (true == m_pColliderCom->Intersect(pTargetCollider))
			this->Set_Dead(true);
	}
}

void CCollectible_Coin::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
	//m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_SHADOW, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CCollectible_Coin::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pModelCom->Bind_Buffers(i);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

//HRESULT CCollectible_Coin::Render_LightDepth()
//{
//	const _float4x4 WorldMatrix = *(m_pTransformCom->Get_WorldFloat4x4());
//
//	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
//		return E_FAIL;
//
//	_float4x4		LightViewMatrix, LightProjMatrix;
//
//	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 300.f, 40.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
//	XMStoreFloat4x4(&LightProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));
//
//	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
//		return E_FAIL;
//	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
//		return E_FAIL;
//
//	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
//
//	for (_uint i = 0; i < iNumMeshes; i++)
//	{
//		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
//			return E_FAIL;
//
//		m_pShaderCom->Begin(1);
//
//		if (FAILED(m_pModelCom->Bind_Buffers(i)))
//			return E_FAIL;
//
//		m_pModelCom->Render((_uint)i);
//	}
//	return S_OK;
//}

HRESULT CCollectible_Coin::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	BoundingDesc{};

	BoundingDesc.fRadius = 0.5f;
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollectible_Coin::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CCollectible_Coin::Save_ModelData(HANDLE hFile, _ulong& dwByte)
{
	const _float4x4 WorldMatrix = *(m_pTransformCom->Get_WorldFloat4x4());

	WriteFile(hFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);

	DWORD iLen1 = static_cast<DWORD>(strlen(m_CltbDesc.strName));
	DWORD iLen2 = static_cast<DWORD>(strlen(m_CltbDesc.strNumbered));
	WriteFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, &m_CltbDesc.strName, iLen1, &dwByte, nullptr);
	WriteFile(hFile, &m_CltbDesc.strNumbered, iLen2, &dwByte, nullptr);
}

void CCollectible_Coin::LootEffect()
{
	m_CltbDesc.matModel.m[3][1] += 1;
	if (m_CltbDesc.matModel.m[3][1] < m_CltbDesc.matModel.m[3][1] + 5)
		m_pTransformCom->Set_WorldMat(m_CltbDesc.matModel);

	_float fTemp = 0;

	if (m_bLootTimer)
	{
		fTemp = m_fTimer;

		_float3 vModelPos(m_CltbDesc.matModel._41, m_CltbDesc.matModel._42, m_CltbDesc.matModel._43);
		vModelPos.y += 1;

		_vector vModelPos1 = XMLoadFloat3(&vModelPos);
		vModelPos1 = XMVectorSetW(vModelPos1, 1.f);

		CCoinEffect::COINEFFECT_DESC CoinEffectDesc;
		CoinEffectDesc.vPos = vModelPos1;

		m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_CoinEffect", L"Prototype_GameObject_Effect_CoinEffect", &CoinEffectDesc);

		m_bLootTimer = false;
	}

	if (m_fTimer > fTemp + 0.4)
	{
		m_bIsDead = true;
		if (m_CltbDesc.m_CltbType == TYPE_COIN ||
			m_CltbDesc.m_CltbType == TYPE_BURROWCOIN)
		{
			m_pUICoin->Set_Anim(true);
		}
		UIBoardUpdate();
		m_fTimer = 0;
		m_bLoot = false;
		m_bLootTimer = true;
	}
}

void CCollectible_Coin::DropEffect()
{

}

void CCollectible_Coin::UIBoardUpdate()
{
	if (m_CltbDesc.m_CltbType == TYPE_COIN)
		m_pUICoin->Set_CoinScore();
	else if (m_CltbDesc.m_CltbType == TYPE_CLOVER)
	{
		PLAYONLY(L"Clover_Gained");
		VOLUME(L"Clover_Gained", 70);
		m_pClover->Set_TextureIndex(1);
	}
	else if (m_CltbDesc.m_CltbType == TYPE_LIFE)
	{
		PLAYONLY(L"Clover_Gained");
		VOLUME(L"Clover_Gained", 70);
		m_pLives->Increase_PlayerLife();
	}
	else if (m_CltbDesc.m_CltbType == TYPE_HEART)
	{
		PLAYONLY(L"Clover_Gained");
		VOLUME(L"Clover_Gained", 70);
		m_pHeart->Increase_Health();
	}
	else if (m_CltbDesc.m_CltbType == TYPE_L)
	{
		PLAYONLY(L"Clover_Gained");
		VOLUME(L"Clover_Gained", 70);
		CUI_Lucky::UI_DESC LuckyDesc;
		LuckyDesc.iTextureIdx = 1;
		m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Lucky", L"Prototype_GameObject_UI_Lucky", &LuckyDesc);
	}
	else if (m_CltbDesc.m_CltbType == TYPE_U)
	{
		PLAYONLY(L"Clover_Gained");
		VOLUME(L"Clover_Gained", 70);
		CUI_Lucky::UI_DESC LuckyDesc;
		LuckyDesc.iTextureIdx = 3;
		m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Lucky", L"Prototype_GameObject_UI_Lucky", &LuckyDesc);
	}
	else if (m_CltbDesc.m_CltbType == TYPE_C)
	{
		PLAYONLY(L"Clover_Gained");
		VOLUME(L"Clover_Gained", 70);
		CUI_Lucky::UI_DESC LuckyDesc;
		LuckyDesc.iTextureIdx = 4;
		m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Lucky", L"Prototype_GameObject_UI_Lucky", &LuckyDesc);
	}
	else if (m_CltbDesc.m_CltbType == TYPE_K)
	{
		PLAYONLY(L"Clover_Gained");
		VOLUME(L"Clover_Gained", 70);
		CUI_Lucky::UI_DESC LuckyDesc;
		LuckyDesc.iTextureIdx = 2;
		m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Lucky", L"Prototype_GameObject_UI_Lucky", &LuckyDesc);
	}
	else if (m_CltbDesc.m_CltbType == TYPE_Y)
	{ 
		PLAYONLY(L"Lucky_Gained");
		VOLUME(L"Lucky_Gained", 70);
		CUI_Lucky::UI_DESC LuckyDesc;
		LuckyDesc.iTextureIdx = 5;
		m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Lucky", L"Prototype_GameObject_UI_Lucky", &LuckyDesc);
		m_pLucky->Set_TextureIndex(1);
	}
	else if (m_CltbDesc.m_CltbType == TYPE_BURROWCOIN)
		m_pUICoin->Set_CoinScore();
	else
		m_pUICoin->Set_CoinScore();
}

wstring CCollectible_Coin::CharToWString(_char* szSrc, wstring& strDest)
{
	if (szSrc == nullptr)
	{
		strDest.clear();
		return strDest;
	}

	int size_needed = MultiByteToWideChar(CP_ACP, 0, szSrc, static_cast<int>(strlen(szSrc)), NULL, 0);

	strDest.resize(size_needed);

	MultiByteToWideChar(CP_ACP, 0, szSrc, static_cast<int>(strlen(szSrc)), &strDest[0], size_needed);

	return strDest;
}

CCollectible_Coin* CCollectible_Coin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* strComponentTag)
{
	CCollectible_Coin* pInstance = new CCollectible_Coin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strComponentTag)))
	{
		MSG_BOX(TEXT("Failed to Created : CCollectable_Coin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCollectible_Coin::Clone(void* pArg)
{
	CCollectible_Coin* pInstance = new CCollectible_Coin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCollectable_Coin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollectible_Coin::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);

	if (m_pPlayer)
		Safe_Release(m_pPlayer);

	if (m_pClover)
		Safe_Release(m_pClover);

	if (m_pUICoin)
		Safe_Release(m_pUICoin);

	if (m_pLucky)
		Safe_Release(m_pLucky);

	if (m_pQuestion)
		Safe_Release(m_pQuestion);

	if (m_pHeart)
		Safe_Release(m_pHeart);

	if (m_pLives)
		Safe_Release(m_pLives);
}
