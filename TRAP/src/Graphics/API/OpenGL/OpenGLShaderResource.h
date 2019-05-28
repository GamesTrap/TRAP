#pragma once

namespace TRAP::Graphics::API
{
	class OpenGLShaderResourceDeclaration final : public ShaderResourceDeclaration
	{
	public:
		enum class Type
		{
			NONE,
			TEXTURE2D,
			TEXTURECUBE,
			TEXTURESHADOW
		};

		OpenGLShaderResourceDeclaration(Type type, const std::string& name, unsigned int count);

		const std::string& GetName() const override;
		unsigned int GetRegister() const override;
		unsigned int GetCount() const override;

		Type GetType() const;

		static Type StringToType(const std::string& type);
		static std::string TypeToString(Type type);

	private:
		friend class OpenGLShader;

		std::string m_name;
		unsigned int m_register;
		unsigned int m_count;
		Type m_type;
	};
}
