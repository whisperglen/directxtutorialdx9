#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include "d3dx9.h"

#include <iostream>
#include <stdint.h>

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

typedef int(*render_func)(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);

render_func select_render_func(render_func storage[], int& selection);