
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
//#include <cassert>

#include "helper.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;

static int tests_total = 0;
static int tests_ok = 0;

static void xassert_str(int success, const char* expression, const char* function, unsigned line, const char* file)
{
	tests_total++;
	if (success) tests_ok++;
	else
	{
		const char* fn = strrchr(file, '\\');
		if (!fn) fn = strrchr(file, '/');
		if (!fn) fn = "fnf";

		cout << "assert failed: " << expression << " in " << function << ":" << line << " " << fn << endl;
	}
}

static void xassert_int(int success, int printval, const char* function, unsigned line, const char* file)
{
	tests_total++;
	if (success) tests_ok++;
	else
	{
		const char* fn = strrchr(file, '\\');
		if (!fn) fn = strrchr(file, '/');
		if (!fn) fn = "fnf";

		cout << "assert failed: " << std::hex << printval << std::dec << " in " << function << ":" << line << " " << fn << endl;
	}
}

#define assertx(expression) (void)(                                                             \
            (xassert_str((!!(expression)), _CRT_STRINGIZE(#expression), (__func__), (unsigned)(__LINE__), (__FILE__)), 0) \
        )


#define assertxiter(expression, iter) (void)(                                                             \
            (xassert_int((!!(expression)), (iter), (__func__), (unsigned)(__LINE__), (__FILE__)), 0) \
        )

static int do_selection(int selection)
{
	int ptrs[] =
	{
		1,
		2,
		3,
		0
	};

	int fcn = 1;
	for (int i = 0; ptrs[i] && i <= selection; i++)
	{
		fcn = ptrs[i];
	}

	return fcn;
}

static int test_selection()
{
	assertx(do_selection(-1) == 1);
	assertx(do_selection(0) == 1);
	assertx(do_selection(1) == 2);
	assertx(do_selection(2) == 3);
	assertx(do_selection(3) == 3);
	assertx(do_selection(100) == 3);

	return 0;
}

int run_tests()
{
	//test_selection();

	cout << "Tests result: " << tests_ok << "/" << tests_total << endl;
	return 0;
}