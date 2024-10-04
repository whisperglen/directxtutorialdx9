#pragma once

#include "helper.h"

#define RSRCDIR ".\\1.Basics\\"
#define RSRC(A) RSRCDIR##A

int first(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);
int triangle(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);
int triangle3d(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);
int trianglefly(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);
int trianglepsychedelic(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);
int triangledepth(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);
int cube(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);

#ifdef BASICS_DATA

static render_func basics_ptrs[] =
{
	first,
	triangle,
	triangle3d,
	trianglefly,
	trianglepsychedelic,
	triangledepth,
	cube,
	0
};

#endif