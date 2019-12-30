#pragma once

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

//==============================
//   ä÷êîêÈåæ
//==============================
bool InitD3D(HWND hWnd);
void UninitD3D(void);
LPDIRECT3DDEVICE9 GetD3DDevice();
