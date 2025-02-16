// directxtutorialdx9.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "helper.h"

#define WNDCLASS_NAME	TEXT("DXTut9WCls")
#define WINDOW_NAME		TEXT("directxtutorialdx9")
#define WSTYLE_WINDOWED		WS_OVERLAPPEDWINDOW //non-fullscreen
#define WSTYLE_FULLSCREEN	WS_EX_TOPMOST | WS_POPUP    // fullscreen values

#define WINDOW_XPOS		200
#define WINDOW_YPOS		200

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

extern int run_tests();
extern int call_loop_unit1(int& selection, LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);
extern int call_loop_unit2(int& selection, LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);
extern int call_loop_unit3(int& selection, LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);

static int selection = INT_MAX;
static int active_unit = INT_MAX;

int create_window();
void fill_in_d3dpresentparams(D3DPRESENT_PARAMETERS &d3dpp);

// this function initializes and prepares Direct3D for use
int initD3D(HWND, LPDIRECT3D9& d3d, LPDIRECT3DDEVICE9& device)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	const D3DFORMAT fmt_bb[] =
	{
		D3DFMT_A8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_R5G6B5,
		D3DFMT_X1R5G5B5,
		D3DFMT_A1R5G5B5,
		D3DFMT_UNKNOWN,
	};
	const D3DFORMAT fmt_ds[] =
	{
		//D3DFMT_D32,
		D3DFMT_D24S8,
		D3DFMT_D24X4S4,
		D3DFMT_D24X8,
		D3DFMT_D16,
		D3DFMT_UNKNOWN,
	};

	const int minwidth = 800;
	const int minheight = 600;

	int bb_match = ARRAYSIZE(fmt_bb) - 1;
	int ds_match = ARRAYSIZE(fmt_ds) - 1;

	int numadapters = d3d->GetAdapterCount();
	//for (int i = 0; i < numadapters; i++)
	int i = D3DADAPTER_DEFAULT;
	{
		d3d->GetAdapterIdentifier(i, 0, &dx_state1.adapter_info);
		LARGE_INTEGER nDriverVersion;
		nDriverVersion.QuadPart = dx_state1.adapter_info.DriverVersion.QuadPart;
		WORD nProduct = HIWORD(nDriverVersion.HighPart);
		WORD nVersion = LOWORD(nDriverVersion.HighPart);
		WORD nSubVersion = HIWORD(nDriverVersion.LowPart);
		WORD nBuild = LOWORD(nDriverVersion.LowPart);
		//printf("driver version %d %d %d %d\n", nProduct, nVersion, nSubVersion, nBuild);

		d3d->GetDeviceCaps(i, D3DDEVTYPE_HAL, &dx_state1.caps);

		d3d->GetAdapterDisplayMode(i, &dx_state1.desktop);
		for (int ibb = 0; ibb < ARRAYSIZE(fmt_bb); ibb++)
		{
			bool bbworks = SUCCEEDED(d3d->CheckDeviceFormat(i, D3DDEVTYPE_HAL, dx_state1.desktop.Format,
				0, D3DRTYPE_SURFACE, fmt_bb[ibb]));
			//printf("bb %d %d\n", fmt_bb[ibb], bbworks);
			if (bbworks)
			{
				for (int ids = 0; ids < ARRAYSIZE(fmt_ds); ids++)
				{
					bool dsworks = SUCCEEDED(d3d->CheckDeviceFormat(i, D3DDEVTYPE_HAL, dx_state1.desktop.Format,
						D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, fmt_ds[ids]));
					//printf("ds %d %d\n", fmt_ds[ids], dsworks);
					if (dsworks)
					{
						if (SUCCEEDED(d3d->CheckDepthStencilMatch(i, D3DDEVTYPE_HAL, dx_state1.desktop.Format,
							fmt_bb[ibb], fmt_ds[ids])))
						{
							if (ids < ds_match) {
								bb_match = ibb;
								ds_match = ids;
							}
							//printf("> %d works with %d\n", fmt_bb[ibb], fmt_ds[ids]);
						}
					}
				}
			}
		}

		dx_state1.fmt_backbuffer = fmt_bb[bb_match];
		dx_state1.fmt_depthstencil = fmt_ds[ds_match];
		//printf("selected bb %d ds %d\n", fmt_bb[bb_match], fmt_ds[ds_match]);

		dx_state1.modes.clear();

		int nummodes = d3d->GetAdapterModeCount(i, dx_state1.desktop.Format);
		for (int mode = 0; mode < nummodes; mode++)
		{
			D3DDISPLAYMODE dispMode;
			d3d->EnumAdapterModes(i, dx_state1.desktop.Format, mode, &dispMode);
			if (dispMode.Width < minwidth || dispMode.Height < minheight)
				continue;
			dx_state1.modes.push_back(dispMode);
			//printf("%d x %d %dHz %d\n", dispMode.Width, dispMode.Height, dispMode.RefreshRate, dispMode.Format);
		}
	}

	int ercd = create_window();
	if (ercd != 0)
	{
		std::cout << "failed to create window " << ercd << std::endl;
		return -1;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	fill_in_d3dpresentparams(d3dpp);

	HRESULT hr = d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		dx_state1.hwindow,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&device);

	if (FAILED(hr))
	{
		std::cout << "failed to create device " << HRESULT_CODE(hr) << std::endl;
		return -1;
	}

	return 0;
}

