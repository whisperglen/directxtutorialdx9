
#include "helper.h"

static int do_nothing_func(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
{
	return 0;
}

render_func select_render_func(render_func storage[], int& selection)
{
	int i;

	render_func fcn = do_nothing_func;
	for (i = 0; storage[i] && (i <= selection || selection < 0); i++)
	{
		fcn = storage[i];
	}
	selection = i - 1;

	return fcn;
}

static union inputs store_press = { 0 };

void store_keypress(union inputs val)
{
	store_press = val;
}

union inputs get_keypress()
{
	union inputs ret = store_press;

	store_press.all = 0;

	return ret;
}