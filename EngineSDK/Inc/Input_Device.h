#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
public:
	using INPUT_STATE = enum tagInputState
	{
		NONE, PUSH, HOLD, AWAY
	};

private:
	CInput_Device();
	virtual ~CInput_Device() = default;
	
public:
	INPUT_STATE Get_DIKState(_ubyte eKeyID) { return m_eKeyState[eKeyID]; }
	INPUT_STATE Get_DIMKeyState(DIMK eMouseKeyID) { return m_eMouseState[eMouseKeyID]; }
	_long		Get_DIMMoveState(DIMM eMouseMoveID) { return ((_long*)&m_MouseState)[eMouseMoveID]; }

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_KeyBoardInfo();
	void	Update_MouseInfo();
	void	Update_InputDev();

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

private:	
	_char					m_byKeyState[256] = { 0, };
	INPUT_STATE				m_eKeyState[256] = { (INPUT_STATE)0, };
	
	INPUT_STATE				m_eMouseState[DIMK_END] = { NONE };
	DIMOUSESTATE			m_MouseState = {};

	HWND					m_hWnd;

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
END