
#include "helper.h"
#include "extern\objloader\OBJ_Loader.h"

#include <map>

#define RSRCDIR ".\\data\\"
#define RSRC(A) RSRCDIR##A

struct CUSTOMVERTEX { FLOAT X, Y, Z; D3DVECTOR NORMAL; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL)

static std::map<std::string, objl::Loader *> loaderCache;

int load_mesh(const char *file, const char *name, LPDIRECT3DDEVICE9 device, struct mesh_obj &out)
{
	objl::Loader *loader;

	LARGE_INTEGER start, end;
	QueryPerformanceFrequency(&start);
	double freq = (double)start.QuadPart / 1000;

	const auto search = loaderCache.find(file);
	if (search != loaderCache.end())
	{
		loader = loaderCache[file];
	}
	else
	{
		QueryPerformanceCounter(&start);

		loader = new objl::Loader();
		std::string fn(RSRCDIR);
		fn.append(file);
		if (loader->LoadFile(fn))
		{
			loaderCache[file] = loader;

			QueryPerformanceCounter(&end);
			std::cout << "Loaded " << file << " in: " << (double)(end.QuadPart - start.QuadPart) / freq << std::endl;
		}
		else return -1;
	}


	for (int i = 0; i < loader->LoadedMeshes.size(); i++)
	{
		if (loader->LoadedMeshes[i].MeshName.compare(name) == 0)
		{
			QueryPerformanceCounter(&start);

			objl::Mesh &mesh = loader->LoadedMeshes[i];
			device->CreateVertexBuffer(sizeof(struct CUSTOMVERTEX) * mesh.Vertices.size(),
				0,
				CUSTOMFVF,
				D3DPOOL_MANAGED,
				&out.v_buffer,
				NULL);

			VOID* pVoid;

			out.v_buffer->Lock(0, 0, (void**)&pVoid, 0);
			struct CUSTOMVERTEX *elm = (struct CUSTOMVERTEX *)pVoid;
			for (auto it = mesh.Vertices.begin(); it != mesh.Vertices.end(); it++, elm++)
			{
				//the object file has z-up coordinates so we swap z and y to obtain a y-up system
				//then, the faces are enumerated counter-clockwise and we need them clock-wise,
				//  therefore we negate the normal vector
				elm->X = it->Position.X;
				elm->Y = it->Position.Z;
				elm->Z = it->Position.Y;
				elm->NORMAL.x = -it->Normal.X;
				elm->NORMAL.y = -it->Normal.Z;
				elm->NORMAL.z = -it->Normal.Y;
			}
			out.v_buffer->Unlock();
			out.numvert = mesh.Vertices.size();

			//std::cout << "vsz " << sizeof(struct CUSTOMVERTEX) * mesh.Vertices.size() << " wr " << (char*)elm - (char*)pVoid << std::endl;

#if 1
			typedef short index_t;
			const D3DFORMAT index_fmt = D3DFMT_INDEX16;
#else
			typedef int32_t index_t;
			const D3DFORMAT index_fmt = D3DFMT_INDEX32;
#endif

			device->CreateIndexBuffer(mesh.Indices.size() * sizeof(index_t),
				0,
				index_fmt,
				D3DPOOL_MANAGED,
				&out.i_buffer,
				NULL);

			out.i_buffer->Lock(0, 0, (void**)&pVoid, 0);
			index_t *idx = (index_t *)pVoid;
			for (auto it = mesh.Indices.begin(); it != mesh.Indices.end(); it+=3, idx+=3)
			{
				//since the faces are enumerated counter-clockwise, reverse the indexes to get them clock-wise
				idx[2] = *it;
				idx[1] = *(it+1);
				idx[0] = *(it+2);
			}
			out.i_buffer->Unlock();
			out.numprim = mesh.Indices.size() / 3;

			//std::cout << "isz " << mesh.Indices.size() * sizeof(short) << " wr " << (char*)idx - (char*)pVoid << std::endl;

			ZeroMemory(&out.material, sizeof(D3DMATERIAL9));
			//defaults are black materials (0 0 0), and in such a case we prefer white materials
			const objl::Vector3 blackmat(0.f, 0.f, 0.f);
			objl::Vector3 &t = mesh.MeshMaterial.Ka;
			if(t == blackmat)
				out.material.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			else
				out.material.Ambient = D3DXCOLOR(t.X, t.Y, t.Z, 1.f);

			t = mesh.MeshMaterial.Kd;
			if(t == blackmat)
				out.material.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			else
				out.material.Diffuse = D3DXCOLOR(t.X, t.Y, t.Z, 1.f);


			QueryPerformanceCounter(&end);
			std::cout << "Loaded " << name << " in: " << (end.QuadPart - start.QuadPart) / freq << std::endl;

			break;
		}
	}

	return 0;
}


