#include "Mesh.hpp"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace glm;

namespace udit
{
	Mesh::Mesh() : vao_id(0), number_of_indices(0)
	{
		for (int i = 0; i < VBO_COUNT; i++)
		{
			vbo_ids[i] = 0;
		}
	}

	Mesh::~Mesh()
	{
		glDeleteVertexArrays(1, &vao_id);
		glDeleteBuffers(VBO_COUNT, vbo_ids);
	}

	bool Mesh::load_mesh(const std::string& file_path)
	{
		Assimp::Importer importer;
		auto scene = importer.ReadFile(
			file_path,
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_SortByPType | aiProcess_FlipUVs
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return false;
		}

		if (scene && scene->mNumMeshes > 0)
		{
			auto mesh = scene->mMeshes[0];
			number_of_indices = mesh->mNumFaces * 3;

			glGenBuffers(VBO_COUNT, vbo_ids);
			glGenVertexArrays(1, &vao_id);
			glBindVertexArray(vao_id);

			static_assert(sizeof(aiVector3D) == sizeof(fvec3), "aiVector3D mismatch");
			glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDINATES_VBO]);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), mesh->mVertices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			if (mesh->HasNormals())
			{
				glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[NORMALS_VBO]);
				glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), mesh->mNormals, GL_STATIC_DRAW);

				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}

			vector<float> uvs;
			uvs.reserve(mesh->mNumVertices * 2);

			for (unsigned i = 0; i < mesh->mNumVertices; i++)
			{
				if (mesh->HasTextureCoords(0))
				{
					uvs.push_back(mesh->mTextureCoords[0][i].x);
					uvs.push_back(mesh->mTextureCoords[0][i].y);
				}
				else
				{
					uvs.push_back(0.0f);
					uvs.push_back(0.0f);
				}
			}

			glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXTURES_VBO]);
			glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

			vector<GLuint> indices(number_of_indices);
			auto vertex_index = indices.begin();

			for (unsigned i = 0; i < mesh->mNumFaces; ++i)
			{
				auto& face = mesh->mFaces[i];
				assert(face.mNumIndices == 3);
				*vertex_index++ = face.mIndices[0];
				*vertex_index++ = face.mIndices[1];
				*vertex_index++ = face.mIndices[2];
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDICES_EBO]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), indices.data(), GL_STATIC_DRAW);

			glBindVertexArray(0);

			return true;
		}
		return false;
	}

	bool Mesh::create_mesh(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
	{
		number_of_indices = (GLsizei)indices.size();

		if (vao_id == 0) glGenVertexArrays(1, &vao_id);
		if (vbo_ids[0] == 0) glGenBuffers(VBO_COUNT, vbo_ids);

		glBindVertexArray(vao_id);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDINATES_VBO]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		if (!normals.empty())
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[NORMALS_VBO]);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if (!uvs.empty())
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXTURES_VBO]);
			glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDICES_EBO]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		return true;
	}

	void Mesh::draw_mesh() const
	{
		glBindVertexArray(vao_id);
		glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}



