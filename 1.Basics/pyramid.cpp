
#include "basics.h"

struct CUSTOMVERTEX
{
	FLOAT X, Y, Z;
	DWORD COLOR;
};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

namespace cube {
static LPDIRECT3DVERTEXBUFFER9 v_buffer;
static LPDIRECT3DINDEXBUFFER9 i_buffer;
static int cube_init_done = 0;
static int cube_do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
{
	CUSTOMVERTEX vertices[] =
	{
		{ -3.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 0, 255), },    // vertex 0
		{ 3.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 0), },     // vertex 1
		{ -3.0f, -3.0f, -3.0f, D3DCOLOR_XRGB(255, 0, 0), },   // 2
		{ 3.0f, -3.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 255), },  // 3
		{ -3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB(0, 0, 255), },     // ...
		{ 3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB(255, 0, 0), },
		{ -3.0f, -3.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 0), },
		{ 3.0f, -3.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 255), },
	};

	// create the vertex and store the pointer into v_buffer, which is created globally
	device->CreateVertexBuffer(sizeof(vertices),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);

	VOID* pVoid;    // the void pointer

	v_buffer->Lock(0, 0, (void**)&pVoid, 0);    // lock the vertex buffer
	memcpy(pVoid, vertices, sizeof(vertices));    // copy the vertices to the locked buffer
	v_buffer->Unlock();    // unlock the vertex buffer

	short indices[] =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 0, 6,    // side 2
		6, 0, 2,
		7, 5, 6,    // side 3
		6, 5, 4,
		3, 1, 7,    // side 4
		7, 1, 5,
		4, 5, 0,    // side 5
		0, 5, 1,
		3, 7, 2,    // side 6
		2, 7, 6,
	};

	// create an index buffer interface called i_buffer
	device->CreateIndexBuffer(sizeof(indices),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&i_buffer,
		NULL);

	// lock i_buffer and load the indices into it
	i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indices, sizeof(indices));
	i_buffer->Unlock();

	cube_init_done = 1;
	return 0;
}

int cube(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
{

	if (cube_init_done == 0)
		if (0 != cube_do_init(d3d, device))
			return -1;

	device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
	//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer

	device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);
	device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	device->BeginScene();

	// select which vertex format we are using
	device->SetFVF(CUSTOMFVF);

	// SET UP THE PIPELINE

	// set the view transform
	D3DXMATRIX matView;    // the view transform matrix
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(0.0f, 15.0f, -15.0f),   // the camera position
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
	device->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

	// set the projection transform
	D3DXMATRIX matProjection;    // the projection transform matrix
	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),    // the horizontal field of view
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
		1.0f,    // the near view-plane
		100.0f);    // the far view-plane
	device->SetTransform(D3DTS_PROJECTION, &matProjection);     // set the projection


	// select the vertex buffer to display
	device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
	device->SetIndices(i_buffer);

	D3DXMATRIX matTranslateA;    // a matrix to store the translation for triangle A
	D3DXMATRIX matTranslateB;    // a matrix to store the translation for triangle B
	D3DXMATRIX matRotateY;    // a matrix to store the rotation for each triangle
	static float index = 0.0f; index += 0.05f; // an ever-increasing float value

	// build MULTIPLE matrices to translate the model and one to rotate
	D3DXMatrixTranslation(&matTranslateA, 5.0f, 0.0f, 0.0f);
	D3DXMatrixRotationZ(&matRotateY, index);    // the front side

	// tell Direct3D about each world transform, and then draw another triangle
	device->SetTransform(D3DTS_WORLD, &(matTranslateA * matRotateY));
	// draw the cube
	device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	device->EndScene();

	device->Present(NULL, NULL, NULL, NULL);

	return 0;
}
}

namespace pyramid {
	static LPDIRECT3DVERTEXBUFFER9 v_buffer;
	static LPDIRECT3DINDEXBUFFER9 i_buffer;
	static int init_done = 0;
	static int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		CUSTOMVERTEX vertices[] =
		{
			{ 0.0f, -1.73f, 3.46f, D3DCOLOR_XRGB(0, 255, 255), },
			{ 0.0f, 3.46f, 0.0f, D3DCOLOR_XRGB(0, 0, 0), },
			{ -3.0f, -1.73f, -1.73f, D3DCOLOR_XRGB(255, 255, 0), },
			{ 3.0f, -1.73f, -1.73f, D3DCOLOR_XRGB(255, 0, 255), },
		};

		// create the vertex and store the pointer into v_buffer, which is created globally
		device->CreateVertexBuffer(sizeof(vertices),
			0,
			CUSTOMFVF,
			D3DPOOL_MANAGED,
			&v_buffer,
			NULL);

		VOID* pVoid;    // the void pointer

		v_buffer->Lock(0, 0, (void**)&pVoid, 0);    // lock the vertex buffer
		memcpy(pVoid, vertices, sizeof(vertices));    // copy the vertices to the locked buffer
		v_buffer->Unlock();    // unlock the vertex buffer

		short indices[] =
		{
			1, 3, 2,
			1, 2, 0,
			1, 0, 3,
			0, 2, 3
		};

