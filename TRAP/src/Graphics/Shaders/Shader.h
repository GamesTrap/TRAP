#ifndef _TRAP_SHADER_H_
#define _TRAP_SHADER_H_

namespace TRAP::Graphics
{
	enum class ShaderType
	{
		Unknown = 0,
		Vertex,
		Fragment,
		Geometry,
		Tessellation_Control,
		Tessellation_Evaluation,
		Compute
	};

	class Shader
	{
	protected:
		Shader() = default;
	public:
		Shader(const Shader&) = default;
		Shader& operator=(const Shader&) = default;
		Shader(Shader&&) = default;
		Shader& operator=(Shader&&) = default;
		virtual ~Shader() = default;

		static const Shader* s_CurrentlyBound;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		std::string_view GetName() const;

		std::string_view GetFilePath() const;

		static Scope<Shader> CreateFromFile(const std::string& name, const std::string& filePath);
		static Scope<Shader> CreateFromFile(const std::string& filepath);
		static Scope<Shader> CreateFromSource(const std::string& name,
		                                      const std::string& VSSource,
		                                      const std::string& FSSource,
		                                      const std::string& GSSource = "",
		                                      const std::string& TCSSource = "",
		                                      const std::string& TESSource = "",
		                                      const std::string& CSSource = "");

		static std::string ShaderTypeToString(ShaderType type);

	protected:
		std::string m_name;
		std::string m_filepath;
		
	private:
		static bool CheckSPIRVMagicNumber(const std::string& filePath);
		static std::vector<uint32_t> Convert8To32(const std::vector<std::byte>& source);
	};
}

#endif /*_TRAP_SHADER_H_*/