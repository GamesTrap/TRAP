#ifndef _TRAP_OPENGLSHADER_H_
#define _TRAP_OPENGLSHADER_H_

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderManager.h"

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

		const std::string& GetName() const override;

		const std::string& GetFilePath() const override;

		const std::string& GetVSSource() const override;
		const std::string& GetFSSource() const override;
		const std::string& GetGSSource() const override;
		const std::string& GetTCSSource() const override;
		const std::string& GetTESSource() const override;
		const std::string& GetCSSource() const override;

		unsigned int GetHandle() const;;

	private:
		static unsigned int Compile(std::array<std::string*, 6>& shaders);
		
		static bool CompileShader(ShaderType type, const char* source, unsigned int& handle);
		static bool CreateProgram(std::array<std::string*, 6>& shaders,
		                          unsigned int& vertex,
		                          unsigned int& fragment,
		                          unsigned int& geometry,
		                          unsigned int& tessControl,
		                          unsigned int& tessEval,
		                          unsigned int& compute,
		                          unsigned int& handle);
		static void LinkProgram(int& linkResult, int& validateResult, const unsigned int& handle);
		static void DeleteShaders(std::array<std::string*, 6>& shaders,
		                          const unsigned int& vertex,
		                          const unsigned int& fragment,
		                          const unsigned int& geometry,
		                          const unsigned int& tessControl,
		                          const unsigned int& tessEval,
		                          const unsigned int& compute,
								  const unsigned int& handle);

		static GLenum ShaderTypeToOpenGL(ShaderType type);

		static bool IsTypeOpaque(GLenum type);

		void CheckForUniforms();

		static void PreProcess(const std::string& source, std::array<std::string*, 6>& shaders);

		friend class ::TRAP::Graphics::Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		unsigned int m_handle;
		std::string m_name, m_filepath, m_source;
		std::string m_VSSource, m_FSSource, m_GSSource, m_TCSSource, m_TESSource, m_CSSource;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned TRAP::Graphics::API::OpenGLShader::GetHandle() const
{
	return m_handle;
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