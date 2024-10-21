
#include "basics.h"

struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;    // from the D3DFVF_XYZRHW flag
	DWORD color;    // from the D3DFVF_DIFFUSE flag
};
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

class Triangle : public Demo
{
	LPDIRECT3DVERTEXBUFFER9 v_buffer;
public:
	Triangle(): Demo() {};

	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		device->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
			0,
			CUSTOMFVF,
			D3DPOOL_MANAGED,
			&v_buffer,
			NULL);

		// create three vertices using the CUSTOMVERTEX struct built earlier
		CUSTOMVERTEX vertices[] =
		{
			{ 320.0f, 50.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 0, 255), },
			{ 520.0f, 400.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 255, 0), },
			{ 120.0f, 400.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(255, 0, 0), },
		};

		// create the vertex and store the pointer into v_buffer, which is created globally
		device->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
			0,
			CUSTOMFVF,
			D3DPOOL_MANAGED,
			&v_buffer,
			NULL);

		VOID* pVoid;    // the void pointer

		v_buffer->Lock(0, 0, (void**)&pVoid, 0);    // lock the vertex buffer
		memcpy(pVoid, vertices, sizeof(vertices));    // copy the vertices to the locked buffer
		v_buffer->Unlock();    // unlock the vertex buffer

		device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ZENABLE, FALSE);    // turn on the z-buffer

		init_done = true;
		return 0;
	}

	virtual int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == false)
			if (0 != do_init(d3d, device))
				return -1;


		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);

		device->BeginScene();

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		// select the vertex buffer to display
		device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

		// copy the vertex buffer to the back buffer
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}
};

Demo *factory_triangle_create()
{
	return new Triangle();
}