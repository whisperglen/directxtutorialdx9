
#include "textures.h"

using std::cout;
using std::endl;

typedef float vec2_t[2];
typedef float vec3_t[3];

class MultiBuffPyramid : public Demo
{
protected:
	LPDIRECT3DVERTEXBUFFER9 v_buffer[3];
	LPDIRECT3DINDEXBUFFER9 i_buffer;
	LPDIRECT3DVERTEXDECLARATION9 vertexDecl;
	LPDIRECT3DTEXTURE9 texture[4];
public:
	MultiBuffPyramid() : Demo() {}

	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		D3DVERTEXELEMENT9 vertexspec[] =
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			D3DDECL_END()
		};
		device->CreateVertexDeclaration(vertexspec, &vertexDecl);

		vec3_t verts[] =
		{
			{ 0.0f, -1.73f, 3.46f, },
			{ 0.0f, 3.46f, 0.0f, },
			{ -3.0f, -1.73f, -1.73f, },
			{ 3.0f, -1.73f, -1.73f, },

			{ -3.0f, -1.73f, -1.73f, }, //2
			{ 0.0f, -1.73f, 3.46f, }, //0
			{ 3.0f, -1.73f, -1.73f, }, //3
		};
		D3DCOLOR cols[] =
		{
			{ D3DCOLOR_XRGB(0, 255, 255), },
			{ D3DCOLOR_XRGB(0, 0, 0), },
			{ D3DCOLOR_XRGB(255, 255, 0), },
			{ D3DCOLOR_XRGB(255, 0, 255), },

			{ D3DCOLOR_XRGB(255, 255, 0), },
			{ D3DCOLOR_XRGB(0, 255, 255), },
			{ D3DCOLOR_XRGB(255, 0, 255), },
		};
		vec2_t texcoords[] =
		{
			{ 0.0f, 1.0f, },
			{ 0.5f, 0.0f, },
			{ 0.0f, 1.0f, },
			{ 1.0f, 1.0f, },

			{ 1.0f, 1.0f, },
			{ 1.0f, 1.0f, },
			{ 0.0f, 1.0f, },
		};

		struct { const void *ptr; uint32_t sz; } ptrs_data[3] =
		{
			{verts, sizeof(verts),},
			{cols, sizeof(cols),},
			{texcoords, sizeof(texcoords),},
		};


		VOID* pVoid;

		for (int i = 0; i < 3; i++)
		{
			device->CreateVertexBuffer(ptrs_data[i].sz,
				0,
				0,
				D3DPOOL_MANAGED,
				&v_buffer[i],
				NULL);

			v_buffer[i]->Lock(0, 0, (void**)&pVoid, 0);
			memcpy(pVoid, ptrs_data[i].ptr, ptrs_data[i].sz);
			v_buffer[i]->Unlock();
		}

		short indices[] =
		{
			1, 3, 2,
			1, 4, 0,
			1, 5, 6,
			5, 4, 3
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

		HRESULT res;
		
		for (int i = 0; i < 4; i++)
		{
			char fname[42]; snprintf(fname, sizeof(fname), "%sflames%d.jpg", RSRCDIR, i + 1);
			res = D3DXCreateTextureFromFileA(device, fname, &texture[i]);
			if (FAILED(res))
			{
				std::cout << "Failed loading texture: " << res << std::endl;
				return -1;
			}
		}

		device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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

		struct timespec start;
		timespec_get(&start, TIME_UTC);
		float timeValue = (float)start.tv_nsec / 100000000 + (start.tv_sec % 10) * 10;
		int texnum = int((sin(timeValue) * 1.5f) + 1.6f); //[0.1-3.1]
		//printf("%g %d\n", timeValue, texnum);

		device->BeginScene();

		// select which vertex format we are using
		device->SetVertexDeclaration(vertexDecl);

		// SET UP THE PIPELINE

		// set the view transform
		D3DXMATRIX matView;    // the view transform matrix
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f, 3.0f, -8.0f),   // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
		device->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

		// set the projection transform
		D3DXMATRIX matProjection;    // the projection transform matrix
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(90),    // the horizontal field of view
			dx_state1.aspect_ratio, // aspect ratio
			1.0f,    // the near view-plane
			100.0f);    // the far view-plane
		device->SetTransform(D3DTS_PROJECTION, &matProjection);     // set the projection


		// select the vertex buffer to display
		device->SetStreamSource(0, v_buffer[0], 0, sizeof(vec3_t));
		device->SetStreamSource(1, v_buffer[1], 0, sizeof(D3DCOLOR));
		device->SetStreamSource(2, v_buffer[2], 0, sizeof(vec2_t));
		device->SetIndices(i_buffer);
		device->SetTexture(0, texture[texnum]);

		D3DXMATRIX matTranslateA;    // a matrix to store the translation for mesh A
		D3DXMATRIX matRotateY;    // a matrix to store the rotation for each mesh
		static float index = 0.0f;
		index += 0.01f; // an ever-increasing float value

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

		D3DXMatrixTranslation(&matTranslateA, x, y, z);
		D3DXMatrixRotationY(&matRotateY, index);    // the front side

		device->SetTransform(D3DTS_WORLD, &(matRotateY));
		// draw the cube
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 4);

		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}
};

Demo *factory_multibufpyramid_create()
{
	return new MultiBuffPyramid();
}
