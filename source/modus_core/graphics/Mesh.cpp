#include <modus_core/graphics/Mesh.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/scene.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ds::list<vertex> mesh::load_from_file(fs::path const & path)
	{
		return load_from_file(path,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ds::list<vertex> mesh::load_from_file(fs::path const & path, int32_t flags)
	{
		ds::list<vertex> verts{};

		// open scene
		Assimp::Importer _ai;
		aiScene const * s{ _ai.ReadFile(path.string().c_str(), flags) };
		ML_defer(&){ _ai.FreeScene(); };
		if (!s) { return verts; }

		// for each mesh
		std::for_each(&s->mMeshes[0], &s->mMeshes[s->mNumMeshes], [&](aiMesh * const m)
		{
			// for each face
			std::for_each(&m->mFaces[0], &m->mFaces[m->mNumFaces], [&](aiFace const & f)
			{
				// reserve space
				verts.reserve(
					verts.capacity() + std::distance(&f.mIndices[0], &f.mIndices[f.mNumIndices])
				);

				// for each index
				std::for_each(&f.mIndices[0], &f.mIndices[f.mNumIndices], [&](uint32_t i)
				{
					auto const vp{ // position
						m->mVertices ? &m->mVertices[i] : nullptr
					};
					auto const vn{ // normal
						m->mNormals ? &m->mNormals[i] : nullptr
					};
					auto const uv{ // texcoord
						m->HasTextureCoords(0) ? &m->mTextureCoords[0][i] : nullptr
					};

					// make vertex
					verts.push_back(vertex{
						vp ? vec3{ vp->x, vp->y, vp->z } : vec3::zero(),
						vn ? vec3{ vn->x, vn->y, vn->z } : vec3::one(),
						uv ? vec2{ uv->x, uv->y } : vec2::one()
					});
				});
			});
		});

		return verts;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}