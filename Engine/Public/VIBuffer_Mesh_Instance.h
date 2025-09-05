#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh_Instance  final : public CVIBuffer_Instance
{
private:
	CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& rhs);
	virtual ~CVIBuffer_Mesh_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Update_Spread(_float fTimeDelta);

private:
	VTXMATRIX*	m_pInstanceVertices = { nullptr };
	size_t		m_iNumInstance = 0;

	HRESULT		Create_Buffer(ID3D11Buffer** ppBuffer, const D3D11_BUFFER_DESC& BufferDesc, const D3D11_SUBRESOURCE_DATA& InitialData);

public:
	static CVIBuffer_Mesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
