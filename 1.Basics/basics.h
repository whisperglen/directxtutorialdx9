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
namespace cube { int cube(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device); }
namespace pyramid { int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device); }
namespace pyramid { int spawn(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device); }
namespace vertexlight { int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device); }
namespace pointlight { int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device); }
namespace spotlight { int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device); }
namespace lightbox { int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device); }
namespace normalsgen { int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device); }

#ifdef BASICS_DATA

static render_func basics_ptrs[] =
{
	first,
	triangle,
	triangle3d,
	trianglefly,
	trianglepsychedelic,
	triangledepth,
	cube::cube,
	pyramid::loop,
	pyramid::spawn,
	vertexlight::loop,
	pointlight::loop,
	spotlight::loop,
	lightbox::loop,
	normalsgen::loop,
	0
};

#endif