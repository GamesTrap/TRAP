#ifndef _TRAP_OPENGLSHADER_H_
#define _TRAP_OPENGLSHADER_H_

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderUniform.h"
#include "Graphics/Shaders/ShaderResource.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Maths/Maths.h"
#include "OpenGLShaderUniform.h"

namespace TRAP::Graphics::API
{
	class OpenGLShader final : public Graphics::Shader
	{
	public:
		OpenGLShader(std::string name, std::string Source);
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

		void SetVSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetFSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetGSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetTCSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetTESUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetCSUniformBuffer(uint8_t* data, unsigned int size) override;

		void SetUniform(const std::string& name, uint8_t* data);
		void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, uint8_t* data, int32_t offset) const;

		const std::string& GetName() const override;

		const std::string& GetFilePath() const override;

		const std::string& GetVSSource() const override;
		const std::string& GetFSSource() const override;
		const std::string& GetGSSource() const override;
		const std::string& GetTCSSource() const override;
		const std::string& GetTESSource() const override;
		const std::string& GetCSSource() const override;

		unsigned int GetHandle() const;

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
		static unsigned int Compile(std::array<std::string*, 6>& shaders);

		void Parse(const std::string& VSSource, const std::string& FSSource, const std::string& GSSource, const std::string& TCSSource, const std::string& TESSource, const std::string& CSSource);
		void ParseUniform(const std::string& statement, ShaderType shaderType);
		void ParseUniformStruct(const std::string& block);

		static bool IsTypeStringResource(const std::string& type);

		ShaderStruct* FindStruct(std::string_view name);

		void ResolveUniforms();
		static bool IsSystemUniform(ShaderUniformDeclaration* uniform);
		int32_t GetUniformLocation(const std::string& name) const;

		static ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name, ShaderUniformBufferDeclaration* buffer);
		ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name);

		void ResolveAndSetUniforms(ShaderUniformBufferDeclaration* buffer, uint8_t* data) const;
		void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, uint8_t* data) const;

		void SetUniformStruct(OpenGLShaderUniformDeclaration* uniform, uint8_t* data, int32_t offset) const;

		void SetUniform1f(const std::string& name, float value) const;
		void SetUniform1fv(const std::string& name, float* value, int32_t count) const;
		void SetUniform1i(const std::string& name, int32_t value) const;
		void SetUniform1iv(const std::string& name, int32_t* value, int32_t count) const;
		void SetUniform2f(const std::string& name, const Math::Vec2& vector) const;
		void SetUniform3f(const std::string& name, const Math::Vec3& vector) const;
		void SetUniform4f(const std::string& name, const Math::Vec4& vector) const;
		void SetUniformMat3(const std::string& name, const Math::Mat3& matrix) const;
		void SetUniformMat4(const std::string& name, const Math::Mat4& matrix) const;

		static void SetUniform1f(unsigned int location, float value);
		static void SetUniform1fv(unsigned int location, float* value, int32_t count);
		static void SetUniform1i(unsigned int location, int32_t value);
		static void SetUniform1iv(unsigned int location, int32_t* value, int32_t count);
		static void SetUniform2f(unsigned int location, const Math::Vec2& vector);
		static void SetUniform3f(unsigned int location, const Math::Vec3& vector);
		static void SetUniform4f(unsigned int location, const Math::Vec4& vector);
		static void SetUniformMat3(unsigned int location, const Math::Mat3& matrix);
		static void SetUniformMat4(unsigned int location, const Math::Mat4& matrix);

		static void PreProcess(const std::string& source, std::array<std::string*, 6>& shaders);

		friend class ::TRAP::Graphics::Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		unsigned int m_handle;
		mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;
		std::string m_name, m_filepath, m_source;
		std::string m_VSSource, m_FSSource, m_GSSource, m_TCSSource, m_TESSource, m_CSSource;

		ShaderUniformBufferList m_VSUniformBuffers;
		ShaderUniformBufferList m_FSUniformBuffers;
		ShaderUniformBufferList m_GSUniformBuffers;
		ShaderUniformBufferList m_TCSUniformBuffers;
		ShaderUniformBufferList m_TESUniformBuffers;
		ShaderUniformBufferList m_CSUniformBuffers;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_VSUniformBuffer;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_FSUniformBuffer;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_GSUniformBuffer;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_TCSUniformBuffer;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_TESUniformBuffer;
		std::unique_ptr<OpenGLShaderUniformBufferDeclaration> m_CSUniformBuffer;
		ShaderResourceList m_resources;
		ShaderStructList m_structs;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetVSUniforms() const
{
	return m_VSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetFSUniforms() const
{
	return m_FSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetGSUniforms() const
{
	return m_GSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetTCSUniforms() const
{
	return m_TCSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetTESUniforms() const
{
	return m_TESUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::OpenGLShader::GetCSUniforms() const
{
	return m_CSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned TRAP::Graphics::API::OpenGLShader::GetHandle() const
{
	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetVSUniformBuffer() const
{
	return m_VSUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetFSUniformBuffer() const
{
	return m_FSUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetGSUniformBuffer() const
{
	return m_GSUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetTCSUniformBuffer() const
{
	return m_TCSUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetTESUniformBuffer() const
{
	return m_TESUniformBuffer.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::OpenGLShader::GetCSUniformBuffer() const
{
	return m_CSUniformBuffer.get();
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

inline const std::string& TRAP::Graphics::API::OpenGLShader::GetFilePath() const
{
	return m_filepath;
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