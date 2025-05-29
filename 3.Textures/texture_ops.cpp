
#include "textures.h"

using std::cout;
using std::endl;

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
	LPDIRECT3DTEXTURE9 texture[5];

	LPDIRECT3D9 _d3d;
	LPDIRECT3DDEVICE9 _device;

	bool with_shaders;
	int framecount;
public:
	TextureOps() : Demo(), v_buffer(0), vs(0), ps(0), constants(0), texture(), _d3d(0), _device(0),
		with_shaders(false), framecount() {}

	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		HRESULT res;

		CUSTOMVERTEX t_vert[] =
		{
			{ 0.0f, 3.46f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 0.5f, 0.0f, },
			{ 3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 1.0f, },
			{ -3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 1.0f, },

			{ 4.0f, 4.f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0.0f, },
			{ 4.0f, -4.f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 1.0f, },
			{ -4.0f, 4.f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0.0f, },
			{ -4.0f, -4.f, 0.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 1.0f, },

			{ 20.0f, 1.f, 0.0f, D3DCOLOR_XRGB(255, 255, 0), 10.0f, 0.0f, },
			{ 20.0f, -1.f, 0.0f, D3DCOLOR_XRGB(255, 255, 0), 10.0f, 1.0f, },
			{ -20.0f, 1.f, 0.0f, D3DCOLOR_XRGB(255, 255, 0), 0.0f, 0.0f, },
			{ -20.0f, -1.f, 0.0f, D3DCOLOR_XRGB(255, 255, 0), 0.0f, 1.0f, },

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
				cout << "Failed compilation: " << HRESULT_FACILITY(res) << " " << HRESULT_CODE(res) << endl;
				return -1;
			}

			res = D3DXCompileShaderFromFileA(RSRC("tex_ops.hlsl"), nullptr, nullptr, "main", "ps_3_0", 0, &pixelShaderBuffer, nullptr, nullptr);
			if (FAILED(res))
			{
				cout << "Failed compilation: " << HRESULT_FACILITY(res) << " " << HRESULT_CODE(res) << endl;
				return -1;
			}

			device->CreateVertexShader((DWORD*)vertexShaderBuffer->GetBufferPointer(), &vs);
			device->CreatePixelShader((DWORD*)pixelShaderBuffer->GetBufferPointer(), &ps);

			vertexShaderBuffer->Release();
			pixelShaderBuffer->Release();
		}

		res = D3DXCreateTextureFromFileA(device, RSRC("castle_walls_short.jpg"), &texture[0]);
		if (FAILED(res))
		{
			cout << "Failed loading texture 0 code: " << HRESULT_CODE(res) << endl;
			return -1;
		}
		cout << "Tex0 lvls:" << texture[0]->GetLevelCount() << endl;

		D3DERR_INVALIDCALL;
		const int size = 256;
		DWORD usage = D3DUSAGE_AUTOGENMIPMAP /*| D3DUSAGE_DYNAMIC*/;
		//D3DPOOL pool = D3DPOOL_DEFAULT;
		D3DPOOL pool = D3DPOOL_MANAGED;
		for (int i = 1; i < ARRAYSIZE(texture); i++)
		{
			res = device->CreateTexture(size, size, 0, usage, D3DFMT_A8R8G8B8, pool, &texture[i], NULL);
			if (FAILED(res))
			{
				cout << "Failed loading texture " << i << " code: " << HRESULT_CODE(res) << endl;
				return -1;
			}
		}

		cout << "AutoMipmap support: " << (D3D_OK == d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8,
			D3DUSAGE_AUTOGENMIPMAP, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8)) << endl;

		D3DLOCKED_RECT lr;
		res = texture[1]->LockRect(0, &lr, NULL, 0);
		if (SUCCEEDED(res))
		{
			uint32_t *data = (uint32_t*)lr.pBits;
			memset(data, 255, size * size * 4);
			for (int x = size / 5; x < size * 4 / 5; x++)
			{
				int scaledx = int(float(x) / size * 255);
				for (int y = size / 5; y < size * 4 / 5; y++)
				{
					int scaledy = int(float(y) / size * 255);
					int r = scaledx;
					int g = 0;
					int b = 0;
					int a = 0;
					data[x*size + y] = D3DCOLOR_ARGB(a, r, g, b);
				}
			}
			texture[1]->UnlockRect(0);
			texture[1]->GenerateMipSubLevels();
		}
		cout << "Tex1 lvls:" << texture[1]->GetLevelCount() << endl;

		res = texture[2]->LockRect(0, &lr, NULL, 0);
		if (SUCCEEDED(res))
		{
			uint32_t *data = (uint32_t*)lr.pBits;
			memset(data, 255, size * size * 4);
			for (int x = size / 5; x < size * 4 / 5; x++)
			{
				int scaledx = int(float(x) / size * 255);
				for (int y = size / 5; y < size * 4 / 5; y++)
				{
					int scaledy = int(float(y) / size * 255);
					int r = 255;
					int g = 0;
					int b = 0;
					int a = 0;
					data[x*size + y] = D3DCOLOR_ARGB(a, r, g, b);
				}
			}
			texture[2]->UnlockRect(0);
			texture[2]->GenerateMipSubLevels();
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

		static float scaling = 0.6f;
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

		if(with_shaders)
		{
			device->SetVertexShader(vs);
			device->SetPixelShader(ps);
		}

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
		if (!with_shaders)    device->SetTransform(D3DTS_VIEW, &matView);
		else                  constants->SetMatrix(device, "viewMatrix", &matView);

		// set the projection transform
		D3DXMATRIX matProjection;
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),
			dx_state1.aspect_ratio,
			1.0f,    // the near view-plane
			1000.0f);    // the far view-plane
		if (!with_shaders)    device->SetTransform(D3DTS_PROJECTION, &matProjection);
		else                  constants->SetMatrix(device, "projectionMatrix", &matProjection);

		device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));


		device->SetTexture(0, texture[0]);

		if (framecount >= 2 * 60) { framecount = 0; }
		if (framecount >= 1 * 60)
		{
			device->SetTexture(1, texture[2]);
		}
		else
		{
			device->SetTexture(1, texture[1]);
		}

		device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
		//device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		//device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

		static int print_once = 1;
		if (print_once)
		{
			print_once--;
			DWORD tmp;
			device->GetTextureStageState(0, D3DTSS_RESULTARG, &tmp);
			printf("resultarg %d %d\n", tmp, D3DTA_CURRENT);
			device->GetTextureStageState(0, D3DTSS_COLORARG1, &tmp);
			printf("colorarg1 %d %d\n", tmp, D3DTA_TEXTURE);
			device->GetTextureStageState(0, D3DTSS_COLORARG2, &tmp);
			printf("colorarg2 %d %d\n", tmp, D3DTA_CURRENT);
			device->GetTextureStageState(0, D3DTSS_ALPHAARG1, &tmp);
			printf("alphaarg1 %d %d\n", tmp, D3DTA_TEXTURE);
			device->GetTextureStageState(0, D3DTSS_ALPHAARG2, &tmp);
			printf("alphaarg2 %d %d\n", tmp, D3DTA_CURRENT);
			device->GetSamplerState(0, D3DSAMP_BORDERCOLOR, &tmp);
			printf("bordercolor %x\n", tmp);
		}

		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 4);
		device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, 4);


		// set the world transform
		D3DXMATRIX matRotate;
		D3DXMATRIX matScale;
		D3DXMATRIX matScratch;
		D3DXMATRIX matTranslate;
		D3DXMATRIX matWorld;
		D3DXMatrixScaling(&matScale, scaling, scaling, scaling);

		//draw triangle
		D3DXMatrixTranslation(&matTranslate, -3.0f, 0.5f, 00.0f);
		matWorld = matScale * matTranslate;
		if (!with_shaders)    device->SetTransform(D3DTS_WORLD, &(matWorld));
		else                  constants->SetMatrix(device, "worldMatrix", &(matWorld));

		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		//draw square
		D3DXMatrixTranslation(&matTranslate, 3.0f, 1.0f, 00.0f);
		matWorld = matScale * matTranslate;
		if (!with_shaders)    device->SetTransform(D3DTS_WORLD, &(matWorld));
		else                  constants->SetMatrix(device, "worldMatrix", &(matWorld));

		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 3, 2);

		//draw background rectangle
		D3DXMatrixTranslation(&matWorld, 0.0f, 1.0f, 1.0f);
		if (!with_shaders)    device->SetTransform(D3DTS_WORLD, &(matWorld));
		else                  constants->SetMatrix(device, "worldMatrix", &(matWorld));

		device->SetTexture(0, 0);
		device->SetTexture(1, 0);

		//device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 7, 2);

		//draw textured rectangle
		D3DXMatrixRotationX(&matRotate, D3DXToRadian(90));
		matRotate *= *D3DXMatrixRotationY(&matScratch, D3DXToRadian(90));
		matRotate *= *D3DXMatrixRotationX(&matScratch, D3DXToRadian(-15));
		D3DXMatrixTranslation(&matTranslate, 0.0f, 2.5f, 15.0f);
		matWorld = matRotate * matTranslate;
		if (!with_shaders)    device->SetTransform(D3DTS_WORLD, &(matWorld));
		else                  constants->SetMatrix(device, "worldMatrix", &(matWorld));

		device->SetTexture(0, texture[0]);
		static D3DTEXTUREFILTERTYPE ftg = D3DTEXF_POINT;
		static D3DTEXTUREFILTERTYPE ftn = D3DTEXF_POINT;
		static D3DTEXTUREFILTERTYPE ftp = D3DTEXF_POINT;
		if (key.p)
		{
			if ( key.ctrl)
			{
				ftp = D3DTEXTUREFILTERTYPE(1 + (int)ftp);
				if (ftp > D3DTEXF_LINEAR) ftp = D3DTEXF_POINT;
			}
			else if (key.shift)
			{
				ftg = D3DTEXTUREFILTERTYPE(1 + (int)ftg);
				if (ftg > D3DTEXF_ANISOTROPIC) ftg = D3DTEXF_POINT;
			}
			else
			{
				ftn = D3DTEXTUREFILTERTYPE(1 + (int)ftn);
				if (ftn > D3DTEXF_ANISOTROPIC) ftn = D3DTEXF_POINT;
			}
		}
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, ftg);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, ftn);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, ftp);
		device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 4);
		device->SetTexture(1, 0);


		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 7, 2);

		device->EndScene();    // ends the 3D scene

		HRESULT hr = device->Present(NULL, NULL, NULL, NULL);    // displays the created frame
		if (FAILED(hr))
		{
			//cout << "Present err: " << HRESULT_CODE(hr) << endl;
		}

		framecount++;

		return 0;
	}

	virtual ~TextureOps()
	{
		if (init_done)
		{
			_device->SetVertexShader(0);
			_device->SetPixelShader(0);
			_device->SetTexture(0, 0);
			_device->SetTexture(1, 0);
		}
	}
};

Demo *factory_texture_ops_create()
{
	return new TextureOps();
}