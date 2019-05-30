#ifndef _TRAP_OPENGLSHADER_H_
#define _TRAP_OPENGLSHADER_H_

namespace TRAP::Graphics::API
{
	struct OpenGLShaderErrorInfo
	{
		unsigned int Shader{};
		std::array<std::string, 6> Message;
		std::array<unsigned int, 6> Line{};
	};

	class OpenGLShader final : public Shader
	{
	public:
		OpenGLShader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource);
		OpenGLShader(const OpenGLShader&) = delete;
		OpenGLShader& operator=(const OpenGLShader&) = delete;
		OpenGLShader(OpenGLShader&&) = default;
		OpenGLShader& operator=(OpenGLShader&&) = default;
		~OpenGLShader();

		void Init();
		void Shutdown() const;
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

		void SetUniform(const std::string& name, uint8_t* data);
		void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, uint8_t* data, int32_t offset) const;

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

		unsigned int GetHandle() const;

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
		static unsigned int Compile(std::array<std::string*, 6>& shaders, OpenGLShaderErrorInfo& info = OpenGLShaderErrorInfo()); //TODO

		void Parse(const std::string& VSSource, const std::string& FSSource, const std::string& GSSource, const std::string& TCSSource, const std::string& TESSource, const std::string& CSSource);
		void ParseUniform(const std::string& statement, unsigned int shaderType);
		void ParseUniformStruct(const std::string& block, unsigned int shaderType);

		static bool IsTypeStringResource(const std::string& type);

		ShaderStruct* FindStruct(std::string_view name);

		void ResolveUniforms();
		static bool IsSystemUniform(ShaderUniformDeclaration* uniform);
		int32_t GetUniformLocation(const std::string& name) const;

		static ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name, ShaderUniformBufferDeclaration* buffer);
		ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name);

		void ResolveAndSetUniforms(ShaderUniformBufferDeclaration* buffer, uint8_t* data, unsigned int size) const;
		void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, uint8_t* data, unsigned int size) const;

		void SetUniformStruct(OpenGLShaderUniformDeclaration* uniform, uint8_t* data, int32_t offset) const;

		void SetUniform1f(const std::string& name, float value) const;
		void SetUniform1fv(const std::string& name, float* value, int32_t count) const;
		void SetUniform1i(const std::string& name, int32_t value) const;
		void SetUniform1iv(const std::string& name, int32_t* value, int32_t count) const;
		void SetUniform2f(const std::string& name, const Maths::Vec2& vector) const;
		void SetUniform3f(const std::string& name, const Maths::Vec3& vector) const;
		void SetUniform4f(const std::string& name, const Maths::Vec4& vector) const;
		void SetUniformMat3(const std::string& name, const Maths::Mat3& matrix) const;
		void SetUniformMat4(const std::string& name, const Maths::Mat4& matrix) const;

		static void SetUniform1f(unsigned int location, float value);
		static void SetUniform1fv(unsigned int location, float* value, int32_t count);
		static void SetUniform1i(unsigned int location, int32_t value);
		static void SetUniform1iv(unsigned int location, int32_t* value, int32_t count);
		static void SetUniform2f(unsigned int location, const Maths::Vec2& vector);
		static void SetUniform3f(unsigned int location, const Maths::Vec3& vector);
		static void SetUniform4f(unsigned int location, const Maths::Vec4& vector);
		static void SetUniformMat3(unsigned int location, const Maths::Mat3& matrix);
		static void SetUniformMat4(unsigned int location, const Maths::Mat4& matrix);

		friend class Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		unsigned int m_handle;
		std::string m_name, m_VSFilepath, m_FSFilepath, m_GSFilepath, m_TCSFilepath, m_TESFilepath, m_CSFilepath;
		std::string m_VSSource, m_FSSource, m_GSSource, m_TCSSource, m_TESSource, m_CSSource;

		ShaderUniformBufferList m_VSUniformBuffers;
		ShaderUniformBufferList m_FSUniformBuffers;
		ShaderUniformBufferList m_GSUniformBuffers;
		ShaderUniformBufferList m_TCSUniformBuffers;
		ShaderUniformBufferList m_TESUniformBuffers;
		ShaderUniformBufferList m_CSUniformBuffers;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_VSUserUniformBuffer;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_FSUserUniformBuffer;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_GSUserUniformBuffer;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_TCSUserUniformBuffer;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_TESUserUniformBuffer;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_CSUserUniformBuffer;
		ShaderResourceList m_resources;
		ShaderStructList m_structs;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetVSSystemUniforms() const
{
	return m_VSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetFSSystemUniforms() const
{
	return m_FSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetGSSystemUniforms() const
{
	return m_GSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetTCSSystemUniforms() const
{
	return m_TCSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetTESSystemUniforms() const
{
	return m_TESUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetCSSystemUniforms() const
{
	return m_CSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned TRAP::Graphics::API::OpenGLShader::GetHandle() const
{
	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetVSUserUniformBuffer() const
{
	return m_VSUserUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetFSUserUniformBuffer() const
{
	return m_FSUserUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetGSUserUniformBuffer() const
{
	return m_GSUserUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetTCSUserUniformBuffer() const
{
	return m_TCSUserUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetTESUserUniformBuffer() const
{
	return m_TESUserUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetCSUserUniformBuffer() const
{
	return m_CSUserUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderResourceList& TRAP::Graphics::API::OpenGLShader::GetResource() const
{
	return m_resources;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetVSFilePath() const
{
	return m_VSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetFSFilePath() const
{
	return m_FSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetGSFilePath() const
{
	return m_GSFilepath;
}
//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetTCSFilePath() const
{
	return m_TCSFilepath;
}
//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetTESFilePath() const
{
	return m_TESFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetCSFilePath() const
{
	return m_CSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetVSSource() const
{
	return m_VSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetFSSource() const
{
	return m_FSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetGSSource() const
{
	return m_GSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetTCSSource() const
{
	return m_TCSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetTESSource() const
{
	return m_TESSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetCSSource() const
{
	return m_CSSource;
}

#endif /*_TRAP_OPENGLSHADER_H_*/