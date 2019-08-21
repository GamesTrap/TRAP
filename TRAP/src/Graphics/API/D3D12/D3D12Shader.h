#ifndef _TRAP_D3D12SHADER_H_
#define _TRAP_D3D12SHADER_H_

#ifdef TRAP_PLATFORM_WINDOWS

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderManager.h"

namespace TRAP::Graphics::API
{
	class D3D12Shader final : public Graphics::Shader
	{
	public:
		D3D12Shader(std::string name, std::string source);
		D3D12Shader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource);
		D3D12Shader(const D3D12Shader&) = default;
		D3D12Shader& operator=(const D3D12Shader&) = default;
		D3D12Shader(D3D12Shader&&) = default;
		D3D12Shader& operator=(D3D12Shader&&) = default;
		~D3D12Shader();

		void Bind() const override;
		void Unbind() const override;

		const std::string& GetName() const override;
		
		const std::string& GetFilePath() const override;

		const std::string& GetVSSource() const override;
		const std::string& GetFSSource() const override;
		const std::string& GetGSSource() const override;
		const std::string& GetTCSSource() const override;
		const std::string& GetTESSource() const override;
		const std::string& GetCSSource() const override;

	private:
		friend class ::TRAP::Graphics::Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		std::string m_name, m_filepath, m_source;
		std::string m_VSSource, m_FSSource, m_GSSource, m_TCSSource, m_TESSource, m_CSSource;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetVSSource() const
{
	return m_VSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetFSSource() const
{
	return m_FSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetGSSource() const
{
	return m_GSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetTCSSource() const
{
	return m_TCSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetTESSource() const
{
	return m_TESSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetCSSource() const
{
	return m_CSSource;
}

#endif

#endif /*_TRAP_D3D12SHADER_H_*/