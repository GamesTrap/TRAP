#ifndef _TRAP_OPENGLSHADER_H_
#define _TRAP_OPENGLSHADER_H_

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderManager.h"

namespace TRAP::Graphics::API
{
	class OpenGLShader final : public Shader
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

		uint32_t GetHandle() const;;

	private:
		static uint32_t Compile(std::array<std::string*, 6>& shaders);
		
		static bool CompileShader(ShaderType type, const char* source, uint32_t& handle);
		static bool CreateProgram(std::array<std::string*, 6>& shaders,
		                          uint32_t& vertex,
		                          uint32_t& fragment,
		                          uint32_t& geometry,
		                          uint32_t& tessControl,
		                          uint32_t& tessEval,
		                          uint32_t& compute,
		                          uint32_t& handle);
		static void LinkProgram(int32_t& linkResult, int32_t& validateResult, const uint32_t& handle);
		static void DeleteShaders(std::array<std::string*, 6>& shaders,
		                          const uint32_t& vertex,
		                          const uint32_t& fragment,
		                          const uint32_t& geometry,
		                          const uint32_t& tessControl,
		                          const uint32_t& tessEval,
		                          const uint32_t& compute,
								  const uint32_t& handle);

		static GLenum ShaderTypeToOpenGL(ShaderType type);

		static bool IsTypeOpaque(GLenum type);

		void CheckForUniforms();

		static void PreProcess(const std::string& source, std::array<std::string*, 6>& shaders);

		friend class ::TRAP::Graphics::Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		uint32_t m_handle;
		std::string m_name, m_filepath, m_source;
		std::string m_VSSource, m_FSSource, m_GSSource, m_TCSSource, m_TESSource, m_CSSource;
	};
}

#endif /*_TRAP_OPENGLSHADER_H_*/