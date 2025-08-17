#include "VIBuffer_Mesh_Instance.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& rhs)
    : CVIBuffer_Instance(rhs)
    , m_pInstanceVertices(rhs.m_pInstanceVertices)
    , m_iNumInstance(rhs.m_iNumInstance)
{
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc)
{
    if (FAILED(__super::Initialize_Prototype(pInstanceDesc)))
        return E_FAIL;

    m_iNumIndexPerInstance = 0;  
    m_iVertexStride = sizeof(VTXMESH); 
    m_iNumVertices = 0;  
    m_iIndexStride = sizeof(_ushort);
    m_iNumIndices = 0;  
    m_iNumVertexBuffers = 2;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    m_pInstanceVertices = new VTXMATRIX[m_iNumInstance];
    ZeroMemory(m_pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        float fScale = Compute_RandomNumber(pInstanceDesc->vScale.x, pInstanceDesc->vScale.y);
        float fLifeTime = Compute_RandomNumber(pInstanceDesc->vLifeTime.x, pInstanceDesc->vLifeTime.y);

        m_pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
        m_pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
        m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
        m_pInstanceVertices[i].vTranslation = _float4(
            Compute_RandomNumber(pInstanceDesc->vCenter.x - pInstanceDesc->vRange.x * 0.5f, pInstanceDesc->vCenter.x + pInstanceDesc->vRange.x * 0.5f),
            Compute_RandomNumber(pInstanceDesc->vCenter.y - pInstanceDesc->vRange.y * 0.5f, pInstanceDesc->vCenter.y + pInstanceDesc->vRange.y * 0.5f),
            Compute_RandomNumber(pInstanceDesc->vCenter.z - pInstanceDesc->vRange.z * 0.5f, pInstanceDesc->vCenter.z + pInstanceDesc->vRange.z * 0.5f),
            1.f);

        m_pInstanceVertices[i].vLifeTime.x = fLifeTime;
    }

    return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Initialize(void* pArg)
{
    m_iInstanceVertexStride = sizeof(VTXMATRIX);

    ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
    m_BufferDesc.ByteWidth = m_iInstanceVertexStride * m_iNumInstance;
    m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = m_pInstanceVertices;

    if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

void CVIBuffer_Mesh_Instance::Update_Spread(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        VTXMATRIX* pVertices = static_cast<VTXMATRIX*>(SubResource.pData) + i;

        _vector vDir = XMLoadFloat4(&pVertices->vTranslation) - XMVectorSetW(XMLoadFloat3(&m_vPivot), 1.f);

        XMStoreFloat4(&pVertices->vTranslation, XMLoadFloat4(&pVertices->vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);

        pVertices->vLifeTime.y += fTimeDelta;
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Mesh_Instance::Create_Buffer(ID3D11Buffer** ppBuffer, const D3D11_BUFFER_DESC& BufferDesc, const D3D11_SUBRESOURCE_DATA& InitialData)
{
    if (nullptr == ppBuffer)
        return E_POINTER;

    HRESULT hr = S_OK;

    Safe_Release(*ppBuffer);

    hr = m_pDevice->CreateBuffer(&BufferDesc, &InitialData, ppBuffer);

    if (FAILED(hr))
    {
        MSG_BOX(TEXT("Failed to create buffer"));
        return hr;
    }

    return S_OK;
}


CVIBuffer_Mesh_Instance* CVIBuffer_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc)
{
    CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pInstanceDesc)))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_Mesh_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Mesh_Instance::Clone(void* pArg)
{
    CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Mesh_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Mesh_Instance::Free()
{
    __super::Free();

    if (false == m_isCloned)
    {
        Safe_Delete_Array(m_pInstanceVertices);
    }
}
