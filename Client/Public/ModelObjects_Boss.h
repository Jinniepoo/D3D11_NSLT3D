#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
END

BEGIN(Client)
class CModelObjects_Boss : public CGameObject
{
public:
	using MODEL_DESC = struct tagModelDesc
	{
		_float4x4	matModel = {};
		_char		strName[256] = {};
		_char		strNumbered[256] = {};
	};

private:
	CModelObjects_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelObjects_Boss(const CModelObjects_Boss& rhs);
	virtual ~CModelObjects_Boss() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* strComponentTag);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void	Save_ModelData(HANDLE hFile, _ulong& dwByte); 

	/* Convert */
	wstring CModelObjects_Boss::CharToWString(_char* szSrc, wstring& strDest);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel*  m_pModelCom = { nullptr };
	class CGameObject* m_pCameraPlayer = { nullptr };
	CTransform* m_pCamera = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	MODEL_DESC m_ModelDesc;
	_tchar	   m_strComponentTag[MAX_PATH] = {L""};

public:
	_vector	   m_vCameraPlayerPos;
	_bool	   m_bNearCam = { false };

public:
	static CModelObjects_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* strComponentTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
