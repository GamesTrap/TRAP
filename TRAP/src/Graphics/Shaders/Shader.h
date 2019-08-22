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

		virtual const std::string& GetName() const = 0;

		virtual const std::string& GetFilePath() const = 0;

		virtual const std::string& GetVSSource() const = 0;
		virtual const std::string& GetFSSource() const = 0;
		virtual const std::string& GetGSSource() const = 0;
		virtual const std::string& GetTCSSource() const = 0;
		virtual const std::string& GetTESSource() const = 0;
		virtual const std::string& GetCSSource() const = 0;

		static std::unique_ptr<Shader> CreateFromFile(const std::string& name, const std::string& filePath, Shader* address = nullptr);
		static std::unique_ptr<Shader> CreateFromSource(const std::string& name, const std::string& VSSource, const std::string& FSSource, const std::string& GSSource, const std::string& TCSSource, const std::string& TESSource, const std::string& CSSource);;

		static std::string ShaderTypeToString(ShaderType type);
	};
}

#endif /*_TRAP_SHADER_H_*/