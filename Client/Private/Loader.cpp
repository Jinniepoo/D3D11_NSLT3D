#include "stdafx.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Loader_Defines.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

unsigned int APIENTRY Loading_Main(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

	return pLoader->Loading();
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

unsigned int CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, 0);

	HRESULT		hr{};

	switch (m_eNextLevelID)
	{
	case LEVEL_MAIN:
		hr = Loading_MainLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlay_Level();
		break;
	case LEVEL_PUZZLE:
		hr = Loading_Puzzle();
		break;
	case LEVEL_BOSS:
		hr = Loading_Boss();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return 1;

	return 0;
}

HRESULT CLoader::Loading_MainLevel()
{
	lstrcpy(m_szLoadingText, TEXT("Loading Textures.."));

	/**********************
	| PROTOTYPE COMPONENTS |
	***********************/

	/* Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAIN, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_Main/TitleScreen_BG%d.dds"), 3))))
		return E_FAIL;

	/* Prototype_Component_Texture_Main_NSLT */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAIN, TEXT("Prototype_Component_Texture_Main_NSLT"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_Main/Main_NSLT%d.dds"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Models.."));

	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	lstrcpy(m_szLoadingText, TEXT("Loading Sounds.."));

	/* BGM */
	ADD_SOUND(L"BGM_Main", L"../Bin/Resources/Sounds/Main/BGM_Main.ogg");

	lstrcpy(m_szLoadingText, TEXT("Loading Shaders.."));

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAIN, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Prototypes.."));

	/************************
	| PROTOTYPE GAME OBJECTS |
	*************************/

	/* Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CLoading_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Main_NSLT */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Main_NSLT"),
		CMain_NSLT::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("< Loading Complete >"));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Loading Textures.."));

	/**********************
	| PROTOTYPE COMPONENTS |
	***********************/

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile1.jpg"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;

	/* Blend Effect*/

	/* For.Prototype_Component_Effect_Checkpoint */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Checkpoint"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect_Checkpoint/Checkpoint%d.dds"), 16))))
		return E_FAIL;

	/* For.Prototype_Component_Effect_Startle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Startle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect_Startle/Startle%d.dds"), 17))))
		return E_FAIL;

	/* For.Prototype_Component_Effect_Dizzy */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Dizzy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect_Dizzy/Dizzy%d.dds"), 21))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Models.."));

	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 120, 50))))
		return E_FAIL;

	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model_Spring_Mushroom */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Spring_Mushroom"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AnimModels/Red_Spring/Visual.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Monster_Wriggle */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Wriggle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Wriggle/WriggleRev02.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Monster_CrumbleR */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_CrumbleR"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AnimModels/Crumble_Red/Crumble02.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Monster_CrumbleB */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_CrumbleB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AnimModels/Crumble_Black/Crumble02.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Player */
	LocalTransformMatrix = XMMatrixTranslation(-0.4f, 0.2f, 1.8f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player2/LuckyRev03.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Player_Burrow */
	LocalTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Burrow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/LuckyRev03.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Anim_GolemHead */
	_matrix TranslationMatrix = XMMatrixTranslation(3.6f, -2.5f, -4.6f);
	_matrix RotationYMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	LocalTransformMatrix = TranslationMatrix * RotationYMatrix;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim_GolemHead"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AnimModels/GolemBabyHead01/GolemBabyHead01.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Anim_Checkpoint */
	LocalTransformMatrix = XMMatrixTranslation(321.2f, 0.f, 1.68f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim_Checkpoint"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AnimModels/Checkpoint/Visuals.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Anim_FoxHole */
	LocalTransformMatrix = XMMatrixTranslation(403.6f, -10.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim_FoxHole"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AnimModels/Foxhole/Foxhole.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Anim_BabyGolem */
	LocalTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Anim_BabyGolem"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AnimModels/BabyGolem/GolemBaby.fbx", LocalTransformMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_Explosion */
	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};
	InstanceDesc.iNumInstance = 100;
	InstanceDesc.vScale = _float2(0.2f, 0.7f);
	InstanceDesc.vCenter = _float3(0.f, 00.0f, 0.f);
	InstanceDesc.vRange = _float3(1.f, 1.f, 1.f);
	InstanceDesc.vSpeed = _float2(1.f, 2.f);
	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vLifeTime = _float2(1.f, 3.f);
	InstanceDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_Snow */
	InstanceDesc.iNumInstance = 3000;
	InstanceDesc.vScale = _float2(0.2f, 0.5f);
	InstanceDesc.vCenter = _float3(64.f, 20.0f, 64.f);
	InstanceDesc.vRange = _float3(129.f, 1.f, 129.f);
	InstanceDesc.vSpeed = _float2(1.f, 2.f);
	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vLifeTime = _float2(4.f, 10.f);
	InstanceDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Particle_Log */
	//CVIBuffer_Instance::INSTANCE_DESC MeshInstanceDesc{};
	//MeshInstanceDesc.iNumInstance = 100; // Number of instances
	//MeshInstanceDesc.vScale = _float2(0.2f, 0.7f); // Scale range
	//MeshInstanceDesc.vCenter = _float3(0.f, 0.f, 0.f); // Center position
	//MeshInstanceDesc.vRange = _float3(1.f, 1.f, 1.f); // Range for random position
	//MeshInstanceDesc.vSpeed = _float2(1.f, 2.f); // Speed range
	//MeshInstanceDesc.vPivot = _float3(0.f, 0.f, 0.f); // Pivot point for instance spreading
	//MeshInstanceDesc.vLifeTime = _float2(1.f, 3.f); // Lifetime range
	//MeshInstanceDesc.isLoop = false; // Loop flag

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Log"),
	//	CVIBuffer_Mesh_Instance::Create(m_pDevice, m_pContext, &MeshInstanceDesc))))
	//	return E_FAIL;

	/* Map Objects */

	/* Prototype_Component_Model_MazeDoor */
	LocalTransformMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MazeDoorL"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AnimModels/TutorialDoor_L/TutorialDoor.fbx", LocalTransformMatrix))))
		return E_FAIL;

	LocalTransformMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MazeDoorR"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AnimModels/TutorialDoor_R/TutorialDoor.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_MazeBell */
	LocalTransformMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MazeBell"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MapObjects/Map_MazeBell/Visuals.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_MovingWall */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MovingWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MapObjects/Floor_GateHeightFloor/Visuals.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_WoodenCrate_Triple */
	LocalTransformMatrix = XMMatrixRotationX(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WoodenCrate_Triple"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MapObjects/WoodenCrate_Triple/Visuals.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_WoodenCrate_Single */
	LocalTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WoodenCrate_Single"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MapObjects/WoodenCrate_Single/Visuals.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_WoodenCrate_Heart */
	LocalTransformMatrix = XMMatrixRotationX(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WoodenCrate_Heart"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MapObjects/WoodenCrate_Heart/Visuals.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_NoHead1 */
	LocalTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NoHead1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AnimModels/BabyGolem_Body1/Visuals.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_NoHead2 */
	LocalTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NoHead2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AnimModels/BabyGolem_Body2/Visuals.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_NoHead3 */
	LocalTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NoHead3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AnimModels/BabyGolem_Body3/Visuals.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Particle_Log */
	LocalTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Particle_Log"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Effects/Particle_Log/Visuals.fbx", LocalTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Loading Sounds.."));

	/************************
	|     SOUND RESOURCES    |
	*************************/

	/* BGM */
	ADD_SOUND(L"BGM_SkyCastle", L"../Bin/Resources/Sounds/SkyCastle/BGM_SkyCastle.ogg");

	/* Player */
	ADD_SOUND(L"Player_Attack", L"../Bin/Resources/Sounds/Player/Player_Attack.mp3");
	ADD_SOUND(L"Player_Attack2", L"../Bin/Resources/Sounds/Player/Player_Attack2.mp3");
	ADD_SOUND(L"Player_IntenseAttack", L"../Bin/Resources/Sounds/Player/Player_IntenseAttack.mp3");
	ADD_SOUND(L"Player_TailSwoosh", L"../Bin/Resources/Sounds/Player/Player_TailSwoosh.mp3");
	ADD_SOUND(L"Player_TailSwoosh2", L"../Bin/Resources/Sounds/Player/Player_TailSwoosh2.mp3");

	ADD_SOUND(L"Player_Jump", L"../Bin/Resources/Sounds/Player/Player_Jump.mp3");
	ADD_SOUND(L"Player_JumpAgain", L"../Bin/Resources/Sounds/Player/Player_JumpAgain.mp3");
	ADD_SOUND(L"Player_JumpAir", L"../Bin/Resources/Sounds/Player/Player_JumpAir.mp3");
	ADD_SOUND(L"Player_DoubleJump", L"../Bin/Resources/Sounds/Player/Player_DoubleJump.mp3");

	ADD_SOUND(L"Player_CarryingGolem", L"../Bin/Resources/Sounds/Player/Player_CarryingGolem.mp3");
	ADD_SOUND(L"Player_PickGolem", L"../Bin/Resources/Sounds/Player/Player_PickGolem.mp3");

	ADD_SOUND(L"Player_FootStep", L"../Bin/Resources/Sounds/Player/Player_FootStep.wav");

	/* Monster */
	ADD_SOUND(L"Monster_Hurt", L"../Bin/Resources/Sounds/Monster/Monster_Hurt.mp3");
	ADD_SOUND(L"Monster_Airborne", L"../Bin/Resources/Sounds/Monster/Monster_Airborne.mp3");

	/* Sound Effects */
	ADD_SOUND(L"Coin_Loot2", L"../Bin/Resources/Sounds/SoundEffects/Coin_Loot2.mp3");
	ADD_SOUND(L"Coin_Loot3", L"../Bin/Resources/Sounds/SoundEffects/Coin_Loot3.mp3");
	ADD_SOUND(L"Coin_Score", L"../Bin/Resources/Sounds/SoundEffects/Coin_Score.mp3");
	ADD_SOUND(L"Clover_Gained", L"../Bin/Resources/Sounds/SoundEffects/Clover_Gained.ogg");
	ADD_SOUND(L"To_Puzzle", L"../Bin/Resources/Sounds/SoundEffects/ToPuzzle.ogg");
	ADD_SOUND(L"Spring", L"../Bin/Resources/Sounds/SoundEffects/Spring.mp3");
	ADD_SOUND(L"Door1", L"../Bin/Resources/Sounds/SoundEffects/DoorOpen1.ogg");
	ADD_SOUND(L"Door2", L"../Bin/Resources/Sounds/SoundEffects/DoorOpen.ogg");
	ADD_SOUND(L"MovingWall", L"../Bin/Resources/Sounds/SoundEffects/MovingWall.ogg");
	ADD_SOUND(L"BellRing", L"../Bin/Resources/Sounds/SoundEffects/BellRing.mp3");

	/* Golems */
	ADD_SOUND(L"Golem_GainedHead", L"../Bin/Resources/Sounds/BabyGolems/Golem_GainedHead.mp3");
	ADD_SOUND(L"Golem_LastTalk", L"../Bin/Resources/Sounds/BabyGolems/Golem_LastTalk.mp3");
	ADD_SOUND(L"Golem_LongTalk", L"../Bin/Resources/Sounds/BabyGolems/Golem_LongTalk.mp3");
	ADD_SOUND(L"Golem_ShortTalk", L"../Bin/Resources/Sounds/BabyGolems/Golem_ShortTalk.mp3");

	lstrcpy(m_szLoadingText, TEXT("Loading Shaders.."));

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	///* For.Prototype_Component_Shader_VtxMeshInstance */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshInstance.hlsl"), VTXMESHINSTANCE::Elements, VTXMESHINSTANCE::iNumElements))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Navigation.."));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/DataFiles/SkyCastle_Nav.dat")))))
		return E_FAIL;

	/************************
	| PROTOTYPE GAME OBJECTS |
	*************************/

	lstrcpy(m_szLoadingText, TEXT("Loading Prototypes.."));

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Burrow_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Burrow_Player"),
		CBurrow_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_GolemHead */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GolemHead"),
		CGolemHead::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_CrumbleR */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_CrumbleR"),
		CMonster_CrumbleR::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_CrumbleB */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_CrumbleB"),
		CMonster_CrumbleB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Map Objects */
	/* For.Prototype_GameObject_MazeDoor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MazeDoorL"),
		CMazeDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MazeDoorR"),
		CMazeDoorR::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_MazeBell */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MazeBell"),
		CMazeBell::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_MovingWall */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MovingWall"),
		CMovingWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Spring_Mushroom */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spring_Mushroom"),
		CSpring_Mushroom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_WoodenCrate_Single */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WoodenCrate_Single"),
		CWoodenCrate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_WoodenCrate_Triple */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WoodenCrate_Triple"),
		CWoodenCrate_Triple::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_WoodenCrate_Heart */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WoodenCrate_Heart"),
		CWoodenCrate_Heart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_NoHead1 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NoHead1"),
		CNoHead1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_NoHead2 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NoHead2"),
		CNoHead2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_NoHead3 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NoHead3"),
		CNoHead3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_Checkpoint */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Checkpoint"),
		CEffect_Checkpoint::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_Startle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Startle"),
		CEffect_Startle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_Dizzy */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Dizzy"),
		CEffect_Dizzy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle_Log */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Particle_Log",
		CParticle_Log::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Explosion"),
		CParticle_Explosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Snow"),
		CParticle_Snow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Anim_Checkpoint */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Anim_Checkpoint"),
		CCheckpoint::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Anim_GolemHead */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Anim_GolemHead"),
		CAnim_GolemHead::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Anim_FoxHole */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Anim_FoxHole"),
		CAnim_Foxhole::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Anim_NPC_Guard */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Anim_NPC_Guard"),
	//	CAnim_Guard::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Anim_BabyGolem */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Anim_BabyGolem"),
		CAnim_BabyGolem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("< Loading Complete >"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Puzzle()
{
	lstrcpy(m_szLoadingText, TEXT("Loading Textures.."));

	/**********************
	| PROTOTYPE COMPONENTS |
	***********************/

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile1.jpg"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Effect_Checkpoint */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Effect_Checkpoint"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect_Checkpoint/Checkpoint%d.dds"), 16))))
		return E_FAIL;

	/* For.Prototype_Component_Effect_Startle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Effect_Startle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect_Startle/Startle%d.dds"), 17))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_LuckyLetters */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Texture_LuckyLetters"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_Puzzle/LuckyLetters%d.dds"), 18))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_LuckyComplete */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Texture_LuckyComplete"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Level_Puzzle/LuckyComplete%d.dds"), 5))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Models.."));

	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 40, 30))))
		//CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model_Player */
	LocalTransformMatrix = XMMatrixTranslation(-0.4f, 0.2f, 1.8f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player2/LuckyRev03.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Player_Burrow */
	LocalTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Model_Player_Burrow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/LuckyRev03.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Anim_GolemHead */
	_matrix TranslationMatrix = XMMatrixTranslation(3.6f, -2.5f, -4.6f);
	_matrix RotationYMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	LocalTransformMatrix = TranslationMatrix * RotationYMatrix;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Model_Anim_GolemHead"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AnimModels/GolemBabyHead01/GolemBabyHead01.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Anim_FoxHole */
	LocalTransformMatrix = XMMatrixTranslation(403.6f, -10.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Model_Anim_FoxHole"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/AnimModels/Foxhole/Foxhole.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Map Objects */

	///* Prototype_Component_Model_MazeDoor */
	//LocalTransformMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Model_MazeDoorL"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AnimModels/TutorialDoor_L/TutorialDoor.fbx", LocalTransformMatrix))))
	//	return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Loading Sounds.."));

	/* BGM */
	//ADD_SOUND(L"BGM_Puzzle", L"../Bin/Resources/Sounds/Puzzle/BGM_Puzzle.ogg");
	ADD_SOUND(L"BGM_Puzzle", L"../Bin/Resources/Sounds/Puzzle/BGM_Puzzle1.ogg");

	/* SoundEffects */
	ADD_SOUND(L"Lucky_Gained", L"../Bin/Resources/Sounds/SoundEffects/StageClear.ogg");


	lstrcpy(m_szLoadingText, TEXT("Loading Shaders.."));

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Navigation.."));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_PUZZLE, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/DataFiles/Puzzle_Nav1.dat")))))
		return E_FAIL;

	/************************
	| PROTOTYPE GAME OBJECTS |
	*************************/

	lstrcpy(m_szLoadingText, TEXT("Loading Prototypes.."));

	/* For.Prototype_GameObject_LuckyComplete */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LuckyComplete"),
		CUI_LuckyComplete::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LuckyLetters */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LuckyLetters"),
		CUI_LuckyLetters::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, L"< Loading Complete >");

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Boss()
{
	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}