#include "..\Public\Navigation.h"
#include "Cell.h"

#include "GameInstance.h"

_float4x4	CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells{ rhs.m_Cells }
	, m_iCurrentIndex{ rhs.m_iCurrentIndex }
#ifdef _DEBUG
	, m_pShader{ rhs.m_pShader }
	, m_vecNavPoints{ rhs.m_vecNavPoints }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const _wstring& strNavigationDataFile)
{
	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[CCell::POINT_END] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		for (size_t i = 0; i < 3; i++)
			m_vecNavPoints.push_back(vPoints[i]);

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;
		
		m_Cells.emplace_back(pCell);
	}

	CloseHandle(hFile);
	SetUp_Neighbors();

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		CNavigation::NAVIGATION_DESC* pDesc = static_cast<CNavigation::NAVIGATION_DESC*>(pArg);
		m_iCurrentIndex = pDesc->iCurrentCellIndex;

	}

	return S_OK;
}

HRESULT CNavigation::Render()
{
#ifdef _DEBUG

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4 vValue = { 0.f, 1.f, 0.f, 1.f };

	if (-1 == m_iCurrentIndex)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vValue, sizeof _float4)))
			return E_FAIL;

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
			pCell->Render();
	}
	else
	{
		_float4x4		WorldMatrix = m_WorldMatrix;
		WorldMatrix.m[3][1] += 0.1f;

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		_float4 vValue1 = { 1.f, 0.f, 0.f, 1.f };

		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vValue1, sizeof _float4)))
			return E_FAIL;

		m_pShader->Begin(0);

		m_Cells[m_iCurrentIndex]->Render();
	}

#endif

	return S_OK;
}

_bool CNavigation::isMove(_fvector vPosition)
{
	if (-1 == m_iCurrentIndex || m_iCurrentIndex >= m_Cells.size())
		return false;

	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_int		iNeighborIndex = { -1 };

	if (true == m_Cells[m_iCurrentIndex]->isIn(vLocalPos, &iNeighborIndex))
		return true;

	else 
	{
		if (-1 == iNeighborIndex)
			return false;

		else
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex))
					break;
			}

			m_iCurrentIndex = iNeighborIndex;
			return true;
		}
	}
}

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

HRESULT CNavigation::Tool_DrawNav(vector<_float3>& vecPoints)
{
	_float3		vPoints[CCell::POINT_END] = {};

	for (int i = 0; i < 3; i++)
	{
		vPoints[i] = vecPoints[i];
	}

	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;

	m_Cells.emplace_back(pCell);

	SetUp_Neighbors();

	return S_OK;
}

HRESULT CNavigation::Tool_LoadNav(const _wstring& strNavigationDataFile)
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0fbx 파일\0*.fbx";

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = filter;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrInitialDir = L"../Bin/Resource/DataFiles/";

	_int iRet = { 0 };
	iRet = GetOpenFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Load File");
		return E_FAIL;
	}

	DWORD dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Load");
		return E_FAIL;
	}

	while (true)
	{
		_float3		vPoints[CCell::POINT_END] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);
	}

	CloseHandle(hFile);

	return S_OK;
}

_float CNavigation::Get_CellHeight()
{
	if (m_iCurrentIndex != -1 && m_iCurrentIndex < m_Cells.size())
	{
		CCell* pCurrentCell = m_Cells[m_iCurrentIndex];

		if (pCurrentCell != nullptr)
			return pCurrentCell->Compute_Height(m_vPlayerPos);
	}
	return 0.f;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}
