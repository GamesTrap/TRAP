#pragma once

namespace TRAP::Graphics::API
{
	class ShaderResourceDeclaration
	{
	public:
		ShaderResourceDeclaration() = default;
		ShaderResourceDeclaration(const ShaderResourceDeclaration&) = default;
		ShaderResourceDeclaration& operator=(const ShaderResourceDeclaration&) = default;
		ShaderResourceDeclaration(ShaderResourceDeclaration&&) = default;
		ShaderResourceDeclaration& operator=(ShaderResourceDeclaration&&) = default;
		virtual ~ShaderResourceDeclaration() = default;

		virtual const std::string& GetName() const = 0;
		virtual unsigned int GetRegister() const = 0;
		virtual unsigned int GetCount() const = 0;
	};

	typedef std::vector<std::unique_ptr<ShaderResourceDeclaration>> ShaderResourceList;
}
