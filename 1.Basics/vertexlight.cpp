
#include "basics.h"

struct CUSTOMVERTEX { FLOAT X, Y, Z; D3DVECTOR NORMAL; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL)

static void create_cube(LPDIRECT3DDEVICE9 device, LPDIRECT3DVERTEXBUFFER9 &v_buffer, LPDIRECT3DINDEXBUFFER9 &i_buffer)
{
	CUSTOMVERTEX t_vert[] =
	{
		{ -3.0f, -3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },    // side 1
		{ 3.0f, -3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },
		{ -3.0f, 3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },
		{ 3.0f, 3.0f, 3.0f, 0.0f, 0.0f, 1.0f, },

		{ -3.0f, -3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },    // side 2
		{ -3.0f, 3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },
		{ 3.0f, -3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },
		{ 3.0f, 3.0f, -3.0f, 0.0f, 0.0f, -1.0f, },

		{ -3.0f, 3.0f, -3.0f, 0.0f, 1.0f, 0.0f, },    // side 3
		{ -3.0f, 3.0f, 3.0f, 0.0f, 1.0f, 0.0f, },
		{ 3.0f, 3.0f, -3.0f, 0.0f, 1.0f, 0.0f, },
		{ 3.0f, 3.0f, 3.0f, 0.0f, 1.0f, 0.0f, },

		{ -3.0f, -3.0f, -3.0f, 0.0f, -1.0f, 0.0f, },    // side 4
		{ 3.0f, -3.0f, -3.0f, 0.0f, -1.0f, 0.0f, },
		{ -3.0f, -3.0f, 3.0f, 0.0f, -1.0f, 0.0f, },
		{ 3.0f, -3.0f, 3.0f, 0.0f, -1.0f, 0.0f, },

		{ 3.0f, -3.0f, -3.0f, 1.0f, 0.0f, 0.0f, },    // side 5
		{ 3.0f, 3.0f, -3.0f, 1.0f, 0.0f, 0.0f, },
		{ 3.0f, -3.0f, 3.0f, 1.0f, 0.0f, 0.0f, },
		{ 3.0f, 3.0f, 3.0f, 1.0f, 0.0f, 0.0f, },

		{ -3.0f, -3.0f, -3.0f, -1.0f, 0.0f, 0.0f, },    // side 6
		{ -3.0f, -3.0f, 3.0f, -1.0f, 0.0f, 0.0f, },
		{ -3.0f, 3.0f, -3.0f, -1.0f, 0.0f, 0.0f, },
		{ -3.0f, 3.0f, 3.0f, -1.0f, 0.0f, 0.0f, },
	};

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

	// create the indices using an int array
	short indices[] =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 5, 6,    // side 2
		6, 5, 7,
		8, 9, 10,    // side 3
		10, 9, 11,
		12, 13, 14,    // side 4
		14, 13, 15,
		16, 17, 18,    // side 5
		18, 17, 19,
		20, 21, 22,    // side 6
		22, 21, 23,
	};

	// create an index buffer interface called i_buffer
	device->CreateIndexBuffer(36 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&i_buffer,
		NULL);

	// lock i_buffer and load the indices into it
	i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indices, sizeof(indices));
	i_buffer->Unlock();
}

class VertexLight : public Demo
{
	LPDIRECT3DVERTEXBUFFER9 v_buffer;
	LPDIRECT3DINDEXBUFFER9 i_buffer;
public:
	VertexLight() : Demo() {}
	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		create_cube(device, v_buffer, i_buffer);

		D3DLIGHT9 light;    // create the light struct

