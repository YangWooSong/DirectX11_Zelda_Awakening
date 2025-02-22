#pragma once

#pragma warning (disable : 4251)
#pragma warning(disable : 5208) //구조체 경고 끄기
#pragma warning(disable : 6031) // ReadFile 경고 끄기

#include <d3d11.h>
#include <time.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Effects11/d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"

#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
using namespace DirectX;


#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\Effects.h" //PrimitiveBatch로 만든걸 그려주는 용도

#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"

/* d3dx11에 포함되어있었지만 너무 무겁다. */
/* 기존 d3dx11포함되어있었던 다양한 기능을 제공하는 헤더 + 라이브러리 파일을 선택적으로 추가한다. */
//#include <d3dx11.h>

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"

namespace Engine
{
	const _wstring		g_strTransformTag = TEXT("Com_Transform");
	const _uint			g_iMaxMeshBones = 512;
}

using namespace Engine;

#define TEXTURE_TYPE_MAX 18

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG


