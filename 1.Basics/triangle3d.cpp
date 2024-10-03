
#include "basics.h"
#include <d3dx9.h>

struct CUSTOMVERTEX
{
	FLOAT X, Y, Z;
	DWORD COLOR;
};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

static LPDIRECT3DVERTEXBUFFER9 v_buffer;
static int triangle3d_init_done = 0;
static int triangle3d_do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
{
	device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting

	device->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);

	// create three vertices using the CUSTOMVERTEX struct built earlier
	CUSTOMVERTEX t_vert[] =
	{
		{ 2.5f, -3.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255), },
		{ 0.0f, 3.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0), },
		{ -2.5f, -3.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0), },
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
	memcpy(pVoid, t_vert, sizeof(t_vert));    // copy the vertices to the locked buffer
	v_buffer->Unlock();    // unlock the vertex buffer

	triangle3d_init_done = 1;
	return 0;
}

int triangle3d(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
{
	if (triangle3d_init_done == 0)
		if (0 != triangle3d_do_init(d3d, device))
			return -1;


	device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);

	device->BeginScene();

	// select which vertex format we are using
	device->SetFVF(CUSTOMFVF);

	// SET UP THE PIPELINE

	D3DXMATRIX matRotateY;    // a matrix to store the rotation information

	static float index = 0.0f; index += 0.05f;    // an ever-increasing float value

	// build a matrix to rotate the model based on the increasing float value
	D3DXMatrixRotationY(&matRotateY, index);

	// tell Direct3D about our matrix
	device->SetTransform(D3DTS_WORLD, &matRotateY);

	D3DXMATRIX matView;    // the view transform matrix

	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(0.0f, 0.0f, 10.0f),    // the camera position
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction

	device->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

	D3DXMATRIX matProjection;     // the projection transform matrix

	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),    // the horizontal field of view
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
		1.0f,    // the near view-plane
		100.0f);    // the far view-plane

	device->SetTransform(D3DTS_PROJECTION, &matProjection);    // set the projection

	// select the vertex buffer to display
	device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

	// copy the vertex buffer to the back buffer
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

	device->EndScene();

	device->Present(NULL, NULL, NULL, NULL);

	return 0;
}