		ZeroMemory(&light, sizeof(light));    // clear out the light struct for use
		light.Type = D3DLIGHT_DIRECTIONAL;    // make the light type 'directional light'
		light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);    // set the light's color
		light.Direction = D3DXVECTOR3(1.0f, -0.8f, 1.0f);

		device->SetLight(0, &light);    // send the light struct properties to light #0
		device->LightEnable(0, TRUE);    // turn on light #0


		D3DMATERIAL9 material;    // create the material struct

		ZeroMemory(&material, sizeof(D3DMATERIAL9));    // clear out the struct for use
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set diffuse color to white
		material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set ambient color to white

		device->SetMaterial(&material);    // set the globably-used material to &material

		device->SetRenderState(D3DRS_LIGHTING, TRUE);    // turn on the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
		//device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));    // ambient light

		init_done = 1;
		return 0;
	}

	virtual int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == 0)
			if (0 != do_init(d3d, device))
				return -1;

		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);
		device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		union inputs key = get_keypress();

		static int light_enabled = TRUE;
		if (key.t)
		{
			light_enabled = !light_enabled;
		}

		device->LightEnable(0, light_enabled);    // turn on light #0

		static float scaling = 1.0f;
		if (key.vert && key.ctrl)
		{
			//scaling += 0.05f * key.vert;
		}

		static float posx = 0.f, posy = 0.f, posz = 0.f;
		if (key.horiz)
		{
			posx += 1.0f * key.horiz;
		}
		if (key.vert)
		{
			if (key.ctrl)
				posz += 1.0f * key.vert;
			else
				posy += 1.0f * key.vert;
		}

		device->BeginScene();

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		// set the view transform
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f, 8.0f, -25.0f),    // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),      // the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
		device->SetTransform(D3DTS_VIEW, &matView);

		// set the projection transform
		D3DXMATRIX matProjection;
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),
			dx_state1.aspect_ratio,
			1.0f,    // the near view-plane
			100.0f);    // the far view-plane
		device->SetTransform(D3DTS_PROJECTION, &matProjection);

		// set the world transform
		static float index = 0.0f; index += 0.03f;
		D3DXMATRIX matRotateY;
		D3DXMATRIX matScale;
		D3DXMATRIX matTranslate;
		D3DXMatrixRotationY(&matRotateY, index);
		D3DXMatrixScaling(&matScale, scaling, scaling, scaling);
		D3DXMatrixTranslation(&matTranslate, posx, posy, posz);
		device->SetTransform(D3DTS_WORLD, &(matRotateY * matScale * matTranslate));

		// select the vertex and index buffers to use
		device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		device->SetIndices(i_buffer);

		// draw the cube
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}
};

Demo *factory_vertexlight_create()
{
	return new VertexLight();
}

class PointLight : public Demo
{
	LPDIRECT3DVERTEXBUFFER9 v_buffer;
	LPDIRECT3DINDEXBUFFER9 i_buffer;
public:
	PointLight() : Demo() {}
	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		create_cube(device, v_buffer, i_buffer);

		D3DLIGHT9 light;    // create the light struct

		ZeroMemory(&light, sizeof(light));    // clear out the light struct for use
		light.Type = D3DLIGHT_POINT;    // make the light type 'directional light'
		light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);    // set the light's color
		light.Position = D3DXVECTOR3(10.0f, 5.0f, 0.0f);
		light.Range = 100.0f;    // a range of 100
		light.Attenuation0 = 0.0f;    // no constant inverse attenuation
		light.Attenuation1 = 0.125f;    // only .125 inverse attenuation
		light.Attenuation2 = 0.0f;    // no square inverse attenuation

		device->SetLight(0, &light);    // send the light struct properties to light #0
		device->LightEnable(0, TRUE);    // turn on light #0


		D3DMATERIAL9 material;    // create the material struct

		ZeroMemory(&material, sizeof(D3DMATERIAL9));    // clear out the struct for use
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set diffuse color to white
		material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set ambient color to white

		device->SetMaterial(&material);    // set the globably-used material to &material

		device->SetRenderState(D3DRS_LIGHTING, TRUE);    // turn on the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
		device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));    // ambient light

		init_done = 1;
		return 0;
	}

	virtual int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == 0)
			if (0 != do_init(d3d, device))
				return -1;

		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);
		device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		union inputs key = get_keypress();

		static int light_enabled = TRUE;
		if (key.t)
		{
			light_enabled = !light_enabled;
		}

		device->LightEnable(0, light_enabled);    // turn on light #0

		static float scaling = 1.0f;
		if (key.vert && key.ctrl)
		{
			scaling += 0.05f * key.vert;
		}

		device->BeginScene();

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		// set the view transform
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f, 8.0f, 25.0f),    // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),      // the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
		device->SetTransform(D3DTS_VIEW, &matView);

		// set the projection transform
		D3DXMATRIX matProjection;
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),
			dx_state1.aspect_ratio,
			1.0f,    // the near view-plane
			100.0f);    // the far view-plane
		device->SetTransform(D3DTS_PROJECTION, &matProjection);

		// set the world transform
		static float index = 0.0f; index += 0.03f;
		D3DXMATRIX matRotateY;
		D3DXMATRIX matScale;
		D3DXMatrixRotationY(&matRotateY, index);
		D3DXMatrixScaling(&matScale, scaling, scaling, scaling);
		device->SetTransform(D3DTS_WORLD, &(matRotateY * matScale));

		// select the vertex and index buffers to use
		device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		device->SetIndices(i_buffer);

		// draw the cube
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}
};

