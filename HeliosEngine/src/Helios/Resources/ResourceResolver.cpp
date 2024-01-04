#include "ResourceResolver.h"
#include "Helios/Resources/Texture.h"
#include "Helios/Utils/MeshBuilder.h"
#include <filesystem>
#include <string>

namespace Helios
{
    struct ObjMesh
	{
		MeshBuilder meshBuilder;
		std::string name;
	};
	
	static std::vector<ObjMesh> loadObjFile(const std::filesystem::path& filePath)
	{
		if (!std::filesystem::exists(filePath))
		{
			throw std::runtime_error("File does not exist");
		}

		std::vector<ObjMesh> meshBuilders;

		std::ifstream file(filePath);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file");
		}

		std::vector<Vector3> vertices;
		std::vector<Vector3> normals;
		std::vector<Vector2> uvs;
		std::string currentName = filePath.stem().string();

		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string keyword;
			bool jmp_back = false;

			if (iss >> keyword) {
				if (keyword == "o") {
createObject:
					ObjMesh objMesh;
					objMesh.meshBuilder = MeshBuilder();
					iss >> objMesh.name;
					meshBuilders.push_back(objMesh);

					vertices.clear();
					normals.clear();
					uvs.clear();
					if (jmp_back) goto continueFace;
				} else if (keyword == "v") {
					Vector3 vertex;
					if (iss >> vertex.x >> vertex.y >> vertex.z) {
						vertices.push_back(vertex);
					}
				} else if (keyword == "vt") {
					Vector2 uv;
					if (iss >> uv.x >> uv.y) {
						uvs.push_back(uv);
					}
				} else if (keyword == "vn") {
					Vector3 normal;
					if (iss >> normal.x >> normal.y >> normal.z) {
						normals.push_back(normal);
					}
				} else if (keyword == "f")
				{
					if (meshBuilders.empty()) {
						jmp_back = true;
						goto createObject;
					}
continueFace:
					std::vector<std::string> faceVerts;
					std::string vertex;
					while (iss >> vertex) faceVerts.push_back(vertex);

					if (faceVerts.size() < 3 || faceVerts.size() > 4) {
						std::cout << "Invalid face: " << line << std::endl;
						continue;
					}

					bool hasUvs = false;
					bool hasNormals = false;

					std::array<uint32_t, 4> vindices;
					std::array<uint32_t, 4> uvIndices;
					std::array<uint32_t, 4> normalIndices;

					for (uint32_t i = 0; i < faceVerts.size(); i++)
					{
						std::string vertex = faceVerts[i];

						std::istringstream vertexStream(vertex);
						std::string vertexPart;
						uint32_t index = 0;
						while (std::getline(vertexStream, vertexPart, '/'))
						{
							if (vertexPart.empty()) {
								switch(index)
								{
									case 0:
										throw std::runtime_error("Invalid face (empty vertex): " + line);
										break;
									case 1:
										if (hasUvs) throw std::runtime_error("Invalid face (vertex missmatch, empty uv): " + line);
										break;
									case 2:
										if (hasNormals) throw std::runtime_error("Invalid face (vertex missmatch, empty normal): " + line);
										break;
								}

								continue;
							}
							
							int value = std::stoi(vertexPart);
							switch(index)
							{
								case 0:
									vindices[i] = value - 1;
									break;
								case 1:
									uvIndices[i] = value - 1;
									hasUvs = true;
									break;
								case 2:
									normalIndices[i] = value - 1;
									hasNormals = true;
									break;
							}
						}
					}

					MeshVertex v0;
					v0.position = vertices[vindices[0]];
					v0.normal = normals[normalIndices[0]];
					v0.texCoord = uvs[uvIndices[0]];

					MeshVertex v1;
					v1.position = vertices[vindices[1]];
					v1.normal = normals[normalIndices[1]];
					v1.texCoord = uvs[uvIndices[1]];

					MeshVertex v2;
					v2.position = vertices[vindices[2]];
					v2.normal = normals[normalIndices[2]];
					v2.texCoord = uvs[uvIndices[2]];

					if (faceVerts.size() == 4)
					{ 
						MeshVertex v3;
						v3.position = vertices[vindices[3]];
						v3.normal = normals[normalIndices[3]];
						v3.texCoord = uvs[uvIndices[3]];

						meshBuilders.back().meshBuilder.CreateQuadFace(v0, v1, v2, v3);
					} else {
						meshBuilders.back().meshBuilder.CreateTriangleFace(v0, v1, v2);
					}
				}
			}
		}

		file.close();

		return meshBuilders;
	}

    Ref<Mesh> ResourceResolver::ResolveMesh(const Helios::UUID& id)
    {
        auto it = s_Meshes.find(id);

        if (it == s_Meshes.end())
            return nullptr;

        switch (it->second.type)
        {
        case ResourceInfo::INBUILT:
            return Mesh::Create("Mesh_" + id.toString(), id, MeshGenerator::GenerateMesh(id));
        case ResourceInfo::FULL_RESOURCE:
            break;
        case ResourceInfo::BUNDLED_FILE:
            break;
        case ResourceInfo::METADATA_FILE:
            break;
        }

        HL_ASSERT(false, "Not implemented");

        return nullptr;
    }

    Ref<Texture2D> ResourceResolver::ResolveTexture2D(const Helios::UUID &id)
    {
        auto it = s_Textures.find(id);

        if (it == s_Textures.end())
            return nullptr;

        switch (it->second.type)
        {
        case ResourceInfo::INBUILT:
            break;
        case ResourceInfo::FULL_RESOURCE:
            break;
        case ResourceInfo::BUNDLED_FILE:
            break;
        case ResourceInfo::METADATA_FILE:
            std::filesystem::path metaFile = std::filesystem::path(std::string(it->second.path) + ".meta");

            std::cout << "File: " << metaFile.string() << std::endl;

            break;
        }

        HL_ASSERT(false, "Not implemented");
        return nullptr;
    }
}