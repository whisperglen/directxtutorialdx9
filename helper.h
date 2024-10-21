#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include "d3dx9.h"

#include <iostream>
#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <tchar.h>

#include "DemoClass.h"

typedef unsigned int uint;

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600


extern Demo *last_demo;
extern int last_selection;


struct mesh_obj
{
	LPDIRECT3DVERTEXBUFFER9 v_buffer;
	LPDIRECT3DINDEXBUFFER9 i_buffer;
	int numvert, numprim;
	D3DMATERIAL9 material;
};

int load_mesh(const char *file, const char *name, LPDIRECT3DDEVICE9 device, struct mesh_obj &out);

/**
 * A hacky way to store keypresses
 */
union inputs
{
	struct {
		int horiz : 2;
		int vert : 2;
		int ctrl : 1;
		uint t : 1;
		uint f : 1;
	};
	uint all;
};

void store_keypress(union inputs val);

union inputs get_keypress();