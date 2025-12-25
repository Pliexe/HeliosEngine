#include "ResourceResolver.h"
#include "Helios/Resources/Texture.h"
#include "Helios/Utils/MeshBuilder.h"
#include "Helios/Utils/YamlExtensions.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Resources/Bundler/Bundle.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Helios
{
    std::unordered_map<UUID, ResourceInfo> ResourceResolver::s_Meshes;
    std::unordered_map<UUID, ResourceInfo> ResourceResolver::s_Textures;

    std::unordered_set<std::filesystem::path> ResourceResolver::s_RegisteredPaths;

    struct ObjMesh
	{
		MeshBuilder meshBuilder;
		std::string name;
	};

    static std::vector<ObjMesh> loadObjFile(const std::filesystem::path& filePath, bool load_as_one = false)
    {
        if (!std::filesystem::exists(filePath))
            throw std::runtime_error("File does not exist");

        // Pre-allocate based on file size
        const size_t fileSize = std::filesystem::file_size(filePath);
        const size_t estimatedVertices = fileSize / 50; // Rough estimation

        std::vector<ObjMesh> meshBuilders;
        meshBuilders.reserve(10); // Typical obj files have few objects

        std::vector<Vector3> vertices;
        std::vector<Vector3> normals;
        std::vector<Vector2> uvs;
        vertices.reserve(estimatedVertices);
        normals.reserve(estimatedVertices);
        uvs.reserve(estimatedVertices);

        // Memory map large files
        std::ifstream file(filePath, std::ios::binary);
        if (!file)
            throw std::runtime_error("Failed to open file");

        auto start = std::chrono::high_resolution_clock::now();

        std::string currentName = filePath.stem().string();
        bool hasUvs = false, hasNormals = false;
        std::vector<std::string_view> tokens;
        tokens.reserve(5);

        std::string line;
        line.reserve(256);

        auto parseFaceVertex = [](std::string_view vertex, uint32_t& v, uint32_t& vt, uint32_t& vn) {
            size_t start = 0, end = 0;
            end = vertex.find('/', start);
            v = std::atoi(vertex.substr(start).data()) - 1;

            if (end == std::string_view::npos) return;
            start = end + 1;
            end = vertex.find('/', start);
            if (start != end)
                vt = std::atoi(vertex.substr(start, end - start).data()) - 1;

            if (end == std::string_view::npos) return;
            start = end + 1;
            if (start < vertex.length())
                vn = std::atoi(vertex.substr(start).data()) - 1;
            };

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            char type = line[0];
            const char* ptr = line.c_str() + 2;

            switch (type) {
            case 'v': {
                if (line[1] == ' ') {
                    Vector3 v;
                    if (sscanf(ptr, "%f %f %f", &v.x, &v.y, &v.z) == 3)
                        vertices.push_back(v);
                }
                else if (line[1] == 't') {
                    Vector2 vt;
                    if (sscanf(ptr, "%f %f", &vt.x, &vt.y) == 2)
                        uvs.push_back(vt);
                }
                else if (line[1] == 'n') {
                    Vector3 vn;
                    if (sscanf(ptr, "%f %f %f", &vn.x, &vn.y, &vn.z) == 3)
                        normals.push_back(vn);
                }
                break;
            }
            case 'f': {
                if (meshBuilders.empty()) {
                    meshBuilders.emplace_back(ObjMesh{ MeshBuilder(), currentName });
                }

                std::array<uint32_t, 4> vindices = {}, uvIndices = {}, normalIndices = {};
                int vertCount = 0;
                char* context = nullptr;
#ifdef HELIOS_PLATFORM_WINDOWS
                char* token = strtok_s((char*)ptr, " ", &context);

                while (token && vertCount < 4) {
                    parseFaceVertex(token, vindices[vertCount], uvIndices[vertCount], normalIndices[vertCount]);
                    vertCount++;
                    token = strtok_s(nullptr, " ", &context);
                }
#else
                char* token = strtok_r((char*)ptr, " ", &context);

                while (token && vertCount < 4) {
                    parseFaceVertex(token, vindices[vertCount], uvIndices[vertCount], normalIndices[vertCount]);
                    vertCount++;
                    token = strtok_r(nullptr, " ", &context);
                }
#endif

                if (vertCount >= 3) {
                    auto& builder = meshBuilders.back().meshBuilder;
                    std::array<MeshVertex, 4> verts;

                    for (int i = 0; i < vertCount; i++) {
                        verts[i].position = vertices[vindices[i]];
                        verts[i].normal = !normals.empty() ? normals[normalIndices[i]] : Vector3();
                        verts[i].texCoord = !uvs.empty() ? uvs[uvIndices[i]] : Vector2();
                    }

                    if (vertCount == 4)
                        builder.CreateQuadFace(verts[0], verts[1], verts[2], verts[3]);
                    else
                        builder.CreateTriangleFace(verts[0], verts[1], verts[2]);
                }
                break;
            }
            case 'o':
                if (!load_as_one) {
                    currentName = line.substr(2);
                    meshBuilders.emplace_back(ObjMesh{ MeshBuilder(), currentName });
                }
                break;
            }
        }

        auto stop = std::chrono::high_resolution_clock::now();
        std::wcout << "Mesh loading took: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
            << "ms, " << meshBuilders.size() << " objects" << std::endl;

        if (!hasNormals) {
            start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
            for (auto& mesh : meshBuilders) {
                MeshBuilder::CalculateVertexNormals(mesh.meshBuilder);
            }
            stop = std::chrono::high_resolution_clock::now();
            std::wcout << "Normal calculation took: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms" << std::endl;
        }

        return meshBuilders;
    }

	static bool IsTextureFile(const std::filesystem::path& path)
	{
		return path.extension() == ".png" || path.extension() == ".jpg" || path.extension() == ".webp";
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
        case Helios::ResourceInfo::INBUILT:
            return Mesh::Create("Mesh_" + id.toString(), id, MeshGenerator::GenerateMesh(id));
        case Helios::ResourceInfo::FULL_RESOURCE:
		{
			ObjMesh meshobj = loadObjFile(it->second.path, true)[0];

			return Mesh::Create(meshobj.name, id, meshobj.meshBuilder);
		}
        case Helios::ResourceInfo::BUNDLED_FILE:
            break;
        case Helios::ResourceInfo::FULL_RESOURCE_AND_METADATA_FILE:
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
        case Helios::ResourceInfo::INBUILT:
            break;
        case Helios::ResourceInfo::FULL_RESOURCE:
			return Texture2D::Create(it->second.path);
        case Helios::ResourceInfo::BUNDLED_FILE:
            {
                auto& raw_data = GetBundleData(it->second.path, id);
                int width, height;
                stbi_uc* data = nullptr;
                {
                    data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(raw_data.data()), raw_data.size(), &width, &height, nullptr, 4);
                }
                HL_EXCEPTION(
                    !data,
                    "Failed to load image at path: " + conversions::from_u8string(it->second.path) + "\n" + stbi_failure_reason()
                );

                auto texture = Texture2D::Create(width, height);
                texture->SetData(data, width * height * 4);
                stbi_image_free(data);

                return texture;
            }
        case Helios::ResourceInfo::FULL_RESOURCE_AND_METADATA_FILE:
            std::filesystem::path metaFile = std::filesystem::path(std::u8string(it->second.path) + u8".meta");
			auto tmp = metaFile.u8string();

            // std::cout << "File: " << reinterpret_cast<const char*>(it->second.path) << std::endl;
            // std::cout << "Metafile: " << std::string(tmp.begin(), tmp.end()) << std::endl;

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
                    #ifdef HELIOS_PLATFORM_WINDOWS
					std::ifstream file(conversions::from_utf8_to_utf16(metaFile.u8string()));
                    #else
                    std::ifstream file(metaFile);
                    #endif

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
				RegisterResource(s_Meshes, uuid, Helios::ResourceInfo{ loadOptions & LoadMetadataFile ? Helios::ResourceInfo::FULL_RESOURCE_AND_METADATA_FILE : Helios::ResourceInfo::FULL_RESOURCE, _path });
			else if (IsTextureFile(path))
				RegisterResource(s_Textures, uuid, Helios::ResourceInfo{ loadOptions & LoadMetadataFile ? Helios::ResourceInfo::FULL_RESOURCE_AND_METADATA_FILE : Helios::ResourceInfo::FULL_RESOURCE, _path });
		}
		else {
			if (IsMeshFile(path))
				RegisterResource(s_Meshes, uuid, Helios::ResourceInfo{ Helios::ResourceInfo::BUNDLED_FILE, _path });
			else if (IsTextureFile(path))
				RegisterResource(s_Textures, uuid, Helios::ResourceInfo{ Helios::ResourceInfo::BUNDLED_FILE, _path });
		}
	}

	const UUID ResourceResolver::GetUUID(const std::filesystem::path& path)
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

			HL_EXCEPTION(!data["uuid"], "Metafile not found (UUID Missing)");

			return data["uuid"].as<UUID>();
		}
		else HL_EXCEPTION(true, "Metafile not found");
	}
}