void fill_in_d3dpresentparams(D3DPRESENT_PARAMETERS &d3dpp)
{
	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = dx_state1.windowed ? TRUE : FALSE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = dx_state1.hwindow;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = dx_state1.fmt_backbuffer;//D3DFMT_X8R8G8B8;    // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = dx_state1.windowed ? dx_state1.width : dx_state1.desktop.Width;    // set the width of the buffer
	d3dpp.BackBufferHeight = dx_state1.windowed ? dx_state1.height : dx_state1.desktop.Height;    // set the height of the buffer
	d3dpp.BackBufferCount = 1;
	d3dpp.EnableAutoDepthStencil = dx_state1.fmt_depthstencil != D3DFMT_UNKNOWN ? TRUE : FALSE;
	d3dpp.AutoDepthStencilFormat = dx_state1.fmt_depthstencil;// D3DFMT_D16;

	if (!dx_state1.windowed)
	{
		d3dpp.FullScreen_RefreshRateInHz = dx_state1.desktop.RefreshRate;
	}
	d3dpp.PresentationInterval = dx_state1.vsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
}

int create_window()
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = dx_state1.happ;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);// (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = WNDCLASS_NAME;

	// register the window class
	RegisterClassEx(&wc);

	RECT rc;
	WINDOWPLACEMENT &wp = dx_state1.wplacement;

	SetRect(&rc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	AdjustWindowRect(&rc, WSTYLE_WINDOWED, false);

	wp.length = sizeof(wp);
	wp.showCmd = SW_SHOWNORMAL;
	wp.ptMaxPosition = wp.ptMinPosition = { -1, -1 };
	wp.rcNormalPosition = { WINDOW_XPOS, WINDOW_YPOS, WINDOW_XPOS + (rc.right - rc.left), WINDOW_YPOS + (rc.bottom - rc.top) };

	if (dx_state1.windowed)
	{
		dx_state1.width = SCREEN_WIDTH;
		dx_state1.height = SCREEN_HEIGHT;
	}
	else
	{
		SetRect(&rc, 0, 0, dx_state1.desktop.Width, dx_state1.desktop.Height);
		AdjustWindowRect(&rc, WSTYLE_FULLSCREEN, false);

		dx_state1.width = dx_state1.desktop.Width;
		dx_state1.height = dx_state1.desktop.Height;
	}
	dx_state1.aspect_ratio = (FLOAT)dx_state1.width / (FLOAT)dx_state1.height;

	dx_state1.hwindow = CreateWindowEx(NULL,
		WNDCLASS_NAME,    // name of the window class
		WINDOW_NAME,   // title of the window
		dx_state1.windowed ? WSTYLE_WINDOWED : WSTYLE_FULLSCREEN,    // window style
		dx_state1.windowed ? WINDOW_XPOS : 0,    // x-position of the window
		dx_state1.windowed ? WINDOW_YPOS : 0,    // y-position of the window
		(rc.right - rc.left),    // width of the window
		(rc.bottom - rc.top),    // height of the window
		NULL,    // parent window
		NULL,    // menu
		dx_state1.happ,    // application handle
		NULL);    // multiple windows

	if (dx_state1.hwindow == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return 0;
}

enum dx_state_change
{
	DXSTATE_FULLSCREEN,
	DXSTATE_VSYNC,
	DXSTATE_WIN_SIZE
};

int process_parameter_change(dx_state_change change)
{
	HRESULT hr = D3DERR_DEVICELOST;
	D3DPRESENT_PARAMETERS d3dpp;

	if (change == DXSTATE_FULLSCREEN)
	{
		if (dx_state1.windowed == true)
		{
			//going fullscreen
			dx_state1.windowed = false;

			//store windows pos and size to be restored when coming back from FS
			dx_state1.wplacement.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(dx_state1.hwindow, &dx_state1.wplacement);

			ShowWindow(dx_state1.hwindow, SW_HIDE);
			dx_state1.wvisible = false;

			SetWindowLong(dx_state1.hwindow, GWL_STYLE, WSTYLE_FULLSCREEN);
		}
		else
		{
			//going windowed
			dx_state1.windowed = true;

			SetWindowLong(dx_state1.hwindow, GWL_STYLE, WSTYLE_WINDOWED);
			if (dx_state1.wplacement.length)
			{
				SetWindowPlacement(dx_state1.hwindow, &dx_state1.wplacement);
			}
		}
	}
	if (change == DXSTATE_FULLSCREEN || change == DXSTATE_WIN_SIZE)
	{
		if (dx_state1.windowed)
		{
			dx_state1.aspect_ratio = (FLOAT)dx_state1.width / (FLOAT)dx_state1.height;
		}
		else
		{
			dx_state1.aspect_ratio = (FLOAT)dx_state1.desktop.Width / (FLOAT)dx_state1.desktop.Height;
		}
	}
	if (change == DXSTATE_VSYNC)
	{
		dx_state1.vsync = !dx_state1.vsync;
	}

	fill_in_d3dpresentparams(d3dpp);
	hr = dx_state1.device->Reset(&d3dpp);
	if (FAILED(hr))
	{
		std::cout << "failed to reset device " << HRESULT_CODE(hr) << std::endl;
		PostMessage(dx_state1.hwindow, WM_CLOSE, -1, 0);
	}

	if ( ! dx_state1.wvisible )
	{
		ShowWindow(dx_state1.hwindow, SW_SHOWNORMAL);
		dx_state1.wvisible = true;
	}

	// Ensure that the display doesn't power down when fullscreen but does when windowed
	if (!dx_state1.windowed)
		SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
	else
		SetThreadExecutionState(ES_CONTINUOUS);

	return 0;
}

// this is the function that cleans up Direct3D and COM
void cleanD3D(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
{
	device->Release();    // close and release the 3D device
	d3d->Release();    // close and release Direct3D
}

int main()
{
	int retcode = 0;
	std::cout << "Hello World!\n";

	run_tests();

	//we have a ctor now, this also breaks std::vector
	//ZeroMemory(&dx_state1, sizeof(dx_state1));

	dx_state1.windowed = true;
	dx_state1.vsync = true;

	dx_state1.happ = (HINSTANCE)GetModuleHandle(NULL);// (HINSTANCE)GetCurrentProcess();

	if (0 > initD3D(NULL, dx_state1.d3d, dx_state1.device))
	{
		return -1;
	}

	ShowWindow(dx_state1.hwindow, SW_SHOWNORMAL);
	dx_state1.wvisible = true;

	// enter the main loop:
	// this struct holds Windows event messages
	MSG msg;
	bool end_msg_loop = false;

	//// wait for the next message in the queue, store the result in 'msg'
	//while (GetMessage(&msg, NULL, 0, 0))
	//{
	//	// translate keystroke messages into the right format
	//	TranslateMessage(&msg);

	//	// send the message to the WindowProc function
	//	DispatchMessage(&msg);
	//}

	// Enter the infinite message loop
	while (TRUE)
	{
		// Check to see if any messages are waiting in the queue
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// wndproc posted WM_QUIT, exit now
			if (msg.message == WM_QUIT)
			{
				end_msg_loop = true;
				//break;
			}

			// Translate the message and dispatch it to WindowProc()
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If the message is WM_QUIT, exit the while loop
		if (end_msg_loop)
			break;

		inputs kp = get_keypress();

		if (kp.f == 1)
		{
			kp.f = 0;
			process_parameter_change(DXSTATE_FULLSCREEN);
			clear_last_demo();
		}

		if (kp.v == 1)
		{
			kp.v = 0;
			process_parameter_change(DXSTATE_VSYNC);
			clear_last_demo();
		}

		//clear the flags for fs and vsync
		store_keypress(kp);

		int unit_res = -1;
		switch (active_unit)
		{
		case 1:
			unit_res = call_loop_unit1(selection, dx_state1.d3d, dx_state1.device);
			break;
		case 2:
			unit_res = call_loop_unit2(selection, dx_state1.d3d, dx_state1.device);
			break;
		default:
		case 3:
			unit_res = call_loop_unit3(selection, dx_state1.d3d, dx_state1.device);
			break;
		}
			
		if(0 != unit_res)
			break;

	}
	cleanD3D(dx_state1.d3d, dx_state1.device);
	ShowWindow(dx_state1.hwindow, SW_HIDE);
	DestroyWindow(dx_state1.hwindow);

	// return this part of the WM_QUIT message to Windows
	retcode = (int)msg.wParam;

	std::cout << std::endl << "msgloop ret: " << retcode << std::endl;
	system("pause");

	return retcode;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool in_size_move = false;
	static bool ctrl_pressed = false;
	static bool shift_pressed = false;
	union inputs pressed = { 0 };

	// sort through and find what code to run for the message given
	switch (message)
	{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				if (dx_state1.windowed == false)
					process_parameter_change(DXSTATE_FULLSCREEN);
				PostMessage(hWnd, WM_CLOSE, -1, 0);
				return 0;
			}
			if (wParam == VK_CONTROL)
			{
				ctrl_pressed = true;
			}
			if (wParam == VK_SHIFT)
			{
				shift_pressed = true;
			}
		break;
		case WM_KEYUP:
			if (wParam == VK_TAB)
			{
				selection--;
			}
			if (wParam == VK_CONTROL)
			{
				ctrl_pressed = false;
			}
			if (wParam == VK_SHIFT)
			{
				shift_pressed = false;
			}
			if (wParam == VK_UP)
			{
				pressed.vert = 1;
			}
			if (wParam == VK_DOWN)
			{
				pressed.vert = -1;
			}
			if (wParam == VK_LEFT)
			{
				pressed.horiz = -1;
			}
			if (wParam == VK_RIGHT)
			{
				pressed.horiz = 1;
			}
			if (wParam == 'T')
			{
				pressed.t = 1;
			}
			if (wParam == 'F')
			{
				pressed.f = 1;
			}
			if (wParam == 'V')
			{
				pressed.v = 1;
			}
			if (wParam >= VK_F1 && wParam <= VK_F12 && VK_F1 < VK_F12)
			{
				active_unit = 1 + ((int)wParam - VK_F1);
				selection = INT_MAX;
			}
			if (wParam == 'P')
			{
				pressed.p = 1;
			}
		break;
		// this message is read when the window is closed
		case WM_DESTROY:
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		break;
		case WM_EXITSIZEMOVE:
		{
			static int count = 0;
			RECT rc;

			in_size_move = false;

			GetClientRect(dx_state1.hwindow, &rc);
			std::cout << count++ << " wm_size " << rc.right << "x" << rc.bottom << " " << dx_state1.width << "x" << dx_state1.height << std::endl;
			if (dx_state1.windowed && (rc.right != dx_state1.width ||
				rc.bottom != dx_state1.height))
			{
				dx_state1.width = rc.right;
				dx_state1.height = rc.bottom;

				process_parameter_change(DXSTATE_WIN_SIZE);
				clear_last_demo();
			}
		}
		break;
		case WM_ENTERSIZEMOVE:
			in_size_move = true;
			break;
		case WM_SIZE:
		{
			if (wParam == SIZE_MAXIMIZED || (!in_size_move && wParam == SIZE_RESTORED))
			{
				RECT rc;
				GetClientRect(dx_state1.hwindow, &rc);
				if (dx_state1.windowed && (rc.right != dx_state1.width ||
					rc.bottom != dx_state1.height))
				{
					std::cout << (wParam == SIZE_MAXIMIZED ? "maximize " : "restore ");
					std::cout << rc.right << "x" << rc.bottom << " " << dx_state1.width << "x" << dx_state1.height << std::endl;

					dx_state1.width = rc.right;
					dx_state1.height = rc.bottom;

					process_parameter_change(DXSTATE_WIN_SIZE);
					clear_last_demo();
				}
			}
		}
		break;
		case WM_ACTIVATE:
			std::cout << "wm_activate " << wParam << std::endl;
			break;
	}

	if (ctrl_pressed)
		pressed.ctrl = 1;
	if (shift_pressed)
		pressed.shift = 1;

	if (pressed.all != 0)
		store_keypress(pressed);

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
