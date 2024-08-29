#pragma once

#include "pch.h"
#include "Helios/Resources/ShaderBuilder.h"

#ifdef HELIOS_PLATFORM_WINDOWS
namespace Helios
{
	class D3D11Shader : public Shader
	{
	public:
		D3D11Shader(const std::string& name, const std::string& vertex_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology);
		D3D11Shader(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology);
		D3D11Shader(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, const std::string& geometry_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology);

		const std::string& GetName() const override;
		const DepthFunc GetDepthFunc() const override;
		const Topology GetTopology() const override;

		void SetDepthFunc(DepthFunc depthFunc) override;
		void SetTopology(Topology topology) override;

		void Bind() const override;
		void Unbind() const override;

		const Helios::InputLayouts& GetInputLayouts() const override;

	private:
		std::string m_Name;

		InputLayouts m_InputLayouts;
		DepthFunc m_DepthFunc = DepthFunc::Less;
		Topology m_Topology = Topology::TriangleList;


		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader = nullptr;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader = nullptr;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_GeometryShader = nullptr;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	};
}
#endif