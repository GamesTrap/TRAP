#ifndef _TRAP_SHADERUNIFORM_H_
#define _TRAP_SHADERUNIFORM_H_

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

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::ShaderStruct::SetOffset(unsigned offset)
{
	m_offset = offset;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::API::ShaderStruct::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::API::ShaderStruct::GetSize() const
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::API::ShaderStruct::GetOffset() const
{
	return m_offset;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::vector<std::unique_ptr<TRAP::Graphics::API::ShaderUniformDeclaration>>& TRAP::Graphics::API::ShaderStruct::GetFields() const
{
	return m_fields;
}

#endif /*_TRAP_SHADERUNIFORM_H_*/