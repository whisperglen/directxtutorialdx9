
#include "basics.h"

struct CUSTOMVERTEX { FLOAT X, Y, Z; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

class Blend: public Demo
{
struct mesh_obj objects[1];
public:
	Blend() : Demo() {};

	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		// create the vertices using the CUSTOMVERTEX struct
		CUSTOMVERTEX vertices[] =
		{
			// square 1
			{ -3.0f, 3.0f, 3.0f, D3DCOLOR_ARGB(255, 0, 0, 255), },
			{ -3.0f, -3.0f, 3.0f, D3DCOLOR_ARGB(255, 0, 255, 0), },
			{ 3.0f, 3.0f, 3.0f, D3DCOLOR_ARGB(255, 255, 0, 0), },
			{ 3.0f, -3.0f, 3.0f, D3DCOLOR_ARGB(255, 0, 255, 255), },

			// square 2
			{ -3.0f, 3.0f, 3.0f, D3DCOLOR_ARGB(192, 0, 0, 255), },
			{ -3.0f, -3.0f, 3.0f, D3DCOLOR_ARGB(192, 0, 255, 0), },
			{ 3.0f, 3.0f, 3.0f, D3DCOLOR_ARGB(192, 255, 0, 0), },
			{ 3.0f, -3.0f, 3.0f, D3DCOLOR_ARGB(192, 0, 255, 255), },
		};

		// create a vertex buffer interface called v_buffer
		device->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX),
			0,
			CUSTOMFVF,
			D3DPOOL_MANAGED,
			&objects[0].v_buffer,
			NULL);

		VOID* pVoid;    // a void pointer

		// lock v_buffer and load the vertices into it
		objects[0].v_buffer->Lock(0, 0, (void**)&pVoid, 0);
		memcpy(pVoid, vertices, sizeof(vertices));
		objects[0].v_buffer->Unlock();

		device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn on the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
		device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		//device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(70, 70, 70));    // ambient light

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

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

		union inputs key = get_keypress();

		static float scaling = 1.0f;
		if (key.vert && key.ctrl)
		{
			scaling += 0.05f * key.vert;
		}


		static float rotx = 0.f, roty = 0.f, rotz = 0.f;
		if (key.horiz && !key.ctrl)
		{
			roty += 0.2f * -key.horiz;
		}
		if (key.horiz && key.ctrl)
		{
			rotz += 0.2f * key.horiz;
		}
		if (key.vert && !key.ctrl)
		{
			rotx += 0.2f * key.vert;
		}

		device->BeginScene();

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		D3DXMATRIX camRotate;
		D3DXMATRIX scratch;
		D3DXMatrixRotationX(&camRotate, rotx);
		camRotate *= *D3DXMatrixRotationY(&scratch, roty);
		camRotate *= *D3DXMatrixRotationZ(&scratch, rotz);
		D3DXVECTOR3 camerapos, cameraup;

		D3DXVec3TransformCoord(&camerapos, &D3DXVECTOR3(0.0f, 20.0f, 50.0f), &camRotate);
		D3DXVec3TransformCoord(&cameraup, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &camRotate);
		// set the view transform
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView,
			&camerapos,    // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),      // the look-at position
			&cameraup);    // the up direction
		device->SetTransform(D3DTS_VIEW, &matView);

		// set the projection transform
		D3DXMATRIX matProjection;
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),
			dx_state1.aspect_ratio,
			1.0f,    // the near view-plane
			1000.0f);    // the far view-plane
		device->SetTransform(D3DTS_PROJECTION, &matProjection);

		
		// set the world transform
		D3DXMATRIX matRotate;
		D3DXMATRIX matScale;
		D3DXMATRIX matScratch;
		D3DXMatrixIdentity(&matRotate); D3DXMatrixIdentity(&matScale);
		//D3DXMatrixRotationX(&matRotate, D3DXToRadian(-90));
		//D3DXMatrixRotationY(&matScratch, rotate);
		//matRotate *= matScratch;
		D3DXMatrixScaling(&matScale, scaling, scaling, scaling);

		// set the stream source
		device->SetStreamSource(0, objects[0].v_buffer, 0, sizeof(CUSTOMVERTEX));

		// set the first world transform
		D3DXMATRIX matTranslate;
		D3DXMatrixTranslation(&matTranslate, 0.0f, 0.0f, -10.0f);
		device->SetTransform(D3DTS_WORLD, &(matTranslate));    // set the world transform

		// draw the first square
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		// set the second world transform
		D3DXMatrixTranslation(&matTranslate, 0.0f, 0.0f, 0.0f);
		device->SetTransform(D3DTS_WORLD, &(matTranslate));    // set the world transform

		// draw the second square
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);


		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);
		return 0;
	}
};

Demo *factory_blend_create()
{
	return new Blend();
}