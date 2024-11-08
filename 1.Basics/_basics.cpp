
#include "basics.h"

Demo *factory_first_create();
Demo *factory_triangle_create();
Demo *factory_triangle3d_create();
Demo *factory_trianglefly_create();
Demo *factory_trianglepsychedelic_create();
Demo *factory_triangledepth_create();
Demo *factory_cube_create();
Demo *factory_pyramid_create();
Demo *factory_pyramidspawn_create();
Demo *factory_vertexlight_create();
Demo *factory_pointlight_create();
Demo *factory_spotlight_create();
Demo *factory_lightbox_create();
Demo *factory_normalsgen_create();
Demo *factory_blend_create();
Demo *factory_cubetrianglestrip_create();
Demo *factory_quad_create();

static void do_selection(Demo* &demo, int & selection)
{
	switch (selection)
	{
	case 0:
		demo = factory_first_create();
		break;
	case 1:
		demo = factory_triangle_create();
		break;
	case 2:
		demo = factory_triangle3d_create();
		break;
	case 3:
		demo = factory_trianglefly_create();
		break;
	case 4:
		demo = factory_trianglepsychedelic_create();
		break;
	case 5:
		demo = factory_triangledepth_create();
		break;
	case 6:
		demo = factory_cube_create();
		break;
	case 7:
		demo = factory_pyramid_create();
		break;
	case 8:
		demo = factory_pyramidspawn_create();
		break;
	case 9:
		demo = factory_vertexlight_create();
		break;
	case 10:
		demo = factory_pointlight_create();
		break;
	case 11:
		demo = factory_spotlight_create();
		break;
	case 12:
		demo = factory_lightbox_create();
		break;
	case 13:
		demo = factory_normalsgen_create();
		break;
	case 14:
		demo = factory_blend_create();
		break;
	case 15:
		demo = factory_cubetrianglestrip_create();
		break;
	default:
		selection = 16;
	case 16:
		demo = factory_quad_create();
		break;
	}
}

int call_loop_unit1(int& selection, LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
{
	int retcode = -100;

	if (last_demo != 0 && last_selection == selection)
	{
		retcode = last_demo->loop(d3d, device);
	}
	else
	{
		clear_last_demo();

		do_selection(last_demo, selection);

		if (last_demo)
		{
			retcode = last_demo->loop(d3d, device);
			last_selection = selection;
		}
	}

	return retcode;
}

class First : public Demo
{

public:
	First() : Demo() {}

	virtual int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == false)
			if (0 != do_init(d3d, device))
				return -1;

		// clear the window to a deep blue
		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);

		device->BeginScene();    // begins the 3D scene

		// do 3D rendering on the back buffer here

		device->EndScene();    // ends the 3D scene

		device->Present(NULL, NULL, NULL, NULL);    // displays the created frame

		return 0;
	}
};

Demo *factory_first_create()
{
	return new First();
}