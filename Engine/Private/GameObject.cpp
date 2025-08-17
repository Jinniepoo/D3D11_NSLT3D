#include "..\Public\GameObject.h"
#include "GameInstance.h"


const _wstring	CGameObject::m_strTransformTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);	
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{ rhs.m_pContext }
	, m_pGameInstance{ rhs.m_pGameInstance }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CGameObject::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	/* 원형을 생서하는 루틴으로 실제 사용하기위한 트랜스폼을 만들어버리자. */
	/* 복제된 객체가 활용하기위한 실제 사용을 위한 트랜스폼을 만든다. */
	/* 1. 객체를 생성하면 무조건 트랜스폼이 추가되는 루틴 */
	/* 2. 트랜폼이 자동으로 객체에게 추가되기위해서는 기존 루틴대로라면 트랜슾모의 웒영이 존재해야하고 복제해서 객체에게 추가한다? */
	/* 3. 트랜스폼 웒여을 엔진이 만들어야되는 상황이 생길 수 있다. 나는 이걸 별로 바라마지 않느다.  */
	/* 4. 프로토타ㅑ입이라는 디자이퍁느을 안쓴다!!! Create로 바로 생성해서 실제 쓰기위한 녀석으로 추가해놔'ㅆ다. */	
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	/* 타 객체가 나의 트랜스폼을 검색할 수 있도록 맵에 추가하낟. */
	if (nullptr != Find_Component(m_strTransformTag))
		return E_FAIL;

	m_Components.emplace(m_strTransformTag, m_pTransformCom);
	Safe_AddRef(m_pTransformCom);

	if (nullptr != pArg)
	{		
		if (FAILED(m_pTransformCom->Initialize(pArg)))
			return E_FAIL;
	}
	
	return S_OK;
}

void CGameObject::PriorityTick(_float fTimeDelta)
{
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::LateTick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}


HRESULT CGameObject::Add_Component(_uint iPrototoypeLevelIndex, const wstring& strPrototypeTag, const wstring & strComponentTag, CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	CComponent*		pComponent = m_pGameInstance->Clone_Component(iPrototoypeLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const wstring & strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;	

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
