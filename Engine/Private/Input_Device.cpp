#include "..\Public\Input_Device.h"

CInput_Device::CInput_Device()
{

}

HRESULT CInput_Device::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	m_hWnd = hWnd;

	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, NULL)))
		return E_FAIL;

	if (FAILED((m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))))
		return E_FAIL;

	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyBoard->Acquire();

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Device::Update_KeyBoardInfo()
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);

	HWND hFocus = GetFocus();

	if (hFocus != m_hWnd)
	{
		for (_uint i = 0; i < 256; ++i)
		{
			if (HOLD == m_eKeyState[i] || PUSH == m_eKeyState[i])
				m_eKeyState[i] = AWAY;

			else if (AWAY == m_eKeyState[i])
				m_eKeyState[i] = NONE;

			else 
				m_eKeyState[i] = NONE;
		}
	}

	if (hFocus == m_hWnd)
	{
		for (_uint i = 0; i < 256; ++i)
		{
			// [O] Key Pressed 
			if (m_byKeyState[i] & 0x80)
			{
				// [O - X] Prev : Not Pressed
				if (NONE == m_eKeyState[i] || AWAY == m_eKeyState[i])
					m_eKeyState[i] = PUSH;

				// [O - O] Prev : Pressed
				else if (PUSH == m_eKeyState[i])
					m_eKeyState[i] = HOLD;
			}
			else// [X] Key Not Pressed 
			{
				// [O - X] Prev : Pressed
				if (PUSH == m_eKeyState[i] || HOLD == m_eKeyState[i])
					m_eKeyState[i] = AWAY;
				// [X - X] Prev : Not Pressed
				else if (AWAY == m_eKeyState[i])
					m_eKeyState[i] = NONE;
				else
					m_eKeyState[i] = NONE;
			}
		}
	}

	return;
}

void CInput_Device::Update_MouseInfo()
{
	HWND hFocus = GetFocus();

	if (hFocus != m_hWnd)
	{
		for (_uint i = 0; i < DIMK_END; ++i)
		{
			if (HOLD == m_eMouseState[i] || PUSH == m_eMouseState[i])
			{
				m_eMouseState[i] = AWAY;
			}

			else if (AWAY == m_eMouseState[i])
			{
				m_eMouseState[i] = NONE;
			}
		}
	}
	else if (hFocus == m_hWnd)
	{
		m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);
		for (_uint i = 0; i < DIMK_END; ++i)
		{
			// [O] Mouse Clicked 
			if (m_MouseState.rgbButtons[i] & 0x80)
			{
				// [O - X] Prev : Not Clicked
				if (NONE == m_eMouseState[i] || AWAY == m_eMouseState[i])
					m_eMouseState[i] = PUSH;

				// [O - O]Prev : Clicked
				else if (PUSH == m_eMouseState[i] || HOLD == m_eMouseState[i])
					m_eMouseState[i] = HOLD;
			}
			else //[X] Mouse Not Clicked
			{
				// Prev[X - O]: Clicked
				if (PUSH == m_eMouseState[i] || HOLD == m_eMouseState[i])
					m_eMouseState[i] = AWAY;
				// Prev[X - X]: Not Clicked
				else if (AWAY == m_eMouseState[i] || NONE == m_eMouseState[i])
					m_eMouseState[i] = NONE;
				else
					m_eMouseState[i] = NONE;
			}
		}
	}
	return;
}

void CInput_Device::Update_InputDev()
{
	Update_KeyBoardInfo();
	Update_MouseInfo();
}

CInput_Device* CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Ready_InputDev(hInst, hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CInput_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

