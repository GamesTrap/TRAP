#ifndef _TRAP_OPENGLSHADERUNIFORM_H_
#define _TRAP_OPENGLSHADERUNIFORM_H_

namespace TRAP::Graphics::API
{
	class OpenGLShaderUniformDeclaration final : public ShaderUniformDeclaration
	{
	public:
		enum class Type
		{
			NONE,
			FLOAT32,
			VEC2,
			VEC3,
			VEC4,
			MAT3,
			MAT4,
			INT32,
			STRUCT
		};

		OpenGLShaderUniformDeclaration(Type type, std::string name, unsigned int count = 1);
		OpenGLShaderUniformDeclaration(ShaderStruct* uniformStruct, std::string name, unsigned int count = 1);

		const std::string& GetName() const override;
		unsigned int GetSize() const override;
		unsigned int GetCount() const override;
		unsigned int GetOffset() const override;
		unsigned int GetAbsoluteOffset() const;

		int32_t GetLocation() const;
		Type GetType() const;
		const ShaderStruct& GetShaderUniformStruct() const;

		static unsigned int SizeOfUniformType(Type type);
		static Type StringToType(std::string_view type);
		static std::string TypeToString(Type type);

	protected:
		void SetOffset(unsigned int offset) override;

	private:
		friend class OpenGLShader;
		friend class OpenGLShaderUniformBufferDeclaration;

		std::string m_name;
		unsigned int m_size;
		unsigned int m_count;
		unsigned int m_offset;

		Type m_type;
		const std::unique_ptr<ShaderStruct>& m_struct;
		mutable int32_t m_location;
	};

	struct OpenGLShaderUniformField
	{
		OpenGLShaderUniformDeclaration::Type Type;
		std::string Name;
		unsigned int Count;
		mutable unsigned int Size;
		mutable int32_t Location;
	};

	class OpenGLShaderUniformBufferDeclaration final : public ShaderUniformBufferDeclaration
	{
	public:
		OpenGLShaderUniformBufferDeclaration(std::string name, unsigned int shaderType);

		void PushUniform(OpenGLShaderUniformDeclaration* uniform);

		std::string_view GetName() const override;
		unsigned int GetRegister() const override;
		unsigned int GetShaderType() const override;
		unsigned int GetSize() const override;
		const ShaderUniformList& GetUniformDeclarations() const override;

		ShaderUniformDeclaration* FindUniform(const std::string& name) override;

	private:
		friend class Shader;

		std::string m_name;
		ShaderUniformList m_uniforms;
		unsigned int m_register;
		unsigned int m_size;
		unsigned int m_shaderType; //0 = VS, 1 = PS, 2 = GS, 3 = TCS, 4 = TES, 5 = CS
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShaderUniformDeclaration::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::API::OpenGLShaderUniformDeclaration::GetSize() const
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::API::OpenGLShaderUniformDeclaration::GetCount() const
{
	return m_count;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::API::OpenGLShaderUniformDeclaration::GetOffset() const
{
	return m_offset;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::API::OpenGLShaderUniformDeclaration::GetAbsoluteOffset() const
{
	return m_struct ? m_struct->GetOffset() + m_offset : m_offset;
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::Graphics::API::OpenGLShaderUniformDeclaration::GetLocation() const
{
	return m_location;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::OpenGLShaderUniformDeclaration::Type TRAP::Graphics::API::OpenGLShaderUniformDeclaration::GetType() const
{
	return m_type;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::API::OpenGLShaderUniformBufferDeclaration::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::API::OpenGLShaderUniformBufferDeclaration::GetRegister() const
{
	return m_register;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::API::OpenGLShaderUniformBufferDeclaration::GetShaderType() const
{
	return m_shaderType;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::API::OpenGLShaderUniformBufferDeclaration::GetSize() const
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformList& TRAP::Graphics::API::OpenGLShaderUniformBufferDeclaration::GetUniformDeclarations() const
{
	return m_uniforms;
}

#endif /*_TRAP_OPENGLSHADERUNIFORM_H_*/