Demo *factory_pointlight_create()
{
	return new PointLight();
}

class SpotLight : public Demo
{
	LPDIRECT3DVERTEXBUFFER9 v_buffer;
	LPDIRECT3DINDEXBUFFER9 i_buffer;
public:
	SpotLight() : Demo() {}
	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		create_cube(device, v_buffer, i_buffer);

		D3DLIGHT9 light;    // create the light struct

		ZeroMemory(&light, sizeof(light));    // clear out the light struct for use
		light.Type = D3DLIGHT_SPOT;    // make the light type 'spot light'
		light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		light.Position = D3DXVECTOR3(-12.0f, 0.0f, 30.0f);
		light.Direction = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		light.Range = 100.0f;    // a range of 100
		light.Attenuation0 = 0.0f;    // no constant inverse attenuation
		light.Attenuation1 = 0.125f;    // only .125 inverse attenuation
		light.Attenuation2 = 0.0f;    // no square inverse attenuation
		light.Phi = D3DXToRadian(40.0f);    // set the outer cone to 30 degrees
		light.Theta = D3DXToRadian(20.0f);    // set the inner cone to 10 degrees
		light.Falloff = 1.0f;    // use the typical falloff

		device->SetLight(0, &light);    // send the light struct properties to light #0
		device->LightEnable(0, TRUE);    // turn on light #0


		D3DMATERIAL9 material;    // create the material struct

		ZeroMemory(&material, sizeof(D3DMATERIAL9));    // clear out the struct for use
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set diffuse color to white
		material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set ambient color to white

		device->SetMaterial(&material);    // set the globably-used material to &material

		device->SetRenderState(D3DRS_LIGHTING, TRUE);    // turn on the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
		device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));    // ambient light

		init_done = 1;
		return 0;
	}

	virtual int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == 0)
			if (0 != do_init(d3d, device))
				return -1;

		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);
		device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		union inputs key = get_keypress();

		static int light_enabled = TRUE;
		if (key.t)
		{
			light_enabled = !light_enabled;
		}

		device->LightEnable(0, light_enabled);    // turn on light #0

		static float scaling = 1.0f;
		if (key.vert && key.ctrl)
		{
			scaling += 0.05f * key.vert;
		}

		device->BeginScene();

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		// set the view transform
		D3DXMATRIX matView;    // the view transform matrix
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(-30.0f, 20.0f, 30.0f),    // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
		device->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView 

		// set the projection transform
		D3DXMATRIX matProjection;    // the projection transform matrix
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),    // the horizontal field of view
			dx_state1.aspect_ratio, // aspect ratio
			1.0f,    // the near view-plane
			100.0f);    // the far view-plane
		device->SetTransform(D3DTS_PROJECTION, &matProjection);    // set the projection

		// set the world transform
		static float index = 0.0f; index += 0.03f;    // an ever-increasing float value
		D3DXMATRIX matTranslate;    // the world transform matrix
		D3DXMatrixTranslation(&matTranslate,
			(float)sin(index) * 12.0f, 0.0f, (float)cos(index) * 6.0f);
		device->SetTransform(D3DTS_WORLD, &(matTranslate));    // set the world transform

		// select the vertex and index buffers to use
		device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		device->SetIndices(i_buffer);

		// draw the cube
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}
};

Demo *factory_spotlight_create()
{
	return new SpotLight();
}

class LightBox : public Demo
{
	struct mesh_obj objects[5];
public:
	LightBox() : Demo() {};
	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		load_mesh("shapes.obj", "Object.1", device, objects[0]);
		load_mesh("shapes.obj", "Object.2", device, objects[1]);
		load_mesh("shapes.obj", "Object.3", device, objects[2]);
		load_mesh("shapes.obj", "Object.4", device, objects[3]);
		load_mesh("shapes.obj", "Object.5", device, objects[4]);

		D3DLIGHT9 light;    // create the light struct

		ZeroMemory(&light, sizeof(light));    // clear out the light struct for use
		light.Type = D3DLIGHT_DIRECTIONAL;    // make the light type 'directional light'
		light.Diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);    // set the light's color
		light.Direction = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
