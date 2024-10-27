
#include "textures.h"

struct CUSTOMVERTEX
{
	FLOAT X, Y, Z;
	DWORD COLOR;
	FLOAT U, V;
};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0))

class TextureOps : public Demo
{
	LPDIRECT3DVERTEXBUFFER9 v_buffer;
	LPDIRECT3DVERTEXSHADER9 vs;
	LPDIRECT3DPIXELSHADER9 ps;
	LPD3DXCONSTANTTABLE constants;
	LPDIRECT3DTEXTURE9 texture[2];

	LPDIRECT3D9 _d3d;
	LPDIRECT3DDEVICE9 _device;

	bool with_shaders;
public:
	TextureOps() : Demo(), v_buffer(0), vs(0), ps(0), constants(0), texture(), _d3d(0), _device(0), with_shaders(false) {}

	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		HRESULT res;

		CUSTOMVERTEX t_vert[] =
		{
			{ 0.0f, 3.46f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 0.5f, 0.0f, },
			{ 3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 1.0f, },
			{ -3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 1.0f, },
		};

		// create the vertex and store the pointer into v_buffer, which is created globally
		device->CreateVertexBuffer(sizeof(t_vert),
			0,
			CUSTOMFVF,
			D3DPOOL_MANAGED,
			&v_buffer,
			NULL);

		VOID* pVoid;    // the void pointer

		v_buffer->Lock(0, 0, (void**)&pVoid, 0);    // lock the vertex buffer
		memcpy(pVoid, t_vert, sizeof(t_vert));    // copy the vertices to the locked buffer
		v_buffer->Unlock();    // unlock the vertex buffer

		if (with_shaders)
		{
			ID3DXBuffer* vertexShaderBuffer{ };
			ID3DXBuffer* pixelShaderBuffer{ };

			res = D3DXCompileShaderFromFileA(RSRC("tex_vertex.hlsl"), nullptr, nullptr, "main", "vs_3_0", 0, &vertexShaderBuffer, nullptr, &constants);
			if (FAILED(res))
			{
				std::cout << "Failed compilation: " << HRESULT_FACILITY(res) << " " << HRESULT_CODE(res) << std::endl;
				return -1;
			}

			res = D3DXCompileShaderFromFileA(RSRC("tex_ops.hlsl"), nullptr, nullptr, "main", "ps_3_0", 0, &pixelShaderBuffer, nullptr, nullptr);
			if (FAILED(res))
			{
				std::cout << "Failed compilation: " << HRESULT_FACILITY(res) << " " << HRESULT_CODE(res) << std::endl;
				return -1;
			}

			device->CreateVertexShader((DWORD*)vertexShaderBuffer->GetBufferPointer(), &vs);
			device->CreatePixelShader((DWORD*)pixelShaderBuffer->GetBufferPointer(), &ps);

			vertexShaderBuffer->Release();
			pixelShaderBuffer->Release();
		}

		D3DERR_INVALIDCALL;
		const int size = 256;
		DWORD usage = D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_DYNAMIC;
		//D3DPOOL pool = D3DPOOL_DEFAULT;
		D3DPOOL pool = D3DPOOL_MANAGED;
		for (int i = 0; i < 2; i++)
		{
			res = device->CreateTexture(size, size, 1, 0, D3DFMT_A8R8G8B8, pool, &texture[i], NULL);
			if (FAILED(res))
			{
				std::cout << "Failed loading texture " << i << " code: " << HRESULT_CODE(res) << std::endl;
				return -1;
			}
		}

		D3DLOCKED_RECT lr;
		res = texture[0]->LockRect(0, &lr, NULL, 0);
		if (SUCCEEDED(res))
		{
			uint32_t *data = (uint32_t*)lr.pBits;
			for (int x = 0; x < size; x++)
			{
				int scaledx = float(x) / size * 255;
				for (int y = 0; y < size; y++)
				{
					int scaledy = float(y) / size * 255;
					int r = scaledx;
					int g = 0;
					int b = 0;
					int a = scaledy;
					data[x*size + y] = D3DCOLOR_ARGB(a, r, g, b);
				}
			}
			texture[0]->UnlockRect(0);
		}

		res = texture[1]->LockRect(0, &lr, NULL, 0);
		if (SUCCEEDED(res))
		{
			uint32_t *data = (uint32_t*)lr.pBits;
			for (int x = 0; x < size; x++)
			{
				int scaledx = float(x) / size * 255;
				for (int y = 0; y < size; y++)
				{
					int scaledy = float(y) / size * 255;
					int r = 0;
					int g = scaledy;
					int b = 0;
					int a = 255;
					data[x*size + y] = D3DCOLOR_ARGB(a, r, g, b);
				}
			}
			texture[1]->UnlockRect(0);
		}

		device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer

		_d3d = d3d;
		_device = device;

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

		device->BeginScene();    // begins the 3D scene

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		D3DXMATRIX camRotate;
		D3DXMATRIX scratch;
		D3DXMatrixRotationX(&camRotate, rotx);
		camRotate *= *D3DXMatrixRotationY(&scratch, roty);
		camRotate *= *D3DXMatrixRotationZ(&scratch, rotz);
		D3DXVECTOR3 camerapos, cameraup;

		D3DXVec3TransformCoord(&camerapos, &D3DXVECTOR3(0.0f, 0.0f, -10.0f), &camRotate);
		D3DXVec3TransformCoord(&cameraup, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &camRotate);
		// set the view transform
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView,
			&camerapos,    // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),      // the look-at position
			&cameraup);    // the up direction
		if (!with_shaders)
		{
			device->SetTransform(D3DTS_VIEW, &matView);
		}
		else
		{
			constants->SetMatrix(device, "viewMatrix", &matView);
		}

		// set the projection transform
		D3DXMATRIX matProjection;
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),
			dx_state1.aspect_ratio,
			1.0f,    // the near view-plane
			1000.0f);    // the far view-plane
		if (!with_shaders)
		{
			device->SetTransform(D3DTS_PROJECTION, &matProjection);
		}
		else
		{
			constants->SetMatrix(device, "projectionMatrix", &matProjection);
		}


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
		device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

		D3DXMATRIX matTranslate;
		D3DXMatrixTranslation(&matTranslate, 0.0f, 0.0f, 00.0f);
		if (!with_shaders)
		{
			device->SetTransform(D3DTS_WORLD, &(matTranslate));    // set the world transform
		}
		else
		{
			constants->SetMatrix(device, "worldMatrix", &matTranslate);

			device->SetVertexShader(vs);
			device->SetPixelShader(ps);
		}

		device->SetTexture(0, texture[0]);
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 4);

		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		device->EndScene();    // ends the 3D scene

		device->Present(NULL, NULL, NULL, NULL);    // displays the created frame

		return 0;
	}

	virtual ~TextureOps()
	{
		if (init_done)
		{
			_device->SetVertexShader(0);
			_device->SetPixelShader(0);
			_device->SetTexture(0, 0);
		}
	}
};

Demo *factory_texture_ops_create()
{
	return new TextureOps();
}