#pragma once

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

//==============================
//   �֐��錾
//==============================
bool InitD3D(HWND hWnd);
void UninitD3D(void);
LPDIRECT3DDEVICE9 GetD3DDevice();
