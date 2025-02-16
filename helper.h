#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include "d3dx9.h"

#include <iostream>
#include <string>
#include <vector>

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <tchar.h>

#include "DemoClass.h"

typedef unsigned int uint;

#define xstr(a) str(a)
#define str(a) #a

#define RTN_ON_FAILURE(X) if(FAILED(X)) { return -1; }
#define MSG_ON_FAILURE(X) if(FAILED(X)) { msg_func(__FILE__, xstr(__LINE__)); }

struct dx_state
{
	LPDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 device;
	HINSTANCE happ;
	HWND hwindow;
	UINT width;
	UINT height;
	FLOAT aspect_ratio;
	bool windowed;
	bool vsync;
	bool wvisible;
	WINDOWPLACEMENT wplacement;
	D3DDISPLAYMODE desktop;
	D3DFORMAT fmt_backbuffer;
	D3DFORMAT fmt_depthstencil;
	D3DADAPTER_IDENTIFIER9 adapter_info;
	D3DCAPS9 caps;
	std::vector<D3DDISPLAYMODE> modes;
	dx_state() : d3d(NULL), device(NULL), happ(NULL), hwindow(NULL), width(0), height(0), aspect_ratio(0.0f),
	  windowed(false), vsync(false), wvisible(false), wplacement(), desktop(), modes(), fmt_backbuffer(), fmt_depthstencil(),
	  adapter_info(), caps()
	{
	};
};

extern struct dx_state dx_state1;
extern Demo *last_demo;
extern int last_selection;

void clear_last_demo();

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
		int shift : 1;
		uint t : 1;
		uint f : 1;
		uint v : 1;
		uint p : 1;
	};
	uint all;
};

void store_keypress(union inputs val);
union inputs get_keypress();

int msg_func(const char *one, const char *two);


template <class T>
constexpr auto& tmpshim(T&& tmpobj) noexcept {
	return tmpobj;
}