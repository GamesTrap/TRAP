#ifndef _TRAP_D3D12SHADER_H_
#define _TRAP_D3D12SHADER_H_

#ifdef TRAP_PLATFORM_WINDOWS

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderManager.h"

namespace TRAP::Graphics::API
{
	class D3D12Shader final : public Shader
	{
	public:
		D3D12Shader(std::string name, std::string source);
		D3D12Shader(std::string name, std::vector<uint32_t>& source);
		D3D12Shader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource);
		D3D12Shader(const D3D12Shader&) = default;
		D3D12Shader& operator=(const D3D12Shader&) = default;
		D3D12Shader(D3D12Shader&&) = default;
		D3D12Shader& operator=(D3D12Shader&&) = default;
		~D3D12Shader();

		void Bind() const override;
		void Unbind() const override;

		std::string_view GetName() const override;
		
		std::string_view GetFilePath() const override;

	private:
		friend class ::TRAP::Graphics::Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		std::string m_name, m_filepath;
		std::string vsSource;
		std::string fsSource;
		std::string gsSource;
		std::string tcsSource;
		std::string tesSource;
		std::string csSource;
	};
}

#endif

#endif /*_TRAP_D3D12SHADER_H_*/