#pragma once
// System includes
#include <Windows.h>

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


// assimp 
#include <cimport.h>
#include <postprocess.h>
#include <scene.h>

#pragma comment(lib, "assimp-vc140-mt.lib")

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



