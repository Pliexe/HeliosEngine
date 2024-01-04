#pragma once

#include "pch.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Math/Vector.h"
#include <stdexcept>

namespace Helios
{
	enum class ScriptAction
	{
		Jump,
		Stop
	};

	struct ScriptInfo
	{
		uint32_t version;
		uint32_t initial_allocations;
		uint32_t allocations;
		uint32_t actions;
		uint32_t functions;
	};

	struct ScriptAllocData
	{
		enum class Type
		{
			Float, Int32, UInt32, Double, String
		} type;
		union
		{
			float f;
			uint32_t ui32;
			int32_t i32;
			double d;
			char* str;
		};
	};

	struct ScriptData
	{
		ScriptAction action;
		
	};

	class NoFileException : public std::runtime_error { public: NoFileException() : std::runtime_error("NoFile") { } };
	class InvalidFileException : public std::runtime_error { public: InvalidFileException() : std::runtime_error("InvalidFile") { } };

	class VisualScript
	{
		ScriptInfo info;
		ScriptAction* actions;
		struct ScriptAllocations
		{
			ScriptAllocData::Type type;
			void* location;
		} *allocations;
		uint32_t* functions;

	public:
		VisualScript(std::string file) { VisualScript(file.c_str()); }
		VisualScript(const char* file)
		{
			std::ifstream input(file, std::ios::binary);
			
			FILE* fp;

			fopen_s(&fp, file, "rb");

			if (fp == nullptr) throw NoFileException();

			if (fread(&info, sizeof(ScriptInfo), 1, fp) != 1) { fclose(fp); throw InvalidFileException(); }
			if (info.functions <= 0) { fclose(fp); throw InvalidFileException(); }
			if (fread(&functions, sizeof(uint32_t), info.functions, fp) != info.functions) { fclose(fp); throw InvalidFileException(); }

			if(info.allocations > 0)
			{
				allocations = new ScriptAllocations[info.allocations];

				if(info.initial_allocations > 0)
				{
					if(info.initial_allocations > info.allocations) { fclose(fp); throw InvalidFileException(); }

					ScriptAllocData tmp;
					for (int i = 0, n = info.initial_allocations; i < n; i++)
					{
						if (fread(&tmp, sizeof(ScriptAllocData), 1, fp) != info.initial_allocations) { fclose(fp); throw InvalidFileException(); }
						allocations[i].type = tmp.type;
						switch (tmp.type)
						{
						case ScriptAllocData::Type::Float:
							allocations[i].location = new float(tmp.f);
							break;
						case ScriptAllocData::Type::Double:
							allocations[i].location = new double(tmp.d);
							break;
						case ScriptAllocData::Type::Int32:
							allocations[i].location = new int32_t(tmp.i32);
							break;
						case ScriptAllocData::Type::UInt32:
							allocations[i].location = new uint32_t(tmp.ui32);
							break;
						case ScriptAllocData::Type::String:
							allocations[i].location = new char[strlen(tmp.str)];
							strcpy_s((char*)allocations[i].location, strlen(tmp.str), tmp.str);
							break;
						default:
							{ fclose(fp); throw InvalidFileException(); }
						}
					}
				}

				//if (fread(&allocations, sizeof(ScriptAllocData), info.initial_allocations, fp) != info.initial_allocations) { fclose(fp); throw InvalidFileException(); }
			}

			if (info.actions)
			{
				actions = new ScriptAction[info.actions];
				if (fread(actions, sizeof(ScriptAction), info.actions, fp) != info.actions) { fclose(fp); throw InvalidFileException(); }
			}

			fclose(fp);
		}

		~VisualScript()
		{
			delete actions;
			delete allocations;
		}
	};

	struct NodeStoredData
	{
		enum class Type
		{
			Float, Int32, UInt32, Int64, UInt64, Double, String
		} type;
		union
		{
			float f;
			uint32_t ui32;
			int32_t i32;
			int64_t i64;
			uint64_t ui64;
			double d;
			std::string str;
		};

		json Serialize()
		{
			json j;
			j["type"] = type;

			switch (type)
			{
			case Type::String:
				j["data"] = str;
				break;
			case Type::Double:
				j["data"] = d;
				break;
			case Type::Float:
				j["data"] = f;
				break;
			case Type::Int32:
					j["data"] = i32;
					break;
			case Type::Int64:
				j["data"] = i64;
				break;
			case Type::UInt32:
				j["data"] = ui32;
				break;
			case Type::UInt64:
				j["data"] = ui64;
				break;
			}
		}

