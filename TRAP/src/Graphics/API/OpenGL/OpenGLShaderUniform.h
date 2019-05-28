#pragma once

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
