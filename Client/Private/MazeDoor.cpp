#include "stdafx.h"
#include "MazeDoor.h"

#include "GameInstance.h"
#include "Player.h"

CMazeDoor::CMazeDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMazeDoor::CMazeDoor(const CMazeDoor& rhs)
    : CGameObject(rhs)
{
}

HRESULT CMazeDoor::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMazeDoor::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitialPosition); 
    m_pTransformCom->Set_Scale(_float3(1.1f, 1.1f, 1.1f));

    m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_GAMEPLAY, L"Layer_Player")->front());
    Safe_AddRef(m_pPlayer);

    if (m_pPlayer == nullptr)
        return E_FAIL;

    return S_OK;
}

void CMazeDoor::PriorityTick(_float fTimeDelta)
{
}

void CMazeDoor::Tick(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Collider", 0));

    if (m_pColliderCom->Intersect(pTargetCollider))
        m_bIsOpening = true;

    if (m_bIsOpening)
    {
        m_fElapsedTime += fTimeDelta;

        _float fAngle = min(m_fElapsedTime * 45.0f, 90.0f);

        float offset = (fAngle / 90.0f) * 4.f; 
        _vector vInitialPosition = XMVectorSet(43.6f, 4.f, 50.36f, 1.f);
        _vector vCurrentPosition = XMVectorSet(43.6f + offset, 4.f, 50.36f + offset, 1.f);

        XMMATRIX translationMatrix = XMMatrixTranslationFromVector(vCurrentPosition);
        XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(fAngle));

        XMMATRIX doorMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);

        XMFLOAT4X4 float4x4Matrix;
        XMStoreFloat4x4(&float4x4Matrix, doorMatrix);
        m_pTransformCom->Set_WorldMat(float4x4Matrix);

        if (fAngle >= 90.0f)
        {
            m_bIsOpening = false;
            m_fElapsedTime = 0.0f;
        }
    }
}

void CMazeDoor::LateTick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CMazeDoor::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Bind_Buffers(i);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CMazeDoor::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MazeDoorL"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc{};

    BoundingDesc.vExtents = _float3(2.7f, 3.f, 3.5f);
    BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMazeDoor::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

_bool CMazeDoor::Near_Player()
{
    _vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayer->Get_Player_Position();

    _vector vDistance = XMVector3Length(vPlayerPosition - vMonsterPosition);

    return XMVectorGetX(vDistance) < 5.0f;
}

CMazeDoor* CMazeDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMazeDoor* pInstance = new CMazeDoor(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMazeDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMazeDoor::Clone(void* pArg)
{
    CMazeDoor* pInstance = new CMazeDoor(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMazeDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CMazeDoor::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);

    if (m_pPlayer)
        Safe_Release(m_pPlayer);
}