#if 0
		light.Type = D3DLIGHT_SPOT;    // make the light type 'directional light'
		light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);    // set the light's color
		light.Position = D3DXVECTOR3(-12.0f, 0.0f, 30.0f);
		light.Direction = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		light.Range = 100.0f;    // a range of 100
		light.Attenuation0 = 0.0f;    // no constant inverse attenuation
		light.Attenuation1 = 0.125f;    // only .125 inverse attenuation
		light.Attenuation2 = 0.0f;    // no square inverse attenuation
		light.Phi = D3DXToRadian(40.0f);    // set the outer cone to 30 degrees
		light.Theta = D3DXToRadian(20.0f);    // set the inner cone to 10 degrees
		light.Falloff = 1.0f;    // use the typical falloff
#endif

		device->SetLight(0, &light);    // send the light struct properties to light #0
		device->LightEnable(0, TRUE);    // turn on light #0

		device->SetRenderState(D3DRS_LIGHTING, TRUE);    // turn on the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
		device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		//device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(70, 70, 70));    // ambient light

		init_done = 1;
		return 0;
	}

	virtual int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == 0)
			if (0 != do_init(d3d, device))
				return -1;

		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);
		device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		union inputs key = get_keypress();

		static int light_enabled = TRUE;
		if (key.t)
		{
			light_enabled = !light_enabled;
		}

		device->LightEnable(0, light_enabled);    // turn on light #0

		static float scaling = 1.0f;
		if (key.vert && key.ctrl)
		{
			scaling += 0.05f * key.vert;
		}

		static float rotate = D3DXToRadian(0);
		if (key.horiz)
		{
			rotate += 0.1f * key.horiz;
		}

		device->BeginScene();

		// select which vertex format we are using
		device->SetFVF(CUSTOMFVF);

		// set the view transform
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f, 20.0f, -50.0f),    // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),      // the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
		device->SetTransform(D3DTS_VIEW, &matView);

		// set the projection transform
		D3DXMATRIX matProjection;
		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(45),
			dx_state1.aspect_ratio,
			1.0f,    // the near view-plane
			1000.0f);    // the far view-plane
		device->SetTransform(D3DTS_PROJECTION, &matProjection);


		static float index = 0.0f; index += 0.03f;

		for (int i = 0; i < 5; i++)
		{
			if (objects[i].v_buffer == 0) continue;
			// set the world transform
			D3DXMATRIX matRotate;
			D3DXMATRIX matScale;
			D3DXMATRIX matScratch;
			D3DXMatrixIdentity(&matRotate); D3DXMatrixIdentity(&matScale);
			//D3DXMatrixRotationX(&matRotate, D3DXToRadian(-90));
			D3DXMatrixRotationY(&matScratch, rotate);
			matRotate *= matScratch;
			D3DXMatrixScaling(&matScale, scaling, scaling, scaling);
			device->SetTransform(D3DTS_WORLD, &(matRotate * matScale));

			// select the vertex and index buffers to use
			device->SetStreamSource(0, objects[i].v_buffer, 0, sizeof(CUSTOMVERTEX));
			device->SetIndices(objects[i].i_buffer);

			device->SetMaterial(&objects[i].material);

			// draw the mesh
#if 0
			D3DVERTEXBUFFER_DESC pDesc_v;
			D3DINDEXBUFFER_DESC pDesc_i;
			v_buffer[i]->GetDesc(&pDesc_v);
			i_buffer[i]->GetDesc(&pDesc_i);
#endif
			//std::cout << "v " << pDesc_v.Size / sizeof(struct CUSTOMVERTEX) << " i " << pDesc_i.Size / sizeof(short) << std::endl;
			device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, objects[i].numvert, 0, objects[i].numprim);
		}


		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}
};

Demo *factory_lightbox_create()
{
	return new LightBox();
}

class NormalsGen : public Demo
{
	struct mesh_obj objects[1];
public:
	NormalsGen() : Demo() {};

	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		load_mesh("cube1.obj", "Object.1", device, objects[0]);

		D3DLIGHT9 light;    // create the light struct

		ZeroMemory(&light, sizeof(light));    // clear out the light struct for use
		light.Type = D3DLIGHT_DIRECTIONAL;    // make the light type 'directional light'
		light.Diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);    // set the light's color
		light.Direction = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
