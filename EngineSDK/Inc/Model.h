#pragma once

#include "Component.h"
#include "Animation.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_bool Get_Finished() {
		return m_Animations[m_iCurrentAnimIndex]->Get_Finished();
	}

	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;

	void Set_AnimIndex(_uint iAnimIndex, _bool isLoop) {
		if (m_iCurrentAnimIndex == iAnimIndex)
			return;
		m_Animations[m_iCurrentAnimIndex]->Reset();
		m_iCurrentAnimIndex = iAnimIndex; 
		m_isLoop = isLoop;
	}

	_uint Get_AnimIndex() { return m_iCurrentAnimIndex; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_float fTimeDelta);
	HRESULT Bind_Buffers(_uint iMeshIndex);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

private:
	Assimp::Importer			m_Importer = {};
	const aiScene*				m_pAIScene = { nullptr };

private:
	TYPE							m_eModelType = { TYPE_END };
	_float4x4						m_TransformMatrix = {};

private:
	_uint							m_iNumMeshes = {};
	vector<class CMesh*>			m_Meshes;

private:
	_uint							m_iNumMaterials = {};
	vector<MATERIAL_MESH>			m_Materials;

private:	
	vector<class CBone*>			m_Bones;

private:
	_bool							m_isLoop = { false };
	_uint							m_iCurrentAnimIndex = {};
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pNode, _int iParentIndex);
	HRESULT Ready_Animations();


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
