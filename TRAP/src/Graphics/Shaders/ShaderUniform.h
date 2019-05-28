#pragma once

namespace TRAP::Graphics::API
{
	class ShaderUniformDeclaration
	{
	public:
		ShaderUniformDeclaration() = default;
		ShaderUniformDeclaration(const ShaderUniformDeclaration&) = default;
		ShaderUniformDeclaration& operator=(const ShaderUniformDeclaration&) = default;
		ShaderUniformDeclaration(ShaderUniformDeclaration&&) = default;
		ShaderUniformDeclaration& operator=(ShaderUniformDeclaration&&) = default;
		virtual ~ShaderUniformDeclaration() = default;

		virtual const std::string& GetName() const = 0;
		virtual unsigned int GetSize() const = 0;
		virtual unsigned int GetCount() const = 0;
		virtual unsigned int GetOffset() const = 0;

	protected:
		virtual void SetOffset(unsigned int offset) = 0;

	private:
		friend class Shader;
		friend class OpenGLShader;
		friend class D3D12Shader;
		friend class VulkanShader;
		friend class ShaderStruct;
	};

	typedef std::vector<std::unique_ptr<ShaderUniformDeclaration>> ShaderUniformList;

	class ShaderUniformBufferDeclaration
	{
	public:
		ShaderUniformBufferDeclaration() = default;
		ShaderUniformBufferDeclaration(const ShaderUniformBufferDeclaration&) = default;
		ShaderUniformBufferDeclaration& operator=(const ShaderUniformBufferDeclaration&) = default;
		ShaderUniformBufferDeclaration(ShaderUniformBufferDeclaration&&) = default;
		ShaderUniformBufferDeclaration& operator=(ShaderUniformBufferDeclaration&&) = default;
		virtual ~ShaderUniformBufferDeclaration() = default;

		virtual std::string_view GetName() const = 0;
		virtual unsigned int GetRegister() const = 0;
		virtual unsigned int GetShaderType() const = 0;
		virtual unsigned int GetSize() const = 0;
		virtual const ShaderUniformList& GetUniformDeclarations() const = 0;

		virtual const ShaderUniformDeclaration* FindUniform(const std::string& name) = 0;
	};

	typedef std::vector<std::unique_ptr<ShaderUniformBufferDeclaration>> ShaderUniformBufferList;

	class ShaderStruct
	{
	public:
		explicit ShaderStruct(std::string name);

		void AddField(ShaderUniformDeclaration* field);

		void SetOffset(unsigned int offset);

		std::string_view GetName() const;
		unsigned int GetSize() const;
		unsigned int GetOffset() const;
		const std::vector<std::unique_ptr<ShaderUniformDeclaration>>& GetFields() const;

	private:
		friend class Shader;

		std::string m_name;
		std::vector<std::unique_ptr<ShaderUniformDeclaration>> m_fields;
		unsigned int m_size;
		unsigned int m_offset;
	};

	typedef std::vector<std::unique_ptr<ShaderStruct>> ShaderStructList;
}
