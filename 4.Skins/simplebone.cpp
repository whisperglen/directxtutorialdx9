#include "skins.h"

struct CUSTOMVERTEX
{
	FLOAT X, Y, Z;
	//FLOAT weights[3];
	//DWORD matind;
	DWORD COLOR;
};
//#define CUSTOMFVF (D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_DIFFUSE)
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

#define STEP_VERTEX 1 << 0
#define STEP_NORMAL 1 << 1
#define STEP_ALL (STEP_VERTEX | STEP_NORMAL )

#define MAX_VERTEXES 250
#define MAX_INDEXES (6*MAX_VERTEXES)

class SimpleBone: public Demo
{
	LPDIRECT3DVERTEXBUFFER9 v_buffer;
	LPDIRECT3DINDEXBUFFER9 i_buffer;
	struct CUSTOMVERTEX *pVert;
	short *pInd;
	struct mesh_obj objects[6];
	int objnum;
	int numsteps;
	int step;
	int total_vertexes;
	int total_indexes;

	LPDIRECT3D9 _d3d;
	LPDIRECT3DDEVICE9 _device;

	friend void recv_vertex( float x, float y, float z, void *custom );
	friend void recv_normal( float x, float y, float z, void *custom );
	friend void recv_prim_indices( UINT i0, UINT i1, UINT i2, void *custom );

public:
	SimpleBone() : Demo(), pVert(0), pInd(0), objects(), objnum(0), numsteps(0), step(0), total_vertexes(0), total_indexes(0) {}

	virtual int do_init(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		device->CreateVertexBuffer( MAX_VERTEXES * sizeof(struct CUSTOMVERTEX),
			0,
			CUSTOMFVF,
			D3DPOOL_MANAGED,
			&v_buffer,
			NULL );

		v_buffer->Lock( 0, 0, (void**)&pVert, 0 );

		device->CreateIndexBuffer( MAX_INDEXES * sizeof( short ),
			0,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&i_buffer,
			NULL );

		i_buffer->Lock( 0, 0, (void**)&pInd, 0 );

		const char* objnames[] = {
			"Object.1",
			"Object.2",
			"Object.3",
			"Object.4",
			"Object.5",
			"Object.6",
		};

		for ( int i = 0; i < ARRAYSIZE( objects ); i++ )
		{
			objnum = i;
			numsteps = 0;
			load_mesh_ex( "simplebone.obj", objnames[i], recv_vertex, recv_normal, recv_prim_indices, objects[i], this );
		}

		v_buffer->Unlock();
		i_buffer->Unlock();

		device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
		device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

		_d3d = d3d;
		_device = device;

		init_done = true;

		return 0;
	}

	virtual void step_finished( UINT stepid )
	{
		step |= stepid;

		if ( step == STEP_ALL )
		{
			step = 0;

			if ( total_vertexes >= MAX_VERTEXES )
			{
				std::cout << "max indexes reached! objectnum: " << objnum << std::endl;
				system("pause");
				exit( -1 );
			}

			DWORD bonenum = 0;
			FLOAT boneweight = 0;

			switch ( objnum )
			{
			case 0:
				boneweight = 1.0;
				bonenum = 1;
				break;
			case 1:
				boneweight = 0.5;
				bonenum = 1;
				break;
			case 2:
				boneweight = 0.25;
				bonenum = 1;
				break;

			case 3:
				boneweight = 0.25;
				bonenum = 2;
				break;
			case 4:
				boneweight = 0.5;
				bonenum = 2;
				break;
			case 5:
				boneweight = 1.0;
				bonenum = 2;
				break;
			}

			D3DMATERIAL9& mat = objects[objnum].material;
			DWORD boneidxs = 0;
			/*for ( int i = 0; i < ARRAYSIZE( pVert->weights ); i++ )
			{
				pVert->weights[i] = boneweight;
				boneidxs |= bonenum << (8 * (3 - i));
			}
			pVert->matind = boneidxs;*/
			pVert->COLOR = D3DCOLOR_COLORVALUE( mat.Diffuse.r, mat.Diffuse.g, mat.Diffuse.b, mat.Diffuse.a );

			pVert++;
			total_vertexes++;
		}
	}

	virtual int loop(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 device)
	{
		if (init_done == false)
			if (0 != do_init(d3d, device))
				return -1;

		device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);
		device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		union inputs key = get_keypress();

		static float scaling = 0.1f;
		if (key.vert && key.ctrl)
		{
			scaling += 0.01f * key.vert;
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

		D3DXVec3TransformCoord(&camerapos, &D3DXVECTOR3(0.0f, 20.0f, -50.0f), &camRotate);
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
			10000.0f);    // the far view-plane
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
		device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		device->SetIndices( i_buffer );

		D3DXMATRIX matTranslate;
		D3DXMatrixTranslation(&matTranslate, 0.0f, 0.0f, 00.0f);
		device->SetTransform(D3DTS_WORLD, &(matTranslate * matScale));    // set the world transform

		int indexpos = 0;
		int vertexpos = 0;
		for ( int i = 0; i < ARRAYSIZE( objects ); i++ )
		{
			device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, vertexpos, 0, objects[i].numvert, indexpos, objects[i].numprim );
			vertexpos += objects[i].numvert;
			indexpos += 3 * objects[i].numprim;
		}

		device->EndScene();    // ends the 3D scene

		device->Present(NULL, NULL, NULL, NULL);    // displays the created frame

		return 0;
	}

	virtual ~SimpleBone()
	{
		if (init_done)
		{
		}
	}
};

void recv_vertex( float x, float y, float z, void* custom )
{
	SimpleBone* sb = (SimpleBone*)custom;

	sb->pVert->X = x;
	sb->pVert->Y = y;
	sb->pVert->Z = z;

	sb->step_finished( STEP_VERTEX );
}

void recv_normal( float x, float y, float z, void* custom )
{
	SimpleBone* sb = (SimpleBone*)custom;

	sb->step_finished( STEP_NORMAL );
}

void recv_prim_indices( UINT i0, UINT i1, UINT i2, void* custom )
{
	SimpleBone* sb = (SimpleBone*)custom;

	if ( sb->total_indexes >= MAX_INDEXES )
	{
		std::cout << "max indexes reached! objectnum: " << sb->objnum << std::endl;
		system("pause");
		exit( -1 );
	}
	else
	{
		sb->pInd[0] = i0;
		sb->pInd[1] = i1;
		sb->pInd[2] = i2;

		sb->pInd += 3;
		sb->total_indexes += 3;
	}
}

Demo *factory_simplebone_create()
{
	return new SimpleBone();
}