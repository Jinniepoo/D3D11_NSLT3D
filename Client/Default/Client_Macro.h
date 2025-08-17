#pragma once

#define FIND_TRANSFORM(Obj,ComponentTag) static_cast<CTransform*>(Obj->Find_Component(ComponentTag))
#define GET_LIST(Level_Index,LayerTag) m_pGameInstance->Get_List(m_pGameInstance->Find_Layer(Level_Index, LayerTag))

#define GET_CURLEVEL (LEVEL)m_pGameInstance->Get_CurrentLevel()->Get_LevelID_For_Init()
#define GET_LAYER(Level_Index, LayerTag) m_pGameInstance->Find_Layer(Level_Index,LayerTag )
#define GET_PIPELINE(TransformTag) *(m_pGameInstance->Get_Transform_Float4x4(TransformTag))
#define ADD_SOUND(SoundTag, FilePath) m_pGameInstance->Add_Sound(SoundTag,FilePath)
#define PLAY(SoundTag,Loop) m_pGameInstance->Play(SoundTag,Loop)
#define PLAYONLY(SoundTag) m_pGameInstance->PlayOnly(SoundTag)
#define PLAYTOBGM(SoundTag,Loop) m_pGameInstance->Play(SoundTag,Loop)
#define STOP(SoundTag)	m_pGameInstance->Stop(SoundTag)
#define VOLUME(SoundTag, Volume) m_pGameInstance->SetVolume(SoundTag,Volume)

#define WIREMAXLEN		20.f