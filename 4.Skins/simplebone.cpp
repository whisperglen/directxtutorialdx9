#include "skins.h"

#define STEP_VERTEX 1 << 0
#define STEP_NORMAL 1 << 1
#define STEP_ALL (STEP_VERTEX | STEP_NORMAL )

class SimpleBone: public Demo
{

	struct CUSTOMVERTEX
	{
		FLOAT X, Y, Z;
		FLOAT weights[1];
		union {
			DWORD matind;
			BYTE matindb[4];
		};
		DWORD COLOR;
	};
	static const UINT CUSTOMFVF = ( D3DFVF_XYZB2 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_DIFFUSE );
	//static const UINT CUSTOMFVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE);

	static const UINT BONEOFFSET = 3;

	static const UINT NUM_OBJECTS = 5;
	static const UINT MAX_VERTEXES = 250;
	static const UINT MAX_INDEXES = (6*MAX_VERTEXES);

	LPDIRECT3DVERTEXBUFFER9 v_buffer;
	LPDIRECT3DINDEXBUFFER9 i_buffer;
	struct CUSTOMVERTEX *pVert;
	short *pInd;
	struct mesh_obj objects[NUM_OBJECTS];
	const float center[NUM_OBJECTS] = { 100, 204, 212, 220, 324};
	const float offset[NUM_OBJECTS] = { 0, 200, 208, 216, 224};
	D3DXMATRIX bones[2];
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
	SimpleBone() : Demo(), v_buffer(0), i_buffer(0), pVert(0), pInd(0),
		objects(), bones(), objnum(0), numsteps(0), step(0), total_vertexes(0), total_indexes(0),
		_d3d(0), _device(0) {}

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

		D3DXMatrixIdentity( &bones[0] );
		D3DXMatrixIdentity( &bones[1] );

		device->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
		device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		device->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_1WEIGHTS );
		device->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE );

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

			pVert->matind = 0;

			DWORD bonenum = 0;
			FLOAT boneweight = 0;
			DWORD boneidxs = 0;

			float centerval = center[objnum];

			//bonenum 0
			bonenum = 0;
			switch ( objnum )
			{
			case 0:
				boneweight = 1.0;
				break;
			case 1:
				if ( pVert->X < centerval )
					boneweight = 1.0;
				else
					boneweight = 0.66f;
				break;
			case 2:
				if ( pVert->X < centerval )
					boneweight = 0.66f;
				else
					boneweight = 0.33f;
				break;
			case 3:
				if ( pVert->X < centerval )
					boneweight = 0.33f;
				else
					boneweight = 0.0;
				break;
			case 4:
				boneweight = 0.0;
				break;
			}
			pVert->weights[bonenum] = boneweight;
			boneidxs |= bonenum << (8 * bonenum);
			pVert->matindb[bonenum] = bonenum + BONEOFFSET;

			//bonenum 1
			bonenum = 1;
			switch ( objnum )
			{
			case 0:
			case 1:
			case 2:
				break;
			}
			//pVert->weights[bonenum] = boneweight;
			boneidxs |= bonenum << (8 * bonenum);
			pVert->matindb[bonenum] = bonenum + BONEOFFSET;

			//pVert->matind = boneidxs;

			D3DMATERIAL9& mat = objects[objnum].material;
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

		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0x33, 0x4d, 0x4d), 1.0f, 0);

		union inputs key = get_keypress();

#define BONEMODS 3
		static int bonemod = 0;
		if ( key.b )
		{
			bonemod++;
			if ( bonemod >= BONEMODS )
			{
				bonemod = 0;
			}

			std::cout << "bonemod: " << bonemod << std::endl;
		}

		static float scaling = 0.1f;
		if (key.vert && key.ctrl)
		{
			scaling += 0.01f * key.vert;
		}


		static float rotx[BONEMODS] = { 0 }, roty[BONEMODS] = { 0 }, rotz[BONEMODS] = { 0 };
		if (key.horiz && !key.ctrl)
		{
			roty[bonemod] += 0.2f * -key.horiz;
		}
		if (key.horiz && key.ctrl)
		{
			rotz[bonemod] += 0.2f * key.horiz;
		}
		if (key.vert && !key.ctrl)
		{
			rotx[bonemod] += 0.2f * key.vert;
		}

		device->BeginScene();

		device->SetFVF(CUSTOMFVF);

		D3DXMATRIX rotate;
		D3DXMATRIX camRotate;
		D3DXMATRIX scratch;
		D3DXMatrixRotationX( &rotate, rotx[bonemod] );
		rotate *= *D3DXMatrixRotationY( &scratch, roty[bonemod] );
		rotate *= *D3DXMatrixRotationZ( &scratch, rotz[bonemod] );

		D3DXMatrixRotationX( &camRotate, rotx[0] );
		camRotate *= *D3DXMatrixRotationY( &scratch, roty[0] );
		camRotate *= *D3DXMatrixRotationZ( &scratch, rotz[0] );

		if ( bonemod == 1 )
		{
			bones[0] = rotate;
		}
		if ( bonemod == 2 )
		{
			bones[1] = rotate;
		}
		D3DXVECTOR3 camerapos, cameraup;
		D3DXVec3TransformCoord(&camerapos, &D3DXVECTOR3(20.0f, 20.0f, -50.0f), &camRotate);
		D3DXVec3TransformCoord(&cameraup, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &scratch);

		// set the view transform
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView,
			&camerapos,    // the camera position
			&D3DXVECTOR3(20.0f, 0.0f, 0.0f),      // the look-at position
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
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matRotate); D3DXMatrixIdentity(&matScale);
		//D3DXMatrixRotationX(&matRotate, D3DXToRadian(-90));
		//D3DXMatrixRotationY(&matScratch, rotate);
		//matRotate *= matScratch;
		D3DXMatrixScaling(&matScale, scaling, scaling, scaling);

		// set the stream source
		device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		device->SetIndices( i_buffer );

		D3DXMATRIX matTranslate;
		D3DXMatrixTranslation(&matTranslate, -50.0f, 0.0f, 00.0f);
		matWorld = matTranslate * matScale;
		//device->SetTransform(D3DTS_WORLD, &matWorld);    // set the world transform

		device->SetTransform( D3DTS_WORLDMATRIX(0 + BONEOFFSET), &(matWorld * bones[0]) );
		device->SetTransform( D3DTS_WORLDMATRIX(1 + BONEOFFSET), &(matWorld * bones[1]) );

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
			_device->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
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

	if ( sb->total_indexes >= SimpleBone::MAX_INDEXES )
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