int load_mesh_ex( const char* file, const char* name, void (*vertex)(float, float, float, void *), void(*normal)(float, float, float, void *), void(*indices)(UINT,UINT,UINT,void *), struct mesh_obj &out, void *custom )
{
	objl::Loader *loader;

	LARGE_INTEGER start, end;
	QueryPerformanceFrequency(&start);
	double freq = (double)start.QuadPart / 1000;

	const auto search = loaderCache.find(file);
	if (search != loaderCache.end())
	{
		loader = loaderCache[file];
	}
	else
	{
		QueryPerformanceCounter(&start);

		loader = new objl::Loader();
		std::string fn(RSRCDIR);
		fn.append(file);
		if (loader->LoadFile(fn))
		{
			loaderCache[file] = loader;

			QueryPerformanceCounter(&end);
			std::cout << "Loaded " << file << " in: " << (double)(end.QuadPart - start.QuadPart) / freq << std::endl;
		}
		else return -1;
	}


	for (int i = 0; i < loader->LoadedMeshes.size(); i++)
	{
		if (loader->LoadedMeshes[i].MeshName.compare(name) == 0)
		{
			QueryPerformanceCounter(&start);

			objl::Mesh &mesh = loader->LoadedMeshes[i];
			out.numvert = mesh.Vertices.size();
			out.numprim = mesh.Indices.size() / 3;

			ZeroMemory(&out.material, sizeof(D3DMATERIAL9));
			//defaults are black materials (0 0 0), and in such a case we prefer white materials
			const objl::Vector3 blackmat(0.f, 0.f, 0.f);
			objl::Vector3 &t = mesh.MeshMaterial.Ka;
			if(t == blackmat)
				out.material.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			else
				out.material.Ambient = D3DXCOLOR(t.X, t.Y, t.Z, 1.f);

			t = mesh.MeshMaterial.Kd;
			if(t == blackmat)
				out.material.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			else
				out.material.Diffuse = D3DXCOLOR(t.X, t.Y, t.Z, 1.f);

			for (auto it = mesh.Vertices.begin(); it != mesh.Vertices.end(); it++)
			{
				//the object file has z-up coordinates so we swap z and y to obtain a y-up system
				//then, the faces are enumerated counter-clockwise and we need them clock-wise,
				//  therefore we negate the normal vector
				vertex( it->Position.X, it->Position.Z, it->Position.Y, custom );
				normal( -it->Normal.X, -it->Normal.Z, -it->Normal.Y, custom );
			}

			//std::cout << "vsz " << sizeof(struct CUSTOMVERTEX) * mesh.Vertices.size() << " wr " << (char*)elm - (char*)pVoid << std::endl;

			for (auto it = mesh.Indices.begin(); it != mesh.Indices.end(); it+=3)
			{
				//since the faces are enumerated counter-clockwise, reverse the indexes to get them clock-wise
				indices( *(it+2), *(it+1), *it, custom );
			}

			//std::cout << "isz " << mesh.Indices.size() * sizeof(short) << " wr " << (char*)idx - (char*)pVoid << std::endl;

			QueryPerformanceCounter(&end);
			std::cout << "Loaded " << name << " in: " << (end.QuadPart - start.QuadPart) / freq << std::endl;

			break;
		}
	}

	return 0;
}