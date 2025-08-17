#pragma once

#include <process.h>

namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_MAIN, LEVEL_GAMEPLAY, LEVEL_PUZZLE, LEVEL_BOSS, LEVEL_STATIC2, LEVEL_END };

	const unsigned int IDLE = 0x00000001;
	const unsigned int WALK = 0x00000002;
	const unsigned int ATT = 0x00000004; 

	const unsigned int BURROW = 0x00000008; 
	const unsigned int BURROWWALK = 0x00000010;

	const unsigned int JUMP = 0x00000020; 
	const unsigned int JUMPING = 0x00000040;

	const unsigned int DIVE = 0x00000080; 
	
	const unsigned int JUMPATT = 0x00000100;

	const unsigned int TURN_L = 0x00000200;
	const unsigned int TURN_R = 0x00000400;

	const unsigned int PICKUP = 0x00000800; 
	const unsigned int CARRY = 0x00001000; 
}

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

#include <ImGuizmo/ImGuizmo.h>
#include <ImGuizmo/GraphEditor.h>
#include <ImGuizmo/ImSequencer.h>
#include <ImGuizmo/ImZoomSlider.h>
#include <ImGuizmo/ImGradient.h>
#include <ImGuizmo/ImCurveEdit.h>

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

#include "Client_Macro.h"
#include <iostream>

using namespace Client;
