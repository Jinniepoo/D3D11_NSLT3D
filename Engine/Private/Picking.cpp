#include "Picking.h"
#include "Transform.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(pDevice);
    Safe_AddRef(pContext);
}

CPicking::CPicking(const CPicking& rhs)
    : m_pDevice{ rhs.m_pDevice }
    , m_pContext{ rhs.m_pContext }
    , m_hWnd{ rhs.m_hWnd }
    , m_iWinSizeX{ rhs.m_iWinSizeX }
    , m_iWinSizeY{ rhs.m_iWinSizeY }
    , m_ViewMatrix{ rhs.m_ViewMatrix }
    , m_ProjMatrix{ rhs.m_ProjMatrix }
    , m_vRayPos{ rhs.m_vRayPos }
    , m_vRayDir{ rhs.m_vRayDir }
{
    if (m_pDevice) {
        m_pDevice->AddRef();
    }
    if (m_pContext) {
        m_pContext->AddRef();
    }
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	m_hWnd = hWnd;

	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

    m_pGameInstance = CGameInstance::Get_Instance();
	return S_OK;
}

void CPicking::Tick()
{
    POINT ptMouse;
    GetCursorPos(&ptMouse);
    ScreenToClient(m_hWnd, &ptMouse);

    float vx = (2.0f * ptMouse.x / m_iWinSizeX - 1.0f);
    float vy = -(2.0f * ptMouse.y / m_iWinSizeY - 1.0f);
    _vector vMousePos = XMVectorSet(vx, vy, 1.0f, 1.0f);

    XMStoreFloat4x4(&m_ViewMatrix, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    XMStoreFloat4x4(&m_ProjMatrix, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    _matrix ProjMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ProjMatrix));
    vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInv);

    m_vRayPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    m_vRayDir = vMousePos - m_vRayPos;

    _matrix ViewMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ViewMatrix));
    m_vRayPos = XMVector3TransformCoord(m_vRayPos, ViewMatrixInv);
    m_vRayDir = XMVector3TransformNormal(m_vRayDir, ViewMatrixInv);
    m_vRayDir = XMVector3Normalize(m_vRayDir);
}

void CPicking::Compute_LocalRayInfo(_vector* pRayDir, _vector* pRayPos, const _matrix& WorldMatrix)
{
    XMMATRIX WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrix);
    *pRayPos = XMVector3TransformCoord(m_vRayPos, WorldMatrixInv);
    *pRayDir = XMVector3TransformNormal(m_vRayDir, WorldMatrixInv);
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    CPicking* pInstance = new CPicking(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
    {
        MSG_BOX(L"Failed To Created CPicking");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPicking::Free()
{
    __super::Free();
    
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