#if 0
		light.Type = D3DLIGHT_SPOT;    // make the light type 'directional light'
		light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);    // set the light's color
		light.Position = D3DXVECTOR3(-12.0f, 0.0f, 30.0f);
		light.Direction = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		light.Range = 100.0f;    // a range of 100
		light.Attenuation0 = 0.0f;    // no constant inverse attenuation
		light.Attenuation1 = 0.125f;    // only .125 inverse attenuation
		light.Attenuation2 = 0.0f;    // no square inverse attenuation
		light.Phi = D3DXToRadian(40.0f);    // set the outer cone to 30 degrees
		light.Theta = D3DXToRadian(20.0f);    // set the inner cone to 10 degrees
		light.Falloff = 1.0f;    // use the typical falloff
#endif

		device->SetLight(0, &light);    // send the light struct properties to light #0
		device->LightEnable(0, TRUE);    // turn on light #0

		//experiment with creating normals
		D3DXVECTOR3 v0(-3.f, -3.f, 3.f);
		D3DXVECTOR3 v1(3.f, -3.f, 3.f);
		D3DXVECTOR3 v2(-3.f, 3.f, 3.f);

		D3DXVECTOR3 s01 = v1 - v0;
		D3DXVECTOR3 s12 = v2 - v1;
		D3DXVECTOR3 n1;
		D3DXVec3Cross(&n1, &s01, &s12);
		D3DXVec3Normalize(&n1, &n1);

		D3DXVECTOR3 v4(-3.f, -3.f, -3.f);
		D3DXVECTOR3 v5(-3.f, 3.f, -3.f);
		D3DXVECTOR3 v6(3.f, -3.f, -3.f);

		D3DXVECTOR3 s45 = v5 - v4;
		D3DXVECTOR3 s56 = v6 - v5;
		D3DXVECTOR3 n5;
		D3DXVec3Cross(&n5, &s45, &s56);
		D3DXVec3Normalize(&n5, &n5);

		D3DXVECTOR3 s65 = v5 - v6;
		D3DXVECTOR3 s54 = v4 - v5;
		//D3DXVECTOR3 n5;
		D3DXVec3Cross(&n5, &s65, &s54);
		D3DXVec3Normalize(&n5, &n5);

		device->SetRenderState(D3DRS_LIGHTING, TRUE);    // turn on the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
		device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		//device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(70, 70, 70));    // ambient light

		init_done = 1;
		return 0;
	}

	virtual int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == 0)
			if (0 != do_init(d3d, device))
				return -1;

		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);
		device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		union inputs key = get_keypress();

		static int light_enabled = TRUE;
		if (key.t)
		{
			light_enabled = !light_enabled;
		}

		device->LightEnable(0, light_enabled);    // turn on light #0

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

		D3DXVec3TransformCoord(&camerapos, &D3DXVECTOR3(0.0f, 10.0f, -20.0f), &camRotate);
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


		static float index = 0.0f; index += 0.03f;

		for (int i = 0; i < ARRAYSIZE(objects); i++)
		{
			if (objects[i].v_buffer == 0) continue;
			// set the world transform
			D3DXMATRIX matRotate;
			D3DXMATRIX matScale;
			D3DXMATRIX matScratch;
			D3DXMatrixIdentity(&matRotate); D3DXMatrixIdentity(&matScale);
			//D3DXMatrixRotationX(&matRotate, D3DXToRadian(-90));
			//D3DXMatrixRotationY(&matScratch, rotate);
			//matRotate *= matScratch;
			D3DXMatrixScaling(&matScale, scaling, scaling, scaling);
			device->SetTransform(D3DTS_WORLD, &(matRotate * matScale));

			// select the vertex and index buffers to use
			device->SetStreamSource(0, objects[i].v_buffer, 0, sizeof(CUSTOMVERTEX));
			device->SetIndices(objects[i].i_buffer);

			device->SetMaterial(&objects[i].material);

			// draw the mesh
#if 0
			D3DVERTEXBUFFER_DESC pDesc_v;
			D3DINDEXBUFFER_DESC pDesc_i;
			v_buffer[i]->GetDesc(&pDesc_v);
			i_buffer[i]->GetDesc(&pDesc_i);
#endif
			//std::cout << "v " << pDesc_v.Size / sizeof(struct CUSTOMVERTEX) << " i " << pDesc_i.Size / sizeof(short) << std::endl;
			device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, objects[i].numvert, 0, objects[i].numprim);
		}


		device->EndScene();

		device->Present(NULL, NULL, NULL, NULL);

		return 0;
	}
};

Demo *factory_normalsgen_create()
{
	return new NormalsGen();
}