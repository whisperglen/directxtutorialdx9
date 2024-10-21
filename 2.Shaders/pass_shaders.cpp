#include "shaders.h"

struct CUSTOMVERTEX
{
	FLOAT X, Y, Z;
	DWORD COLOR;
};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

class PassShaders: public Demo
{
	LPDIRECT3DVERTEXBUFFER9 v_buffer;
	LPDIRECT3DVERTEXSHADER9 vs;
	LPDIRECT3DPIXELSHADER9 ps;
	LPD3DXCONSTANTTABLE constants;

	LPDIRECT3D9 _d3d;
	LPDIRECT3DDEVICE9 _device;
public:
	PassShaders() : Demo(), v_buffer(0) {}

	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		CUSTOMVERTEX t_vert[] =
		{
			{ 0.0f, 3.46f, 0.0f, D3DCOLOR_XRGB(255, 0, 255), },
			{ 3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(255, 255, 0), },
			{ -3.0f, -1.73f, 0.0f, D3DCOLOR_XRGB(0, 255, 255), },
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

		ID3DXBuffer* vertexShaderBuffer{ };
		ID3DXBuffer* pixelShaderBuffer{ };

		HRESULT compres;
		compres = D3DXCompileShaderFromFileA(RSRC("pass_vertex.hlsl"), nullptr, nullptr, "main", "vs_3_0", 0, &vertexShaderBuffer, nullptr, &constants);
		if (FAILED(compres))
		{
			std::cout << "Failed compilation: " << compres << std::endl;
			return -1;
		}

		compres = D3DXCompileShaderFromFileA(RSRC("pass_pixel.hlsl"), nullptr, nullptr, "main", "ps_3_0", 0, &pixelShaderBuffer, nullptr, nullptr);
		if (FAILED(compres))
		{
			std::cout << "Failed compilation: " << compres << std::endl;
			return -1;
		}

		device->CreateVertexShader((DWORD*)vertexShaderBuffer->GetBufferPointer(), &vs);
		device->CreatePixelShader((DWORD*)pixelShaderBuffer->GetBufferPointer(), &ps);

		vertexShaderBuffer->Release();
		pixelShaderBuffer->Release();

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
		//device->SetTransform(D3DTS_VIEW, &matView);
		constants->SetMatrix(device, "viewMatrix", &matView);

		// set the projection transform
		D3DXMATRIX matProjection;
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),
			(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT,
			1.0f,    // the near view-plane
			1000.0f);    // the far view-plane
		//device->SetTransform(D3DTS_PROJECTION, &matProjection);
		constants->SetMatrix(device, "projectionMatrix", &matProjection);


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
		//device->SetTransform(D3DTS_WORLD, &(matTranslate));    // set the world transform
		constants->SetMatrix(device, "worldMatrix", &matTranslate);

		device->SetVertexShader(vs);
		device->SetPixelShader(ps);

		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		device->EndScene();    // ends the 3D scene

		device->Present(NULL, NULL, NULL, NULL);    // displays the created frame

		return 0;
	}

	virtual ~PassShaders()
	{
		if (init_done)
		{
			_device->SetVertexShader(0);
			_device->SetPixelShader(0);
		}
	}
};

Demo *factory_pass_shaders_create()
{
	return new PassShaders();
}