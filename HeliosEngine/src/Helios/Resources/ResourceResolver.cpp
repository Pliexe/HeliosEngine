#include "ResourceResolver.h"
#include "Helios/Resources/Texture.h"
#include "Helios/Utils/MeshBuilder.h"
#include "Helios/Utils/YamlExtensions.h"
#include "Helios/Core/Exceptions.h"

namespace Helios
{
    struct ObjMesh
	{
		MeshBuilder meshBuilder;
		std::string name;
	};
	
	static std::vector<ObjMesh> loadObjFile(const std::filesystem::path& filePath, bool load_as_one = false)
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

		bool hasUvs = false;
		bool hasNormals = false;

		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string keyword;
			bool jmp_back = false;

			if (iss >> keyword) {
				if (!load_as_one && keyword == "o") {
					iss >> currentName;

					vertices.clear();
					normals.clear();
					uvs.clear();
createObject:
					ObjMesh objMesh;
					objMesh.meshBuilder = MeshBuilder();
					objMesh.name = currentName;
					meshBuilders.push_back(objMesh);

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

					hasUvs = false;
					hasNormals = false;

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

								index++;
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
							index++;
						}
					}

					MeshVertex v0;
					v0.position = vertices[vindices[0]];
					v0.normal = normals.empty() ? Vector3() : normals[normalIndices[0]];
					v0.texCoord = uvs.empty() ? Vector2() : uvs[uvIndices[0]];

					MeshVertex v1;
					v1.position = vertices[vindices[1]];
					v1.normal = normals.empty() ? Vector3() : normals[normalIndices[1]];
					v1.texCoord = uvs.empty() ? Vector2() : uvs[uvIndices[1]];

					MeshVertex v2;
					v2.position = vertices[vindices[2]];
					v2.normal = normals.empty() ? Vector3() : normals[normalIndices[2]];
					v2.texCoord = uvs.empty() ? Vector2() : uvs[uvIndices[2]];

