
#include "textures.h"

Demo *factory_textured_triangle_create();

static void do_selection(Demo* &demo, int & selection)
{
	switch (selection)
	{
	default:
		selection = 0;
	case 0:
		demo = factory_textured_triangle_create();
		break;

	}
}

int call_loop_unit3(int& selection, LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
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