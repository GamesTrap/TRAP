#ifndef _TRAP_OPENGLSHADER_H_
#define _TRAP_OPENGLSHADER_H_

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderManager.h"

namespace TRAP::Graphics::API
{
	class OpenGLShader final : public Shader
	{
	public:
		OpenGLShader(std::string name, const std::string& source);
		OpenGLShader(std::string name, std::vector<uint32_t>& source);
		OpenGLShader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource);
		OpenGLShader(const OpenGLShader&) = delete;
		OpenGLShader& operator=(const OpenGLShader&) = delete;
		OpenGLShader(OpenGLShader&&) = default;
		OpenGLShader& operator=(OpenGLShader&&) = default;
		~OpenGLShader();
		
		void Shutdown() const;
		void Bind() const override;
		void Unbind() const override;

		uint32_t GetHandle() const;;

	private:
		void InitGLSL(const std::string& source);
		void InitGLSL(std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource);
		void InitSPIRV(std::vector<uint32_t>& source);
		
		static uint32_t CompileGLSL(std::array<std::string, 6>& shaders);
		
		static bool CompileGLSLShader(ShaderType type, const char* source, uint32_t& handle);
		static bool CreateGLSLProgram(std::array<std::string, 6>& shaders,
		                          uint32_t& vertex,
		                          uint32_t& fragment,
		                          uint32_t& geometry,
		                          uint32_t& tessControl,
		                          uint32_t& tessEval,
		                          uint32_t& compute,
		                          uint32_t& handle);
		static void LinkGLSLProgram(int32_t& linkResult, int32_t& validateResult, const uint32_t& handle);
		static void DeleteGLSLShaders(std::array<std::string, 6>& shaders,
		                          const uint32_t& vertex,
		                          const uint32_t& fragment,
		                          const uint32_t& geometry,
		                          const uint32_t& tessControl,
		                          const uint32_t& tessEval,
		                          const uint32_t& compute,
								  const uint32_t& handle);

		static std::array<std::string, 6> ConvertSPIRVToGLSL(std::array<std::vector<uint32_t>, 6>& SPIRVShaders);
		
		static GLenum ShaderTypeToOpenGL(ShaderType type);

		static bool IsTypeOpaque(GLenum type);

		void CheckForUniforms();

		static bool PreProcess(const std::string& source, std::array<std::string, 6>& shaders);

		uint32_t m_handle;
	};
}

#endif /*_TRAP_OPENGLSHADER_H_*/