#pragma once

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define BEGIN(NAMESPACE) namespace NAMESPACE { 
#define END }
#define MAX_CHANNEL 128

/* Input Device */
#define KEY_NONE(KEY) CInput_Device::NONE == m_pGameInstance->Get_DIKState(KEY)
#define KEY_PUSH(KEY) CInput_Device::PUSH == m_pGameInstance->Get_DIKState(KEY)
#define KEY_HOLD(KEY) CInput_Device::HOLD == m_pGameInstance->Get_DIKState(KEY)
#define KEY_AWAY(KEY) CInput_Device::AWAY == m_pGameInstance->Get_DIKState(KEY)

#define MOUSE_NONE(BUTTON) CInput_Device::NONE == m_pGameInstance->Get_DIMKeyState(BUTTON)
#define MOUSE_PUSH(BUTTON) CInput_Device::PUSH == m_pGameInstance->Get_DIMKeyState(BUTTON)
#define MOUSE_HOLD(BUTTON) CInput_Device::HOLD == m_pGameInstance->Get_DIMKeyState(BUTTON)
#define MOUSE_AWAY(BUTTON) CInput_Device::AWAY == m_pGameInstance->Get_DIMKeyState(BUTTON)

#define MOUSE_MOVE_X(Mouse) Mouse =  m_pGameInstance->Get_DIMMoveState(DIMM_X)
#define MOUSE_MOVE_Y(Mouse)	Mouse =  m_pGameInstance->Get_DIMMoveState(DIMM_Y)

#define MSG_BOX(message)								\
::MessageBox(nullptr, message, L"System Message", MB_OK)

#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static ClassName* Get_Instance();					\
	static void		  Destroy_Instance();				\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
void ClassName::Destroy_Instance()						\
{														\
	if(nullptr != m_pInstance)							\
	{													\
		delete m_pInstance;								\
		m_pInstance = nullptr;							\
	}													\
}