		void Deserialize(json j)
		{
			type = j["type"];

			switch (type)
			{
			case Type::String:
				str = j["data"].get<std::string>();
				break;
			case Type::Double:
				d = j["data"].get<double>();
				break;
			case Type::Float:
				f = j["data"].get<float>();
				break;
			case Type::Int32:
				i32 = j["data"].get<int32_t>();
				break;
			case Type::Int64:
				i64 = j["data"].get<int64_t>();
				break;
			case Type::UInt32:
				ui32 = j["data"].get<uint32_t>();
				break;
			case Type::UInt64:
				ui64 = j["data"].get<uint64_t>();
				break;
			}
		}
	};

	struct Node
	{
		virtual ~Node() = default;

		enum class Type
		{
			Event,
			If,
			Add
		} type;

		Vector2 position;

		std::vector<uint32_t> flow_out;

		std::vector<uint32_t> data_out;
		std::map<uint32_t, int64_t> data_in;

		Node() { }

		json Serialize()
		{
			json tmp;
			tmp["type"] = type;
			tmp["flow_out"] = flow_out;
			tmp["data_out"] = data_out;
			tmp["data_in"]  = data_in;
			json j;
			j["x"] = position.x;
			j["y"] = position.y;
			tmp["position"] = j;
		}
		void Deserialize(json tmp)
		{
			type = tmp["type"];
			flow_out = tmp["flow_out"].get<std::vector<uint32_t>>();
			data_out = tmp["data_out"].get<std::vector<uint32_t>>();
			data_in = tmp["data_in"].get<std::map<uint32_t, int64_t>>();
			json j = tmp["position"];
			position = { j["x"].get<float>(), j["y"].get<float>() };
		}

		Node(Type type) : type(type) { }
	};

	struct Event
	{
		std::string name;
		Vector2 position;
		uint32_t flow_out;

		Event(std::string name, Vector2 position) : name(name), position(position) { }
		Event(std::string name) : name(name) { }
		Event() { }

		json Serialize()
		{
			json tmp;

			tmp["type"] = Node::Type::Event;
			tmp["name"] = name;
			tmp["flow_out"] = flow_out;

			json j;
			j["x"] = position.x;
			j["y"] = position.y;
			tmp["position"] = j;
			
			return tmp;
		}
		
		void Deserialize(json tmp)
		{
			name = tmp["name"];
			flow_out = tmp["flow_out"].get<uint32_t>();
			json j = tmp["position"];
			position = { j["x"].get<float>(), j["y"].get<float>() };
		}
	};

	class VisualGraph
	{
		std::vector<Node> nodes;
		std::vector<Event> events { Event("Start"), { "Update" } };
		//std::vector<NodeStoredData> data;

		void Load()
		{
			
		}
	public:

		void Serialize(std::string file)
		{
			YAML::Emitter out;

			out << YAML::BeginMap;

			json n_j;
			for (auto& node : nodes)
				n_j.emplace_back(node.Serialize());

			out << YAML::Key << "Nodes" << YAML::Value << n_j.dump();

			json e_j;
			for (auto& node : events)
				e_j.emplace_back(node.Serialize());

			out << YAML::Key << "Events" << YAML::Value << e_j.dump();

			out << YAML::EndMap;
		}

		void Deserialize(std::string file)
		{
			auto in = YAML::LoadFile(file);

			if(!in.IsNull()) 
			{
				if(in["nodes"].IsNull())
				{					
					nodes.clear();
					for (auto& node : json::parse(in["nodes"].as<std::string>()).array())
					{
						Node nd;
						nd.Deserialize(node);
						nodes.emplace_back(nd);
						break;
					}
				}

				if (in["events"].IsNull())
				{
					events.clear();
					for (auto& node : json::parse(in["events"].as<std::string>()).array())
					{
						Event nd;
						nd.Deserialize(node);
						events.emplace_back(nd);
						break;
					}
				}
			}
		}
	};

	class VisualScriptFramework
	{
		inline static std::unordered_map<std::string, VisualGraph> graphs;

	public:

		static VisualGraph Create(std::string file)
		{
			VisualGraph tmp;
			tmp.Serialize(file);
			graphs.emplace(file, tmp);
			return tmp;
		}

		static VisualGraph Load(std::string file)
		{
			VisualGraph tmp;
			tmp.Deserialize(file);
			graphs.emplace(file, tmp);
			return tmp;
		}

		static void Run()
		{
		}
	};
}