					if (faceVerts.size() == 4)
					{
						MeshVertex v3;
						v3.position = vertices[vindices[3]];
						v3.normal = normals.empty() ? Vector3() : normals[normalIndices[3]];
						v3.texCoord = uvs.empty() ? Vector2() : uvs[uvIndices[3]];

						meshBuilders.back().meshBuilder.CreateQuadFace(v0, v1, v2, v3);
					} else {
						meshBuilders.back().meshBuilder.CreateTriangleFace(v0, v1, v2);
					}
				}
			}
		}

		file.close();

		if (!hasUvs)
		{
			uint32_t index = 0;
			for (auto& vertex : meshBuilders[0].meshBuilder.GetVertices())
			{
				vertex = MeshBuilder::CalculateVertexNormal(meshBuilders[0].meshBuilder.GetVertices(), meshBuilders[0].meshBuilder.GetTriangles(), index);
				index++;
			}
		}

		return meshBuilders;
	}

	static bool IsTextureFile(const std::filesystem::path& path)
	{
		return path.extension() == ".png" || path.extension() == ".jpg";
	}

	static bool IsMeshFile(const std::filesystem::path& path)
	{
		return path.extension() == ".obj";
	}

	static bool IsCompatibleFile(const std::filesystem::path& path)
	{
		return IsMeshFile(path) || IsTextureFile(path);
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
		{
			ObjMesh meshobj = loadObjFile(it->second.path, true)[0];

			return Mesh::Create(meshobj.name, id, meshobj.meshBuilder);
		}
        case ResourceInfo::BUNDLED_FILE:
            break;
        case ResourceInfo::METADATA_FILE:
			ObjMesh meshobj = loadObjFile(it->second.path, true)[0];

			return Mesh::Create(meshobj.name, id, meshobj.meshBuilder);
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
			return Texture2D::Create(it->second.path);
        case ResourceInfo::BUNDLED_FILE:
            break;
        case ResourceInfo::METADATA_FILE:
            std::filesystem::path metaFile = std::filesystem::path(std::u8string(it->second.path) + u8".meta");
			auto tmp = metaFile.u8string();

            std::cout << "File: " << reinterpret_cast<const char*>(it->second.path) << std::endl;
            std::cout << "Metafile: " << std::string(tmp.begin(), tmp.end()) << std::endl;

			return Texture2D::Create(it->second.path);

            break;
        }

        HL_ASSERT(false, "Not implemented");
        return nullptr;
    }

	bool utf8_check_is_valid(const std::string& string)
	{
		int c, i, ix, n, j;
		for (i = 0, ix = string.length(); i < ix; i++)
		{
			c = (unsigned char)string[i];
			//if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
			if (0x00 <= c && c <= 0x7f) n = 0; // 0bbbbbbb
			else if ((c & 0xE0) == 0xC0) n = 1; // 110bbbbb
			else if (c == 0xed && i < (ix - 1) && ((unsigned char)string[i + 1] & 0xa0) == 0xa0) return false; //U+d800 to U+dfff
			else if ((c & 0xF0) == 0xE0) n = 2; // 1110bbbb
			else if ((c & 0xF8) == 0xF0) n = 3; // 11110bbb
			//else if (($c & 0xFC) == 0xF8) n=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
			//else if (($c & 0xFE) == 0xFC) n=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
			else return false;
			for (j = 0; j < n && i < ix; j++) { // n bytes matching 10bbbbbb follow ?
				if ((++i == ix) || (((unsigned char)string[i] & 0xC0) != 0x80))
					return false;
			}
		}
		return true;
	}

	const UUID ResourceResolver::RegisterResource(const std::filesystem::path& path, LoadOptions loadOptions)
	{

		if (!std::filesystem::exists(path))
		{
			throw std::runtime_error("File does not exist: " + path.string());
		}

		if (!IsCompatibleFile(path))
		{
			throw std::runtime_error("File is not compatible: " + path.string());
		}

		UUID uuid;

		if (loadOptions & LoadMetadataFile)
		{
			std::filesystem::path metaFile = std::filesystem::path(path.wstring() + L".meta");
			if (std::filesystem::exists(metaFile))
			{
				YAML::Node data;

				try
				{
					std::ifstream file(conversions::from_utf8_to_utf16(metaFile.u8string()));

					std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

					data = YAML::Load(fileContents);
				}
				catch (const YAML::BadFile& e)
				{
					//HL_EXCEPTION(true, std::string("Failed register resource! File: "));
					HL_EXCEPTION(true, "Failed register resource! File: " + conversions::from_u8string(metaFile.u8string()) + "\n" + e.what());
				}
				catch (const std::exception& e)
				{
					HL_EXCEPTION(true, "Failed register resource! File: " + conversions::from_u8string(metaFile.u8string()) + "\n" + e.what());
					//std::runtime_error();
				}

				uuid = data["uuid"].as<UUID>();
			}
			else {
				uuid = UUID();

				YAML::Emitter out;

				out << YAML::BeginMap;
				out << YAML::Key << "uuid" << YAML::Value << uuid;
				out << YAML::EndMap;

				std::ofstream fout(metaFile);
				fout << out.c_str();
			}
		}
		else uuid = UUID();

		RegisterResource(uuid, path, loadOptions);

		return uuid;
	}

	void ResourceResolver::RegisterResource(const UUID& uuid, const std::filesystem::path& path, LoadOptions loadOptions)
	{
		if (!std::filesystem::exists(path))
		{
			throw std::runtime_error("File does not exist: " + path.string());
		}

		if (!IsCompatibleFile(path))
		{
			throw std::runtime_error("File is not compatible: " + path.string());
		}

		if (loadOptions == 0) loadOptions = LoadFullResource;

		char8_t* _path = new char8_t[path.u8string().size() + 1];
		std::u8string str = path.u8string();
		std::copy(str.begin(), str.end(), _path);
		_path[str.size()] = '\0';

		if (loadOptions & LoadFullResource)
		{
			if (IsMeshFile(path))
				RegisterResource(s_Meshes, uuid, ResourceInfo{ loadOptions & LoadMetadataFile ? ResourceInfo::METADATA_FILE : ResourceInfo::FULL_RESOURCE, _path });
			else if (IsTextureFile(path))
				RegisterResource(s_Textures, uuid, ResourceInfo{ loadOptions & LoadMetadataFile ? ResourceInfo::METADATA_FILE : ResourceInfo::FULL_RESOURCE, _path });
		}
		else {
			if (IsMeshFile(path))
				RegisterResource(s_Meshes, uuid, ResourceInfo{ ResourceInfo::BUNDLED_FILE, _path });
			else if (IsTextureFile(path))
				RegisterResource(s_Textures, uuid, ResourceInfo{ ResourceInfo::BUNDLED_FILE, _path });
		}
	}

	const UUID& ResourceResolver::GetUUID(const std::filesystem::path& path)
	{
		std::filesystem::path metaFile = std::filesystem::path(path.string() + ".meta");

		if (std::filesystem::exists(metaFile)) {
			YAML::Node data;

			try
			{
				data = YAML::LoadFile(metaFile.string());
			}
			catch (const std::exception& e)
			{
				std::runtime_error("Failed to load scene file: " + metaFile.string());
			}

			if (!data["uuid"]) HL_ASSERT(false, "Metafile not found (UUID Missing)");

			return data["uuid"].as<UUID>();
		}
		else HL_ASSERT(false, "Metafile not found");
	}
}