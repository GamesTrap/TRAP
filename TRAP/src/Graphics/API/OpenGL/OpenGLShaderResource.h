#ifndef _TRAP_OPENGLSHADERRESOURCE_H_
#define _TRAP_OPENGLSHADERRESOURCE_H_

#include "Graphics/Shaders/ShaderResource.h"

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

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::OpenGLShaderResourceDeclaration::GetName() const
{	
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned TRAP::Graphics::API::OpenGLShaderResourceDeclaration::GetRegister() const
{
	return m_register;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned TRAP::Graphics::API::OpenGLShaderResourceDeclaration::GetCount() const
{
	return m_count;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::OpenGLShaderResourceDeclaration::Type TRAP::Graphics::API::OpenGLShaderResourceDeclaration::GetType() const
{
	return m_type;
}

#endif /*_TRAP_OPENGLSHADERRESOURCE_H_*/