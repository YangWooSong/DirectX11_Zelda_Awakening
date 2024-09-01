#pragma once

#pragma warning (disable : 4251)
#pragma warning(disable : 5208) //����ü ��� ����
#pragma warning(disable : 6031) // ReadFile ��� ����

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
#include "DirectXTK\Effects.h" //PrimitiveBatch�� ����� �׷��ִ� �뵵

#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"

/* d3dx11�� ���ԵǾ��־����� �ʹ� ���̴�. */
/* ���� d3dx11���ԵǾ��־��� �پ��� ����� �����ϴ� ��� + ���̺귯�� ������ ���������� �߰��Ѵ�. */
//#include <d3dx11.h>

#include <map>
#include <unordered_map>
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


