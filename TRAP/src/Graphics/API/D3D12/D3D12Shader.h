#ifndef _TRAP_D3D12SHADER_H_
#define _TRAP_D3D12SHADER_H_

namespace TRAP::Graphics::API
{
	class D3D12Shader : public Shader
	{
	public:
		D3D12Shader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource);
		D3D12Shader(const D3D12Shader&) = default;
		D3D12Shader& operator=(const D3D12Shader&) = default;
		D3D12Shader(D3D12Shader&&) = default;
		D3D12Shader& operator=(D3D12Shader&&) = default;
		~D3D12Shader() = default;

		void Bind() const override;
		void Unbind() const override;

		void SetVSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot) override;
		void SetFSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot) override;
		void SetGSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot) override;
		void SetTCSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot) override;
		void SetTESSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot) override;
		void SetCSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot) override;

		void SetVSUserUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetFSUserUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetGSUserUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetTCSUserUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetTESUserUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetCSUserUniformBuffer(uint8_t* data, unsigned int size) override;

		const std::string& GetName() const override;
		
		const std::string& GetVSFilePath() const override;
		const std::string& GetFSFilePath() const override;
		const std::string& GetGSFilePath() const override;
		const std::string& GetTCSFilePath() const override;
		const std::string& GetTESFilePath() const override;
		const std::string& GetCSFilePath() const override;

		const std::string& GetVSSource() const override;
		const std::string& GetFSSource() const override;
		const std::string& GetGSSource() const override;
		const std::string& GetTCSSource() const override;
		const std::string& GetTESSource() const override;
		const std::string& GetCSSource() const override;

		const ShaderUniformBufferList& GetVSSystemUniforms() const override;
		const ShaderUniformBufferList& GetFSSystemUniforms() const override;
		const ShaderUniformBufferList& GetGSSystemUniforms() const override;
		const ShaderUniformBufferList& GetTCSSystemUniforms() const override;
		const ShaderUniformBufferList& GetTESSystemUniforms() const override;
		const ShaderUniformBufferList& GetCSSystemUniforms() const override;
		ShaderUniformBufferDeclaration* GetVSUserUniformBuffer() const override;
		ShaderUniformBufferDeclaration* GetFSUserUniformBuffer() const override;
		ShaderUniformBufferDeclaration* GetGSUserUniformBuffer() const override;
		ShaderUniformBufferDeclaration* GetTCSUserUniformBuffer() const override;
		ShaderUniformBufferDeclaration* GetTESUserUniformBuffer() const override;
		ShaderUniformBufferDeclaration* GetCSUserUniformBuffer() const override;
		const ShaderResourceList& GetResource() const override;

	private:
		friend class Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		//unsigned int m_handle;
		std::string m_name, m_VSFilepath, m_FSFilepath, m_GSFilepath, m_TCSFilepath, m_TESFilepath, m_CSFilepath;
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

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetVSSystemUniforms() const
{
	return m_VSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetFSSystemUniforms() const
{
	return m_FSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetGSSystemUniforms() const
{
	return m_GSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetTCSSystemUniforms() const
{
	return m_TCSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetTESSystemUniforms() const
{
	return m_TESUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::D3D12Shader::GetCSSystemUniforms() const
{
	return m_CSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetVSUserUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetFSUserUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetGSUserUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetTCSUserUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetTESUserUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::D3D12Shader::GetCSUserUniformBuffer() const
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

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetVSFilePath() const
{
	return m_VSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetFSFilePath() const
{
	return m_FSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetGSFilePath() const
{
	return m_GSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetTCSFilePath() const
{
	return m_TCSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetTESFilePath() const
{
	return m_TESFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::D3D12Shader::GetCSFilePath() const
{
	return m_CSFilepath;
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

#endif /*_TRAP_D3D12SHADER_H_*/