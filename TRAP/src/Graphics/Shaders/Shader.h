#ifndef _TRAP_SHADER_H_
#define _TRAP_SHADER_H_

namespace TRAP::Graphics::API
{

	enum class ShaderType
	{
		UNKNOWN,
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		TESSELLATIONCONTROL,
		TESSELLATIONEVALUATION,
		COMPUTE
	};

	class Shader
	{
	public:
		Shader() = default;
		Shader(const Shader&) = default;
		Shader& operator=(const Shader&) = default;
		Shader(Shader&&) = default;
		Shader& operator=(Shader&&) = default;
		virtual ~Shader() = default;

		static const Shader* s_CurrentlyBound;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetVSUniformBuffer(uint8_t* data, unsigned int size) = 0;
		virtual void SetFSUniformBuffer(uint8_t* data, unsigned int size) = 0;
		virtual void SetGSUniformBuffer(uint8_t* data, unsigned int size) = 0;
		virtual void SetTCSUniformBuffer(uint8_t* data, unsigned int size) = 0;
		virtual void SetTESUniformBuffer(uint8_t* data, unsigned int size) = 0;
		virtual void SetCSUniformBuffer(uint8_t* data, unsigned int size) = 0;

		virtual const ShaderUniformBufferList& GetVSUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetFSUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetGSUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetTCSUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetTESUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetCSUniforms() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetVSUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetFSUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetGSUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetTCSUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetTESUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetCSUniformBuffer() const = 0;

		virtual const ShaderResourceList& GetResource() const = 0;

		virtual const std::string& GetName() const = 0;

		virtual const std::string& GetFilePath() const = 0;

		virtual const std::string& GetVSSource() const = 0;
		virtual const std::string& GetFSSource() const = 0;
		virtual const std::string& GetGSSource() const = 0;
		virtual const std::string& GetTCSSource() const = 0;
		virtual const std::string& GetTESSource() const = 0;
		virtual const std::string& GetCSSource() const = 0;

		static std::unique_ptr<Shader> CreateFromFile(const std::string& name, const std::string& filePath, Shader* address = nullptr);
		static std::unique_ptr<Shader> CreateFromSource(const std::string& name, const std::string& VSSource, const std::string& FSSource, const std::string& GSSource, const std::string& TCSSource, const std::string& TESSource, const std::string& CSSource);;
	};
}

#endif /*_TRAP_SHADER_H_*/