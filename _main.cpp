// directxtutorialdx9.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "helper.h"

#define WNDCLASS_NAME	TEXT("DXTut9WCls")
#define WINDOW_NAME		TEXT("directxtutorialdx9")
#define WINDOW_STYLE	WS_OVERLAPPEDWINDOW //non-fullscreen
//#define WINDOW_STYLE	WS_EX_TOPMOST | WS_POPUP    // fullscreen values

#define WINDOW_XPOS		200
#define WINDOW_YPOS		200

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

extern int run_tests();
extern int call_loop_unit1(int& selection, LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device);

static int selection = INT_MAX;


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd, LPDIRECT3D9& d3d, LPDIRECT3DDEVICE9& device)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;    // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = SCREEN_WIDTH;    // set the width of the buffer
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;    // set the height of the buffer
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create a device class using this information and information from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&device);
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

	HINSTANCE hInstance = (HINSTANCE)GetCurrentProcess();
	int nCmdShow = SW_SHOWNORMAL;

	// the handle for the window, filled by a function
	HWND hWnd;
	// this struct holds information for the window class
	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = WNDCLASS_NAME;

	// register the window class
	RegisterClassEx(&wc);

	// create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		WNDCLASS_NAME,    // name of the window class
		WINDOW_NAME,   // title of the window
		WINDOW_STYLE,    // window style
		WINDOW_XPOS,    // x-position of the window
		WINDOW_YPOS,    // y-position of the window
		SCREEN_WIDTH,    // width of the window
		SCREEN_HEIGHT,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL

	if (hWnd == NULL)
	{
		retcode = GetLastError();
		goto exit_main;
	}

	// display the window on the screen
	ShowWindow(hWnd, nCmdShow);


	{
		LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
		LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class

		initD3D(hWnd, d3d, d3ddev);

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
					break;
				}

				// Translate the message and dispatch it to WindowProc()
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			// If the message is WM_QUIT, exit the while loop
			if (end_msg_loop)
				break;

			// Run game code here
			if(0 != call_loop_unit1(selection, d3d, d3ddev))
				break;
		}
		cleanD3D(d3d, d3ddev);
		DestroyWindow(hWnd);
		DeleteObject(hWnd);

		// return this part of the WM_QUIT message to Windows
		retcode = msg.wParam;
	}

exit_main:
	std::cout << std::endl << "msgloop ret: " << retcode << std::endl;
	system("pause");

	return retcode;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool ctrl_pressed = false;
	union inputs pressed = { 0 };

	// sort through and find what code to run for the message given
	switch (message)
	{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
				return 0;
			}
			if (wParam == VK_CONTROL)
			{
				ctrl_pressed = true;
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
		break;
		// this message is read when the window is closed
		case WM_DESTROY:
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		break;
	}

	if (ctrl_pressed)
		pressed.ctrl = 1;

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
