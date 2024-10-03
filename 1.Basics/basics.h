#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>

#include <iostream>
#include <stdint.h>

#include "helper.h"

#define RSRCDIR ".\\1.Basics\\"
#define RSRC(A) RSRCDIR##A

typedef int (*render_func)(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);

int first(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);
int triangle(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);
int triangle3d(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);

#ifdef BASICS_DATA

static render_func basics_ptrs[] =
{
	first,
	triangle,
	triangle3d,
	0
};

#endif