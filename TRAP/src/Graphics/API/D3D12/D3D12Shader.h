#ifndef _TRAP_D3D12SHADER_H_
#define _TRAP_D3D12SHADER_H_

#ifdef TRAP_PLATFORM_WINDOWS

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderUniform.h"
#include "Graphics/Shaders/ShaderResource.h"
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

		void SetVSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetFSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetGSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetTCSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetTESUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetCSUniformBuffer(uint8_t* data, unsigned int size) override;

		const std::string& GetName() const override;
		
		const std::string& GetFilePath() const override;

		const std::string& GetVSSource() const override;
		const std::string& GetFSSource() const override;
		const std::string& GetGSSource() const override;
		const std::string& GetTCSSource() const override;
		const std::string& GetTESSource() const override;
		const std::string& GetCSSource() const override;

		const ShaderUniformBufferList& GetVSUniforms() const override;
		const ShaderUniformBufferList& GetFSUniforms() const override;
		const ShaderUniformBufferList& GetGSUniforms() const override;
		const ShaderUniformBufferList& GetTCSUniforms() const override;
		const ShaderUniformBufferList& GetTESUniforms() const override;
		const ShaderUniformBufferList& GetCSUniforms() const override;
		ShaderUniformBufferDeclaration* GetVSUniformBuffer() const override;
		ShaderUniformBufferDeclaration* GetFSUniformBuffer() const override;
		ShaderUniformBufferDeclaration* GetGSUniformBuffer() const override;
		ShaderUniformBufferDeclaration* GetTCSUniformBuffer() const override;
		ShaderUniformBufferDeclaration* GetTESUniformBuffer() const override;
		ShaderUniformBufferDeclaration* GetCSUniformBuffer() const override;
		const ShaderResourceList& GetResource() const override;

	private:
		friend class ::TRAP::Graphics::Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		//unsigned int m_handle;
		std::string m_name, m_filepath, m_source;
		std::string m_VSSource, m_FSSource, m_GSSource, m_TCSSource, m_TESSource, m_CSSource;

		ShaderUniformBufferList m_VSUniformBuffers;
		ShaderUniformBufferList m_FSUniformBuffers;
		ShaderUniformBufferList m_GSUniformBuffers;
		ShaderUniformBufferList m_TCSUniformBuffers;
		ShaderUniformBufferList m_TESUniformBuffers;
		ShaderUniformBufferList m_CSUniformBuffers;
		//std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_VSUserUniformBuffer;
		//std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_FSUserUniformBuffer;
		//std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_GSUserUniformBuffer;
		//std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_TCSUserUniformBuffer;
		//std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_TESUserUniformBuffer;
		//std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_CSUserUniformBuffer;
		ShaderResourceList m_resources;
		ShaderStructList m_structs;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetVSUniforms() const
{
	return m_VSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetFSUniforms() const
{
	return m_FSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetGSUniforms() const
{
	return m_GSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetTCSUniforms() const
{
	return m_TCSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetTESUniforms() const
{
	return m_TESUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetCSUniforms() const
{
	return m_CSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetVSUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetFSUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetGSUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetTCSUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetTESUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetCSUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderResourceList& TRAP::Graphics::API::D3D12Shader::GetResource() const
{
	return m_resources;
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