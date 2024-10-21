
#include "basics.h"

struct CUSTOMVERTEX
{
	FLOAT X, Y, Z;
	DWORD COLOR;
};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

class Triangle3D : public Demo
{
protected:
	LPDIRECT3DVERTEXBUFFER9 v_buffer;
public:
	Triangle3D() : Demo() {};
	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
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

		device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
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
};

Demo *factory_triangle3d_create()
{
	return new Triangle3D();
}

class TriangleFly : public Triangle3D
{
public:
	TriangleFly() : Triangle3D() {};

	virtual int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == false)
			if (0 != do_init(d3d, device))
				return -1;

		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);

		device->BeginScene();

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		// SET UP THE PIPELINE

		D3DXMATRIX matObjLoc;    // a matrix to store the rotation information
		D3DXMATRIX matScratch;;

		static float index = 0.0f; index += 0.05f;    // an ever-increasing float value
		if (index > 10.f)
			index = 0.f;

		// build a matrix to rotate the model based on the increasing float value
		D3DXMatrixRotationY(&matObjLoc, index);
		matObjLoc *= *D3DXMatrixTranslation(&matScratch, 0.f, -index * 5, 0.f);

		// tell Direct3D about our matrix
		device->SetTransform(D3DTS_WORLD, &matObjLoc);

		D3DXMATRIX matView;    // the view transform matrix

		D3DXVECTOR3 lookatpos;
		D3DXVec3TransformCoord(&lookatpos, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &matObjLoc);

		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f, 0.0f, 10.0f),    // the camera position
			&lookatpos,    // the look-at position
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
};

Demo *factory_trianglefly_create()
{
	return new TriangleFly();
}

class TrianglePsychedelic : public Demo
{
	LPDIRECT3DVERTEXBUFFER9 v_buffer2;
public:
	TrianglePsychedelic() : Demo() {};

	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		// create three vertices using the CUSTOMVERTEX struct built earlier
		CUSTOMVERTEX t_vert[] =
		{
			{ 4.0f, -1.73f, 2.0f, D3DCOLOR_XRGB(50, 50, 50), },
			{ 4.0f, -1.73f, -2.0f, D3DCOLOR_XRGB(50, 50, 50), },
			{ -4.0f, -1.73f, -2.0f, D3DCOLOR_XRGB(50, 50, 50), },
			{ -4.0f, -1.73f, -2.0f, D3DCOLOR_XRGB(50, 50, 50), },
			{ -4.0f, -1.73f, 2.0f, D3DCOLOR_XRGB(50, 50, 50), },
			{ 4.0f, -1.73f, 2.0f, D3DCOLOR_XRGB(50, 50, 50), },

			{ -3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(255, 0, 0), },
			{ 0.0f, 3.46f, 0.0f, D3DCOLOR_XRGB(0, 255, 0), },
			{ 3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(0, 0, 255), },
			{ 0.0f, 3.46f, 0.0f, D3DCOLOR_XRGB(255, 0, 255), },
			{ -3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(0, 255, 255), },
			{ 3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(255, 255, 0), },

			{ -4.0f, -1.73f, -2.0f, D3DCOLOR_XRGB(0, 0, 0), },
			{ 4.0f, -1.73f, -2.0f, D3DCOLOR_XRGB(0, 0, 0), },
			{ 4.0f, -1.73f, 2.0f, D3DCOLOR_XRGB(0, 0, 0), },
			{ 4.0f, -1.73f, 2.0f, D3DCOLOR_XRGB(0, 0, 0), },
			{ -4.0f, -1.73f, 2.0f, D3DCOLOR_XRGB(0, 0, 0), },
			{ -4.0f, -1.73f, -2.0f, D3DCOLOR_XRGB(0, 0, 0), },
		};

		// create the vertex and store the pointer into v_buffer, which is created globally
		device->CreateVertexBuffer(sizeof(t_vert),
			0,
			CUSTOMFVF,
			D3DPOOL_MANAGED,
			&v_buffer2,
			NULL);

		VOID* pVoid;    // the void pointer

		v_buffer2->Lock(0, 0, (void**)&pVoid, 0);    // lock the vertex buffer
		memcpy(pVoid, t_vert, sizeof(t_vert));    // copy the vertices to the locked buffer
		v_buffer2->Unlock();    // unlock the vertex buffer

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

		// SET UP THE PIPELINE

