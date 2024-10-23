
#include "helper.h"

Demo *last_demo = 0;
int last_selection = 0;

struct dx_state dx_state1;

static union inputs store_press = { 0 };

void clear_last_demo()
{
	if (last_demo)
	{
		delete last_demo;
		last_demo = 0;
	}
}

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

int msg_func(const char *one, const char *two)
{
	return strlen(one) + strlen(two);
}
