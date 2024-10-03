
#define BASICS_DATA
#include "basics.h"

int call_loop_unit1(int& selection, LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
{
	int retcode = 0;

	render_func fcn = first;
	for (int i = 0; basics_ptrs[i] && i <= selection; i++)
	{
		fcn = basics_ptrs[i];
	}

	retcode = fcn(d3d, device);

	return retcode;
}

int first(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
{
	// clear the window to a deep blue
	device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);

	device->BeginScene();    // begins the 3D scene

	// do 3D rendering on the back buffer here

	device->EndScene();    // ends the 3D scene

	device->Present(NULL, NULL, NULL, NULL);    // displays the created frame

	return 0;
}