		// create an index buffer interface called i_buffer
		device->CreateIndexBuffer(sizeof(indices),
			0,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&i_buffer,
			NULL);

		// lock i_buffer and load the indices into it
		i_buffer->Lock(0, 0, (void**)&pVoid, 0);
		memcpy(pVoid, indices, sizeof(indices));
		i_buffer->Unlock();

		init_done = 1;
		return 0;
	}

	int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == 0)
			if (0 != do_init(d3d, device))
				return -1;

		device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
		//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer

		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);
		device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		device->BeginScene();

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		// SET UP THE PIPELINE

		// set the view transform
		D3DXMATRIX matView;    // the view transform matrix
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f, 10.0f, -10.0f),   // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
		device->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

		// set the projection transform
		D3DXMATRIX matProjection;    // the projection transform matrix
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),    // the horizontal field of view
			(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
			1.0f,    // the near view-plane
			100.0f);    // the far view-plane
		device->SetTransform(D3DTS_PROJECTION, &matProjection);     // set the projection


		// select the vertex buffer to display
		device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		device->SetIndices(i_buffer);

		D3DXMATRIX matTranslateA;    // a matrix to store the translation for mesh A
		D3DXMATRIX matRotateY;    // a matrix to store the rotation for each mesh
		static float index = 0.0f;
		index += 0.05f; // an ever-increasing float value

		static float x = 0.f, y = 0.f, z = 0.f;
		union inputs kp = get_keypress();
		if (kp.horiz)
			x += 1.f * kp.horiz;
		if (kp.vert) {
			if (kp.ctrl)
				z += 1.f * kp.vert;
			else
				y += 1.f * kp.vert;
		}
		if (kp.all)
			std::cout << "Coords: x:" << x << " y:" << y << " z:" << z << std::endl;

		// build MULTIPLE matrices to translate the model and one to rotate
		D3DXMatrixTranslation(&matTranslateA, x, y, z);
		D3DXMatrixRotationZ(&matRotateY, index);    // the front side

		// tell Direct3D about each world transform, and then draw another triangle
		device->SetTransform(D3DTS_WORLD, &( matTranslateA));
		// draw the cube
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 4);

		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}

	int spawn(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		struct coords
		{
			float x, y, z, size;
		};
		static struct coords pyramids[10];

		if (init_done == 0)
		{
			srand((unsigned int)time(NULL));
			for (int i = 0; i < sizeof(pyramids) / sizeof(pyramids[0]); i++)
			{
				int val = rand();
				//x [-9, 12]
				val = val % (10*(9 + 12 + 1));
				pyramids[i].x = (float)(val) / 10 -9;
				val = rand();
				//y [-11, 7]
				val = val % (10 * (11 + 7 + 1));
				pyramids[i].y = (float)(val) / 10 -11;
				val = rand();
				//z [-10, 23]
				val = val % (10 * (10 + 23 + 1));
				pyramids[i].z = (float)(val) / 10 -10;
				val = rand();
				//scale [-1, 1]
				val = val % (10 * (0 + 1));
				pyramids[i].size = (float)(val) / 10 - 0.f;
			}
			if (0 != do_init(d3d, device))
				return -1;
		}

		device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
		//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer

		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);
		device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		device->BeginScene();

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		// SET UP THE PIPELINE

		// set the view transform
		static float x = 0.f, y = 0.f, z = 0.f;
		union inputs kp = get_keypress();
		if (kp.horiz)
			x += 1.f * kp.horiz;
		if (kp.vert) {
			if (kp.ctrl)
				z += 1.f * kp.vert;
			else
				y += 1.f * kp.vert;
		}

		D3DXMATRIX matView;    // the view transform matrix
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(-10.0f +x, 10.0f +y, -10.0f +z),   // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
		device->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

		// set the projection transform
		D3DXMATRIX matProjection;    // the projection transform matrix
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),    // the horizontal field of view
			(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
			1.0f,    // the near view-plane
			100.0f);    // the far view-plane
		device->SetTransform(D3DTS_PROJECTION, &matProjection);     // set the projection


		// select the vertex buffer to display
		device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		device->SetIndices(i_buffer);

		D3DXMATRIX matTranslateA;    // a matrix to store the translation for mesh A
		D3DXMATRIX matRotateY;    // a matrix to store the rotation for each mesh
		D3DXMATRIX matScale;
		static float index = 0.0f;
		index += 0.05f; // an ever-increasing float value

		for (int i = 0; i < sizeof(pyramids) / sizeof(pyramids[0]); i++)
		{

			// build MULTIPLE matrices to translate the model and one to rotate
			D3DXMatrixTranslation(&matTranslateA, pyramids[i].x, pyramids[i].y, pyramids[i].z);
			D3DXMatrixScaling(&matScale, pyramids[i].size, pyramids[i].size, pyramids[i].size);
			D3DXMatrixRotationY(&matRotateY, index);    // the front side

			// tell Direct3D about each world transform, and then draw another triangle
			device->SetTransform(D3DTS_WORLD, &(matRotateY * matScale * matTranslateA));
			// draw the cube
			device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 4);
		}

		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}
}