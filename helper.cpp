
#include "helper.h"

Demo *last_demo = 0;
int last_selection = 0;

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