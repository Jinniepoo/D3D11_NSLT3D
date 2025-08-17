#pragma once

#include "Renderer.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(_uint iNumLevels, HINSTANCE hInst, const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear(_uint iLevelIndex);

public:
	/* Graphic Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();

public:
	/* Input Device */
	CInput_Device::INPUT_STATE Get_DIKState(_ubyte eKeyID);
	CInput_Device::INPUT_STATE Get_DIMKeyState(DIMK eMouseKeyID);
	_long Get_DIMMoveState(DIMM eMouseMoveID);


public:
	/* Level Manager */
	HRESULT Change_Level(class CLevel* pNewLevel);
	class CLevel* Get_CurrentLevel();

public:
	/* Timer Manager */
	_float	Get_TimeDelta(const wstring& strTimerTag);
	HRESULT Add_Timer(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);

public:
	/* Object Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	list <class CGameObject*>* Get_List(class CLayer* Layer);
	void Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag);

public:
	/* Component Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public:
	/* Renderer */
	HRESULT Add_RenderObjects(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);

public:
	/* Pipeline */
	void Set_Transform(CPipeLine::TRANSFORMATIONSTATE eState, _fmatrix TransformationMatrix);
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORMATIONSTATE eState) const;
	const _float4x4* Get_Transform_Float4x4(CPipeLine::TRANSFORMATIONSTATE eState) const;
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMATIONSTATE eState) const;
	const _float4x4* Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMATIONSTATE eState) const;
	_vector Get_CamPosition_Vector() const;
	const _float4* Get_CamPosition_Float4() const;

public:
	/* Font Manager */
	HRESULT Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath);
	HRESULT Render_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fRotation = 0.f, _float fScale = 1.f);
	HRESULT Render_FontOutlined(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor1 = XMVectorSet(0.f, 0.f, 0.f, 1.f), _fvector vColor2 = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, _float fScale = 1.f);
public:
	/* Light Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

	/* Picking */
	class CPicking* Get_Picking() { return m_pPicking; }
	
	/* Pixel Picking */
	_float4 Get_PickedPos();

	/* Target Manager */
	HRESULT Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool isClear = true);
	HRESULT End_MRT();
	HRESULT Bind_RT_SRV(const _wstring& strRenderTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RT_Resource(const _wstring& strRenderTargetTag, ID3D11Texture2D* pResource);

	/* Sound Manager */
	HRESULT	Add_Sound(const wstring& strTag, const wstring& strPath);
	HRESULT Play(const wstring& strSoundTag, _bool bLoop = false);
	HRESULT	PlayOnly(const wstring& strSoundTag);
	HRESULT PlayToBgm(const wstring& strSoundTag, _bool bLoop = true);
	HRESULT Stop(const wstring& strSoundTag);
	HRESULT Paused(const wstring& strSoundTag, _bool bPause = true);
	HRESULT SetVolume(const wstring& strSoundTag, _float fVolume);
	HRESULT SetPosition(const wstring& strSoundTag, _float fPosition);

	/* Frustrum */
	void	Transform_Frustum_To_LocalSpace(_fmatrix matWorldMatrix);
	_bool	IsIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRange);
	_bool	IsIn_Frustum_LocalSpace(_fvector vWorldPos, _float fRange);

#ifdef _DEBUG
public:
	HRESULT	Add_DebugComponent(class CComponent* pComponent);
	HRESULT Ready_RTDebug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RTDebug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };	
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CPixelPicking*			m_pPixelPicking = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CFmod_System*				m_pFmod_System = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END
