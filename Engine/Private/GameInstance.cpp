#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Font_Manager.h"
#include "Light_Manager.h"
#include "Input_Device.h"
#include "Target_Manager.h"
#include "Picking.h"
#include "PixelPicking.h"
#include "Layer.h"
#include "Fmod_System.h"
#include "Sound_Manager.h"
#include "Frustum.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, HINSTANCE hInst, const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pPixelPicking = CPixelPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pPixelPicking)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pFmod_System = CFmod_System::Create();
	if (nullptr == m_pFmod_System)
		return E_FAIL;
	FMOD::System* pFmod_System = m_pFmod_System->GetFmodSystem();

	m_pSound_Manager = CSound_Manager::Create(&pFmod_System);
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager)
		return;

	m_pFmod_System->FmodUpdate();
	m_pInput_Device->Update_InputDev();
	
	m_pPicking->Tick();
	m_pPixelPicking->Update();
	m_pObject_Manager->PriorityTick(fTimeDelta);
	
	/* 카메라가 갱신해준 뷰, 투영행렬을 이용하자. */

	m_pObject_Manager->Tick(fTimeDelta);
	m_pPipeLine->Update();

	m_pFrustum->Update();
	m_pObject_Manager->LateTick(fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);
	
}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pRenderer->Draw();	

	return m_pLevel_Manager->Render();
}
void CGameInstance::Clear(_uint iLevelIndex)
{
	m_pRenderer->Clear();
	m_pObject_Manager->Clear(iLevelIndex);	
	m_pComponent_Manager->Clear(iLevelIndex);
}

#pragma region GRAPHIC_DEVICE
HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);	
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();
}

#pragma endregion

#pragma region INPUT_DEVICE

CInput_Device::INPUT_STATE CGameInstance::Get_DIKState(_ubyte eKeyID)
{
	if (nullptr == m_pInput_Device)
		return CInput_Device::INPUT_STATE::NONE;

	return m_pInput_Device->Get_DIKState(eKeyID);
}

CInput_Device::INPUT_STATE CGameInstance::Get_DIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		CInput_Device::INPUT_STATE::NONE;

	return m_pInput_Device->Get_DIMKeyState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}
#pragma endregion



#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Change_Level(CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Change_Level(pNewLevel);
}

CLevel* CGameInstance::Get_CurrentLevel()
{
	if (nullptr == m_pLevel_Manager)
		return nullptr;

	return m_pLevel_Manager->Get_CurrentLevel();
}
#pragma endregion

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}
HRESULT CGameInstance::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}
void CGameInstance::Compute_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return ;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
	
}
HRESULT CGameInstance::Add_Clone(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Clone(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_GameObject(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

CComponent * CGameInstance::Find_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

list<class CGameObject*>* CGameInstance::Get_List(CLayer* Layer)
{
	if (nullptr == Layer)
		return nullptr;

	return Layer->Get_List();
}

void CGameInstance::Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear_Layer(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

#pragma endregion

#pragma region RENDERER
HRESULT CGameInstance::Add_RenderObjects(CRenderer::RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderObjects(eRenderGroup, pRenderObject);	
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMATIONSTATE eState, _fmatrix TransformationMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformationMatrix);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMATIONSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix(eState);
}

const _float4x4 * CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMATIONSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMATIONSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

const _float4x4 * CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMATIONSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_Transform_Float4x4_Inverse(eState);
}

_vector CGameInstance::Get_CamPosition_Vector() const
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->Get_CamPosition_Vector();
}

const _float4 * CGameInstance::Get_CamPosition_Float4() const
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_CamPosition_Float4();
}

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor, _float fRotation, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fRotation, fScale);
}

HRESULT CGameInstance::Render_FontOutlined(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor1, _fvector vColor2, _float fRotation, _float fScale)
{
	return m_pFont_Manager->RenderOutlined(strFontTag, strText, vPosition, vColor1, vColor2, fRotation, fScale);
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

_float4 CGameInstance::Get_PickedPos()
{
	return m_pPixelPicking->Get_PickedPos();
}

HRESULT CGameInstance::Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strRenderTargetTag, iWidth, iHeight, ePixel, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strRenderTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool isClear)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV, isClear);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_SRV(const _wstring& strRenderTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_RT_SRV(strRenderTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Copy_RT_Resource(const _wstring& strRenderTargetTag, ID3D11Texture2D* pResource)
{
	return m_pTarget_Manager->Copy_Resource(strRenderTargetTag, pResource);
}

HRESULT CGameInstance::Add_Sound(const wstring& strTag, const wstring& strPath)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Add_Sound(strTag, strPath);
}

HRESULT CGameInstance::Play(const wstring& strSoundTag, _bool bLoop)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Play(strSoundTag, bLoop);
}

HRESULT CGameInstance::PlayOnly(const wstring& strSoundTag)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->PlayOnly(strSoundTag);
}

HRESULT CGameInstance::PlayToBgm(const wstring& strSoundTag, _bool bLoop)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->PlayToBgm(strSoundTag, bLoop);
}

HRESULT CGameInstance::Stop(const wstring& strSoundTag)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Stop(strSoundTag);
}

HRESULT CGameInstance::Paused(const wstring& strSoundTag, _bool bPause)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Paused(strSoundTag, bPause);
}

HRESULT CGameInstance::SetVolume(const wstring& strSoundTag, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->SetVolume(strSoundTag, fVolume);
}

HRESULT CGameInstance::SetPosition(const wstring& strSoundTag, _float fPosition)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->SetPosition(strSoundTag, fPosition);
}


void CGameInstance::Transform_Frustum_To_LocalSpace(_fmatrix matWorldMatrix)
{
	return m_pFrustum->Transform_ToLocalSpace(matWorldMatrix);
}

_bool CGameInstance::IsIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRange)
{
	return m_pFrustum->isIn_WorldSpace(vWorldPos, fRange);
}

_bool CGameInstance::IsIn_Frustum_LocalSpace(_fvector vWorldPos, _float fRange)
{
	return m_pFrustum->isIn_LocalSpace(vWorldPos, fRange);
}

HRESULT CGameInstance::Add_DebugComponent(CComponent* pComponent)
{
	return m_pRenderer->Add_DebugComponent(pComponent);
}

HRESULT CGameInstance::Ready_RTDebug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strRenderTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_RTDebug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Free();
	CGameInstance::Get_Instance()->Destroy_Instance();	
}

void CGameInstance::Free()
{	
	Safe_Release(m_pFrustum);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pPixelPicking);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pFmod_System);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pGraphic_Device);
}
