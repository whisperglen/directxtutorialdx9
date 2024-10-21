
#include "shaders.h"

Demo *factory_pass_shaders_create();

static void do_selection(Demo* &demo, int & selection)
{
	switch (selection)
	{
	default:
		selection = 0;
	case 0:
		demo = factory_pass_shaders_create();
		break;

	}
}

int call_loop_unit2(int& selection, LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
{
	int retcode = -100;

	if (last_demo != 0 && last_selection == selection)
	{
		retcode = last_demo->loop(d3d, device);
	}
	else
	{
		if (last_demo)
		{
			delete last_demo;
			last_demo = 0;
		}

		do_selection(last_demo, selection);

		if (last_demo)
		{
			retcode = last_demo->loop(d3d, device);
			last_selection = selection;
		}
	}

	return retcode;
}