		D3DXMATRIX matObjLoc;    // a matrix to store the rotation information
		D3DXMATRIX matScratch;;

		static float index = 0.0f; index += 0.01f;    // an ever-increasing float value

		D3DXMatrixIdentity(&matObjLoc);

		// build a matrix to rotate the model based on the increasing float value
		//D3DXMatrixRotationZ(&matObjLoc, index);
		//matObjLoc *= *D3DXMatrixTranslation(&matScratch, 0.f, 0.f, -10.f);

		// tell Direct3D about our matrix
		device->SetTransform(D3DTS_WORLD, &matObjLoc);

		D3DXMATRIX matView;    // the view transform matrix

		D3DXVECTOR3 camerapos, updir;
		D3DXMatrixRotationX(&matScratch, index);
		D3DXVec3TransformCoord(&camerapos, &D3DXVECTOR3(10.0f, 0.0f, 10.0f), &matScratch);
		D3DXVec3TransformCoord(&updir, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &matScratch);

		D3DXMatrixLookAtLH(&matView,
			&camerapos,    // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
			&updir);    // the up direction

		device->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

		D3DXMATRIX matProjection;     // the projection transform matrix

		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),    // the horizontal field of view
			(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
			1.0f,    // the near view-plane
			100.0f);    // the far view-plane

		device->SetTransform(D3DTS_PROJECTION, &matProjection);    // set the projection

		// select the vertex buffer to display
		device->SetStreamSource(0, v_buffer2, 0, sizeof(CUSTOMVERTEX));

		// copy the vertex buffer to the back buffer
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 6);

		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}
};

Demo *factory_trianglepsychedelic_create()
{
	return new TrianglePsychedelic();
}

class TriangleDepth : public Demo
{
	LPDIRECT3DVERTEXBUFFER9 v_buffer3;
public:
	TriangleDepth() : Demo() {}

	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		// create three vertices using the CUSTOMVERTEX struct built earlier
		CUSTOMVERTEX t_vert[] =
		{
			{ 0.0f, 3.46f, 0.0f, D3DCOLOR_XRGB(255, 0, 255), },
			{ -3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(0, 255, 255), },
			{ 3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(255, 255, 0), },
		};

		// create the vertex and store the pointer into v_buffer, which is created globally
		device->CreateVertexBuffer(sizeof(t_vert),
			0,
			CUSTOMFVF,
			D3DPOOL_MANAGED,
			&v_buffer3,
			NULL);

		VOID* pVoid;    // the void pointer

		v_buffer3->Lock(0, 0, (void**)&pVoid, 0);    // lock the vertex buffer
		memcpy(pVoid, t_vert, sizeof(t_vert));    // copy the vertices to the locked buffer
		v_buffer3->Unlock();    // unlock the vertex buffer

		device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer

		init_done = true;
		return 0;
	}

	virtual int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == false)
			if (0 != do_init(d3d, device))
				return -1;

		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);
		device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		device->BeginScene();

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		// SET UP THE PIPELINE

		// set the view transform
		D3DXMATRIX matView;    // the view transform matrix
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f, 0.0f, 15.0f),   // the camera position
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
		device->SetStreamSource(0, v_buffer3, 0, sizeof(CUSTOMVERTEX));

		D3DXMATRIX matTranslateA;    // a matrix to store the translation for triangle A
		D3DXMATRIX matScaleA;
		D3DXMATRIX matTranslateB;    // a matrix to store the translation for triangle B
		D3DXMATRIX matRotateY;    // a matrix to store the rotation for each triangle
		static float index = 0.0f; index += 0.05f; // an ever-increasing float value

		// build MULTIPLE matrices to translate the model and one to rotate
		D3DXMatrixTranslation(&matTranslateA, 2.0f, -2.0f, 2.0f);
		D3DXMatrixScaling(&matScaleA, 0.5f, 0.5f, 0.5f);
		D3DXMatrixTranslation(&matTranslateB, 0.0f, 0.0f, -2.0f);
		D3DXMatrixIdentity(&matRotateY);
		D3DXMatrixRotationY(&matRotateY, index);    // the front side

		// tell Direct3D about each world transform, and then draw another triangle
		device->SetTransform(D3DTS_WORLD, &(matScaleA * matTranslateA * matRotateY));
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		device->SetTransform(D3DTS_WORLD, &(matTranslateB * matRotateY));
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}
};

Demo *factory_triangledepth_create()
{
	return new TriangleDepth();
}
