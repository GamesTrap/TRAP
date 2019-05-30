#ifndef _TRAP_SHADER_H_
#define _TRAP_SHADER_H_

namespace TRAP::Graphics::API
{
	constexpr int SHADER_VERTEX_INDEX = 0;
	constexpr int SHADER_UV_INDEX = 1;
	constexpr int SHADER_MASK_UV_INDEX = 2;
	constexpr int SHADER_TID_INDEX = 3;
	constexpr int SHADER_MID_INDEX = 4;
	constexpr int SHADER_COLOR_INDEX = 5;

	constexpr const char* SHADER_UNIFORM_PROJECTION_MATRIX_NAME = "sys_ProjectionMatrix";
	constexpr const char* SHADER_UNIFORM_VIEW_MATRIX_NAME = "sys_ViewMatrix";
	constexpr const char* SHADER_UNIFORM_MODEL_MATRIX_NAME = "sys_ModelMatrix";

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

		virtual void SetVSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) = 0;
		virtual void SetFSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) = 0;
		virtual void SetGSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) = 0;
		virtual void SetTCSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) = 0;
		virtual void SetTESSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) = 0;
		virtual void SetCSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) = 0;

		virtual void SetVSUserUniformBuffer(uint8_t* data, unsigned int size) = 0;
		virtual void SetFSUserUniformBuffer(uint8_t* data, unsigned int size) = 0;
		virtual void SetGSUserUniformBuffer(uint8_t* data, unsigned int size) = 0;
		virtual void SetTCSUserUniformBuffer(uint8_t* data, unsigned int size) = 0;
		virtual void SetTESUserUniformBuffer(uint8_t* data, unsigned int size) = 0;
		virtual void SetCSUserUniformBuffer(uint8_t* data, unsigned int size) = 0;

		virtual const ShaderUniformBufferList& GetVSSystemUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetFSSystemUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetGSSystemUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetTCSSystemUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetTESSystemUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetCSSystemUniforms() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetVSUserUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetFSUserUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetGSUserUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetTCSUserUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetTESUserUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration* GetCSUserUniformBuffer() const = 0;

		virtual const ShaderResourceList& GetResource() const = 0;

		virtual const std::string& GetName() const = 0;

		virtual const std::string& GetVSFilePath() const = 0;
		virtual const std::string& GetFSFilePath() const = 0;
		virtual const std::string& GetGSFilePath() const = 0;
		virtual const std::string& GetTCSFilePath() const = 0;
		virtual const std::string& GetTESFilePath() const = 0;
		virtual const std::string& GetCSFilePath() const = 0;

		virtual const std::string& GetVSSource() const = 0;
		virtual const std::string& GetFSSource() const = 0;
		virtual const std::string& GetGSSource() const = 0;
		virtual const std::string& GetTCSSource() const = 0;
		virtual const std::string& GetTESSource() const = 0;
		virtual const std::string& GetCSSource() const = 0;

		static std::unique_ptr<Shader> CreateFromFile(const std::string& name, const std::string& VSFilePath, const std::string& FSFilePath, const std::string& GSFilePath, const std::string& TCSFilePath, const std::string& TESFilePath, const std::string& CSFilePath, Shader* address = nullptr);
		static std::unique_ptr<Shader> CreateFromSource(const std::string& name, const std::string& VSSource, const std::string& FSSource, const std::string& GSSource, const std::string& TCSSource, const std::string& TESSource, const std::string& CSSource);;
	};
}

#endif /*_TRAP_SHADER_H_*/