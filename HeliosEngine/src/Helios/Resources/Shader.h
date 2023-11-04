#pragma once

#include "pch.h"
#include "Helios/Core/Asserts.h"
#include "Platform/Direct3D11/Direct3D11Context.h"
#include "Buffer.h"


namespace Helios {

	enum class DepthFunc
	{
		Always,
		Never,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual
	};

	enum class Topology
	{
		TriangleList,
		LineList,
		PointList,
	};

	class Shader
	{
	public:
		virtual const std::string& GetName() const = 0;

		virtual const InputLayouts& GetInputLayouts() const = 0;
		virtual const DepthFunc GetDepthFunc() const = 0;
		virtual const Topology GetTopology() const = 0;

		virtual void SetDepthFunc(DepthFunc depthFunc) = 0;
		virtual void SetTopology(Topology topology) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Ref<Shader> Create(const std::string& name, const std::string& vertex_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology);
		static Ref<Shader> Create(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology);
		static Ref<Shader> Create(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, const std::string& geometry_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology);
	};
}