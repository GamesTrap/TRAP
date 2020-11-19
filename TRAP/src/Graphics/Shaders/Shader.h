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

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		const std::string& GetName() const;

		const std::string& GetFilePath() const;

		static Scope<Shader> CreateFromFile(const std::string& name, const std::string& filePath);
		static Scope<Shader> CreateFromFile(const std::string& filepath);
		static Scope<Shader> CreateFromSource(const std::string& name,
		                                      std::string_view VSSource,
		                                      std::string_view FSSource,
		                                      std::string_view GSSource = "",
		                                      std::string_view TCSSource = "",
		                                      std::string_view TESSource = "",
		                                      std::string_view CSSource = "");

		static std::string ShaderTypeToString(ShaderType type);

	protected:
		std::string m_name;
		std::string m_filepath;
		
	private:
		static bool CheckSPIRVMagicNumber(std::string_view filePath);
		static std::vector<uint32_t> Convert8To32(const std::vector<uint8_t>& source);
	};
}

#endif /*_TRAP_SHADER_H_*/