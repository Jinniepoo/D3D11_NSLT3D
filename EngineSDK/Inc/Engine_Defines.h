#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 4099)



#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

using namespace Assimp;

#include <Fx11\d3dx11effect.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>

#include <DirectXTK\VertexTypes.h>
#include <DirectXTK\PrimitiveBatch.h>
#include <DirectXTK\Effects.h>

#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>
#include <DirectXTK\ScreenGrab.h>

using namespace DirectX;
using namespace std;
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <commdlg.h>
#include <iostream>

/* fmod*/
#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod.h>

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace Engine;