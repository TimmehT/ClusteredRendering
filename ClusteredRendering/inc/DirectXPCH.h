#pragma once
// System includes
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>
#include <Shlwapi.h>
#include <comdef.h>
#include <mmsystem.h>
#include <wrl.h>

// DirectX includes
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// STL includes
#include <locale>
#include <codecvt>
#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <map>
#include <ctime>
#include <random>
#include <atomic>
#include <mutex>
#include <thread>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Shlwapi.lib")


// assimp 
#include <DefaultLogger.hpp>
#include <Importer.hpp>
#include <Exporter.hpp>
#include <ProgressHandler.hpp>
#include <scene.h>
#include <postprocess.h>
#include <mesh.h>
#include <importerdesc.h>

#pragma comment(lib, "assimp-vc140-mt.lib")

// DirectXTex
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

using namespace DirectX;

// Safaely release a COM object
template<typename T>
inline void SafeRelease(T& ptr)
{
	if (ptr != NULL)
	{
		ptr->Release();
		ptr = NULL;
	}
}

// Safely delte array
template<typename T>
inline void SafeDeleteArray(T& ptr)
{
	if (ptr != NULL)
	{
		delete[] ptr;
		ptr = NULL;
	}
}

// Safely delte pointer
template<typename T>
inline void SafeDelete(T& ptr)
{
	if (ptr != NULL)
	{
		delete ptr;
		ptr = NULL;
	}
}

namespace EngineMath
{
	inline XMVECTOR FloatToVector(XMFLOAT3& val)
	{
		return XMLoadFloat3(&val);
	}

	inline XMFLOAT3 VectorToFloat(XMVECTOR& vec)
	{
		XMFLOAT3 val;
		XMStoreFloat3(&val, vec);
		return val;
	}

	inline XMMATRIX Float4X4ToMatrix(XMFLOAT4X4& val)
	{
		return XMLoadFloat4x4(&val);
	}

	inline XMFLOAT4X4 MatrixToFLoat4X4(XMMATRIX& mat)
	{
		XMFLOAT4X4 val;
		XMStoreFloat4x4(&val, mat);
		return val;
	}
}


#define VK_W	0x57
#define VK_A	0x41
#define VK_S	0x53
#define VK_D	0x44




