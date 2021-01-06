#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <array>

#include <glslang/Public/ShaderLang.h>
#include <GlslangToSpv.h>

#define MAKE_ENUM_FLAG(ENUM_TYPE)\
	static inline ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b) { return static_cast<ENUM_TYPE>(static_cast<std::underlying_type<ENUM_TYPE>::type>(a) | \
																		                        static_cast<std::underlying_type<ENUM_TYPE>::type>(b)); } \
	static inline ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b) { return static_cast<ENUM_TYPE>(static_cast<std::underlying_type<ENUM_TYPE>::type>(a) & \
																		                        static_cast<std::underlying_type<ENUM_TYPE>::type>(b)); } \
	static inline ENUM_TYPE operator|=(ENUM_TYPE& a, ENUM_TYPE b) { return a = (a | b); }\
	static inline ENUM_TYPE operator&=(ENUM_TYPE& a, ENUM_TYPE b) { return a = (a & b); }

enum class ShaderStage
{
	None = 0,
	Vertex = 0x00000001,
	TessellationControl = 0x00000002,
	TessellationEvaluation = 0x00000004,
	Geometry = 0x00000008,
	Fragment = 0x00000010,
	Compute = 0x00000020,
	RayTracing = 0x00000040,

	AllGraphics = (static_cast<uint32_t>(Vertex) | static_cast<uint32_t>(TessellationControl) |
	static_cast<uint32_t>(TessellationEvaluation) | static_cast<uint32_t>(Geometry) |
		static_cast<uint32_t>(Fragment)),
	Hull = TessellationControl,
	Domain = TessellationEvaluation,

	SHADER_STAGE_COUNT = 7
};

struct SubShader
{
	std::string Source;
	std::vector<uint32_t> SPIRV{};;
};
struct Shader
{
	std::string FilePath;
	std::string Source;
	ShaderStage Stages;
	std::array<SubShader, static_cast<uint32_t>(ShaderStage::SHADER_STAGE_COUNT)> SubShaderSources{};
};
MAKE_ENUM_FLAG(ShaderStage);

static bool s_glslangInitialized = false;

bool FileOrFolderExists(const std::filesystem::path& path);
std::string ReadTextFile(const std::filesystem::path& filePath);
std::vector<std::string> GetLines(const std::string& string);
std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters);
bool StartsWith(const std::string& string, const std::string& start);
const char* FindToken(const char* str, const std::string& token);
const char* FindToken(const std::string& string, const std::string& token);
std::string GetSuffix(const std::string& name);
std::string ToLower(std::string string);
bool CheckForParameters(int argc);
std::vector<Shader> LoadShaderSources(int argc, char* argv[]);
bool PreProcessGLSL(Shader& shader);
bool ValidateShaderStages(const Shader& shader);
bool CompileGLSLToSPIRV(Shader& shader);
std::unique_ptr<glslang::TShader> PreProcessGLSLForConversion(const char* source, ShaderStage stage, std::string& preProcessedSource);
bool ParseGLSL(glslang::TShader* shader);
bool LinkGLSL(glslang::TShader* shader, glslang::TProgram& program);
std::vector<uint32_t> ConvertToSPIRV(glslang::TShader* shader, ShaderStage stage, glslang::TProgram& program);
void SaveSPIRV(Shader& shader);

//-------------------------------------------------------------------------------------------------------------------//

int main(const int argc, char* argv[])
{
	if (CheckForParameters(argc))
	{
		std::vector<Shader> shaders = LoadShaderSources(argc, argv);

		for(auto& shader : shaders)
		{
			if(!PreProcessGLSL(shader))
			{
				std::cout << "Skipping File!" << '\n';
				continue;
			}
			if(!ValidateShaderStages(shader))
			{
				std::cout << "Skipping File!" << '\n';
				continue;
			}
			
			if(CompileGLSLToSPIRV(shader))
				SaveSPIRV(shader);					
			else
				std::cout << "Skipping File!" << '\n';

			std::cout << '\n';
		}
	}
	
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

bool FileOrFolderExists(const std::filesystem::path& path)
{
	if (!exists(path))
	{
		std::cout << "File/Folder: " << path << " doesn't exist!" << '\n';
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string ReadTextFile(const std::filesystem::path& filePath)
{
	std::ifstream file(filePath);
	if (file.is_open())
	{
		std::string line;
		std::string result;

		while (std::getline(file, line))
		{
			if(!line.empty() && line.back() == '\r')
				line.pop_back();

			result += line;
			result += '\n';
		}

		file.close();
		return result;
	}

	std::cout << "Could not open File: " << filePath << '\n';
	
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> GetLines(const std::string& string)
{
	return SplitString(string, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
{
	std::size_t start = 0;
	std::size_t end = string.find_first_of(delimiters);

	std::vector<std::string> result;

	while (end <= std::string::npos)
	{
		std::string token = string.substr(start, end - start);

		if (!token.empty())
			result.push_back(token);

		if (end == std::string::npos)
			break;

		start = end + 1;
		end = string.find_first_of(delimiters, start);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool StartsWith(const std::string& string, const std::string& start)
{
	return string.find(start) == 0;
}

//-------------------------------------------------------------------------------------------------------------------//

const char* FindToken(const char* str, const std::string& token)
{
	const char* t = str;
	while ((t = strstr(t, token.data())))
	{
		const bool left = str == t || isspace(t[-1]);
		const bool right = !t[token.size()] || isspace(t[token.size()]);
		if (left && right)
			return t;

		t += token.size();
	}
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

const char* FindToken(const std::string& string, const std::string& token)
{
	return FindToken(string.c_str(), token);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string GetSuffix(const std::string& name)
{
	const std::size_t pos = name.rfind('.');

	return (pos == std::string::npos) ? "" : name.substr(name.rfind('.') + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string ToLower(std::string string)
{
	std::transform(string.begin(), string.end(), string.begin(), ::tolower);

	return string;
}

//-------------------------------------------------------------------------------------------------------------------//

bool CheckForParameters(const int argc)
{
	if (argc < 2)
	{
		std::cout << "No Console Parameters found!" << '\n';
		std::cout << "Usage: Drag and Drop a file or a folder on the .exe file to convert TRAP GLSL Shaders to SPIRV" << '\n';

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<Shader> LoadShaderSources(const int argc, char* argv[])
{
	std::vector<Shader> shaders{};
	
	for (int i = 1; i < argc; i++)
	{
		std::cout << "Parameter " << i << ": \"" << argv[i] << "\"" << '\n';

		if (FileOrFolderExists(argv[i]))
		{
			if (!std::filesystem::is_directory(argv[i]))
			{
				if (ToLower(GetSuffix(argv[i])) == "shader")
				{
					std::string source = ReadTextFile(argv[i]);
					if (!source.empty())
					{
						std::string newPath = argv[i];
						newPath = newPath.substr(0, newPath.size() - 7);
						shaders.push_back({ newPath, source });
					}
					else
					{
						std::cout << "File: \"" << argv[i] << "\" is empty!" << '\n';
						std::cout << "Skipping File!" << '\n' << '\n';
					}
				}
				else
				{
					std::cout << "File: \"" << argv[i] << "\" suffix is not \".shader\"!" << '\n';
					std::cout << "Skipping File!" << '\n' << '\n';
				}
			}
			else
			{
				for (const auto& file : std::filesystem::recursive_directory_iterator(argv[i]))
				{
					if (!file.is_directory())
					{
						std::cout << "File: \"" << file.path().string() << "\"" << '\n';

						if (ToLower(GetSuffix(file.path().string())) == "shader")
						{
							std::string source = ReadTextFile(file.path());
							if (!source.empty())
							{
								std::string newPath = file.path().string();
								newPath = newPath.substr(0, newPath.size() - 7);
								shaders.push_back({ newPath, source });
							}
							else
							{
								std::cout << "File: \"" << file.path().string() << "\" is empty!" << '\n';
								std::cout << "Skipping File!" << '\n' << '\n';
							}
						}
						else
						{
							std::cout << "File: \"" << file.path().string() << "\" suffix is not \".shader\"!" << '\n';
							std::cout << "Skipping File!" << '\n' << '\n';
						}
					}
				}
			}
		}
		else
			std::cout << "Skipping File!" << '\n' << '\n';
	}

	return shaders;
}

//-------------------------------------------------------------------------------------------------------------------//

bool PreProcessGLSL(Shader& shader)
{
	ShaderStage currentShaderStage = ShaderStage::None;
	std::vector<std::string> lines = GetLines(shader.Source);

	//Go through every line of the shader source
	for (uint32_t i = 0; i < lines.size(); ++i)
	{
		//Optimization lines converted to lower case
		std::string lowerLine = ToLower(lines[i]);

		//Search for a shader type tag
		if (StartsWith(lowerLine, "#shader"))
		{
			//Detect shader type
			if (FindToken(lowerLine, "vertex"))
			{
				std::cout << "[GLSL] Adding Vertex Shader to \"" << shader.FilePath << "\"" << '\n';
				currentShaderStage = ShaderStage::Vertex;
			}
			else if (FindToken(lowerLine, "fragment") ||
				     FindToken(lowerLine, "pixel"))
			{
				std::cout << "[GLSL] Adding Fragment Shader to \"" << shader.FilePath << "\"" << '\n';
				currentShaderStage = ShaderStage::Fragment;
			}
			else if (FindToken(lowerLine, "geometry"))
			{
				std::cout << "[GLSL] Adding Geometry Shader to \"" << shader.FilePath << "\"" << '\n';
				currentShaderStage = ShaderStage::Geometry;
			}
			else if (FindToken(lowerLine, "tessellation"))
			{
				//Either Control or Evaluation
				if (FindToken(lowerLine, "control"))
				{
					std::cout << "[GLSL] Adding TessellationControl Shader to \"" << shader.FilePath << "\"" << '\n';
					currentShaderStage = ShaderStage::TessellationControl;
				}
				else if (FindToken(lowerLine, "evaluation"))
				{
					std::cout << "[GLSL] Adding TessellationEvaluation Shader to \"" << shader.FilePath << "\"" << '\n';
					currentShaderStage = ShaderStage::TessellationEvaluation;
				}
			}
			else if (FindToken(lowerLine, "compute"))
			{
				std::cout << "[GLSL] Adding Compute Shader to \"" << shader.FilePath << "\"" << '\n';
				currentShaderStage = ShaderStage::Compute;
			}
			//TODO RayTracing Shaders i.e. "RayGen" "AnyHit" "ClosestHit" "Miss" "Intersection" ("Callable")

			//Check for duplicate "#shader XXX" defines
			if (static_cast<uint32_t>(shader.Stages & currentShaderStage))
			{
				std::cout << "[GLSL] Found duplicate \"#shader\" define: " << lines[i] << '\n';
				return false;
			}

			shader.Stages |= currentShaderStage;
		}
		else if (FindToken(lowerLine, "#version")) //Check for unnecessary "#version" define
			std::cout << "[GLSL] Found Tag: \"" << lines[i] << "\" this is unnecessary! Skipping Line: " << i << '\n';
		else if (currentShaderStage != ShaderStage::None) //Add shader code to detected shader stage
		{
			switch (currentShaderStage)
			{
			case ShaderStage::Vertex:
				shader.SubShaderSources[0].Source.append(lines[i] + '\n');
				break;

			case ShaderStage::TessellationControl:
				//case RendererAPI::ShaderStage::Hull:
				shader.SubShaderSources[1].Source.append(lines[i] + '\n');
				break;

			case ShaderStage::TessellationEvaluation:
				//case RendererAPI::ShaderStage::Domain:
				shader.SubShaderSources[2].Source.append(lines[i] + '\n');
				break;

			case ShaderStage::Geometry:
				shader.SubShaderSources[3].Source.append(lines[i] + '\n');
				break;

			case ShaderStage::Fragment:
				shader.SubShaderSources[4].Source.append(lines[i] + '\n');
				break;

			case ShaderStage::Compute:
				shader.SubShaderSources[5].Source.append(lines[i] + '\n');
				break;

			case ShaderStage::RayTracing:
				std::cout << "[GLSL] RayTracing Shader support is WIP!" << '\n';
				return false;

			case ShaderStage::AllGraphics:
			case ShaderStage::SHADER_STAGE_COUNT:
			case ShaderStage::None:
			default:
				std::cout << "[GLSL] Unsupported Shader Type!" << '\n';
				break;
			}
		}
	}

	for(uint32_t i = 0; i < shader.SubShaderSources.size(); ++i)
	{
		if (ToLower(shader.SubShaderSources[i].Source).find("main") == std::string::npos)
		{
			switch(i)
			{
			case 0:
				if(static_cast<uint32_t>(ShaderStage::Vertex & shader.Stages))
				{
					std::cout << "[GLSL] Vertex Shader Couldn't find \"main\" function!" << '\n';
					return false;
				}
				break;

			case 1:
				if (static_cast<uint32_t>(ShaderStage::TessellationControl & shader.Stages))
				{
					std::cout << "[GLSL] TessellationControl Shader Couldn't find \"main\" function!" << '\n';
					return false;
				}
				break;

			case 2:
				if (static_cast<uint32_t>(ShaderStage::TessellationEvaluation & shader.Stages))
				{
					std::cout << "[GLSL] TessellationEvaluation Shader Couldn't find \"main\" function!" << '\n';
					return false;
				}
				break;
					
			case 3:
				if (static_cast<uint32_t>(ShaderStage::Geometry & shader.Stages))
				{
					std::cout << "[GLSL] Geometry Shader Couldn't find \"main\" function!" << '\n';
					return false;
				}
				break;

			case 4:
				if (static_cast<uint32_t>(ShaderStage::Fragment & shader.Stages))
				{
					std::cout << "[GLSL] Fragment Shader Couldn't find \"main\" function!" << '\n';
					return false;
				}
				break;
					
			case 5:
				if (static_cast<uint32_t>(ShaderStage::Compute & shader.Stages))
				{
					std::cout << "[GLSL] Compute Shader Couldn't find \"main\" function!" << '\n';
					return false;
				}
				break;
					
			default:
				break;
			}
		}

		if (!shader.SubShaderSources[i].Source.empty())
		{
			//Found main function
			//Add GLSL version before any shader code
			shader.SubShaderSources[i].Source = "#version 460 core\n" + shader.SubShaderSources[i].Source;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ValidateShaderStages(const Shader& shader)
{
	const ShaderStage& stages = shader.Stages;

	//Check if any Shader Stage is set
	if(ShaderStage::None == stages)
	{
		std::cout << "[GLSL] No Shader Stage found!" << '\n';
		return false;
	}
	
	//Check for "Normal" Shader Stages combined with Compute
	if ((static_cast<uint32_t>(ShaderStage::Vertex & stages) ||
		static_cast<uint32_t>(ShaderStage::Fragment & stages) ||
		static_cast<uint32_t>(ShaderStage::TessellationControl & stages) ||
		static_cast<uint32_t>(ShaderStage::TessellationEvaluation & stages) ||
		static_cast<uint32_t>(ShaderStage::Geometry & stages)) &&
		static_cast<uint32_t>(ShaderStage::Compute & stages))
	{
		std::cout << "[GLSL] Rasterizer Shader Stages combined with Compute stage!" << '\n';
		return false;
	}

	//Check for "Normal" Shader Stages combined with RayTracing
	if ((static_cast<uint32_t>(ShaderStage::Vertex & stages) ||
		static_cast<uint32_t>(ShaderStage::Fragment & stages) ||
		static_cast<uint32_t>(ShaderStage::TessellationControl & stages) ||
		static_cast<uint32_t>(ShaderStage::TessellationEvaluation & stages) ||
		static_cast<uint32_t>(ShaderStage::Geometry & stages)) &&
		static_cast<uint32_t>(ShaderStage::RayTracing & stages))
	{
		std::cout << "[GLSL] Rasterizer Shader Stages combined with RayTracing stage!" << '\n';
		return false;
	}

	//Check for Compute Shader Stage combined with RayTracing
	if (static_cast<uint32_t>(ShaderStage::Compute & stages) &&
		static_cast<uint32_t>(ShaderStage::RayTracing & stages))
	{
		std::cout << "[GLSL] Compute Shader Stage combined with RayTracing stage!" << '\n';
		return false;
	}

	//Check for Vertex Shader Stage & required Fragment/Pixel Shader Stage
	if (static_cast<uint32_t>(ShaderStage::Vertex & stages) &&
		!(static_cast<uint32_t>(ShaderStage::Fragment & stages)))
	{
		std::cout << "[GLSL] Only Vertex Shader Stage provided! Missing Fragment/Pixel Shader Stage" << '\n';
		return false;
	}

	//Shader Stages should be valid
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool CompileGLSLToSPIRV(Shader& shader)
{
	if (!s_glslangInitialized)
	{
		if (!glslang::InitializeProcess())
		{
			std::cout << "[GLSL] GLSLang initialization failed!" << '\n';
			return false;
		}
		s_glslangInitialized = true;
	}

	std::array<std::unique_ptr<glslang::TShader>, static_cast<uint32_t>(ShaderStage::SHADER_STAGE_COUNT)> glslShaders{};
	for (uint32_t i = 0; i < shader.SubShaderSources.size(); ++i)
	{
		if (!shader.SubShaderSources[i].Source.empty())
		{
			glslang::TProgram program;

			std::string preProcessedSource;

			switch (i)
			{
			case 0:
			{
				std::cout << "[GLSL] Pre-Processing Vertex Shader" << '\n';
				glslShaders[i] = PreProcessGLSLForConversion(shader.SubShaderSources[i].Source.c_str(), ShaderStage::Vertex, preProcessedSource);
				if (preProcessedSource.empty())
					return false;

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[i]->setStrings(&preProcessedCStr, 1);

				std::cout << "[GLSL] Parsing Vertex Shader" << '\n';
				if (!ParseGLSL(glslShaders[i].get()))
					return false;

				std::cout << "[GLSL] Linking Vertex Shader" << '\n';
				if (!LinkGLSL(glslShaders[i].get(), program))
					return false;

				std::cout << "[SPIRV] Converting GLSL -> SPIR-V" << '\n';
				shader.SubShaderSources[i].SPIRV = ConvertToSPIRV(glslShaders[i].get(), ShaderStage::Vertex, program);
				break;
			}

			case 1:
			{
				std::cout << "[GLSL] Pre-Processing TessellationControl Shader" << '\n';
				glslShaders[i] = PreProcessGLSLForConversion(shader.SubShaderSources[i].Source.c_str(), ShaderStage::TessellationControl, preProcessedSource);
				if (preProcessedSource.empty())
					return false;

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[i]->setStrings(&preProcessedCStr, 1);

				std::cout << "[GLSL] Parsing TessellationControl Shader" << '\n';
				if (!ParseGLSL(glslShaders[i].get()))
					return false;

				std::cout << "[GLSL] Linking TessellationControl Shader" << '\n';
				if (!LinkGLSL(glslShaders[i].get(), program))
					return false;

				std::cout << "[SPIRV] Converting GLSL -> SPIR-V" << '\n';
				shader.SubShaderSources[i].SPIRV = ConvertToSPIRV(glslShaders[i].get(), ShaderStage::TessellationControl, program);
				break;
			}

			case 2:
			{
				std::cout << "[GLSL] Pre-Processing TessellationEvaluation Shader" << '\n';
				glslShaders[i] = PreProcessGLSLForConversion(shader.SubShaderSources[i].Source.c_str(), ShaderStage::TessellationEvaluation, preProcessedSource);
				if (preProcessedSource.empty())
					return false;

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[i]->setStrings(&preProcessedCStr, 1);

				std::cout << "[GLSL] Parsing TessellationEvaluation Shader" << '\n';
				if (!ParseGLSL(glslShaders[i].get()))
					return false;

				std::cout << "[GLSL] Linking TessellationEvaluation Shader" << '\n';
				if (!LinkGLSL(glslShaders[i].get(), program))
					return false;

				std::cout << "[SPIRV] Converting GLSL -> SPIR-V" << '\n';
				shader.SubShaderSources[i].SPIRV = ConvertToSPIRV(glslShaders[i].get(), ShaderStage::TessellationEvaluation, program);
				break;
			}

			case 3:
			{
				std::cout << "[GLSL] Pre-Processing Geometry Shader" << '\n';
				glslShaders[i] = PreProcessGLSLForConversion(shader.SubShaderSources[i].Source.c_str(), ShaderStage::Geometry, preProcessedSource);
				if (preProcessedSource.empty())
					return false;

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[i]->setStrings(&preProcessedCStr, 1);

				std::cout << "[GLSL] Parsing Geometry Shader" << '\n';
				if (!ParseGLSL(glslShaders[i].get()))
					return false;

				std::cout << "[GLSL] Linking Geometry Shader" << '\n';
				if (!LinkGLSL(glslShaders[i].get(), program))
					return false;

				std::cout << "[SPIRV] Converting GLSL -> SPIR-V" << '\n';
				shader.SubShaderSources[i].SPIRV = ConvertToSPIRV(glslShaders[i].get(), ShaderStage::Geometry, program);
				break;
			}

			case 4:
			{
				std::cout << "[GLSL] Pre-Processing Fragment Shader" << '\n';
				glslShaders[i] = PreProcessGLSLForConversion(shader.SubShaderSources[i].Source.c_str(), ShaderStage::Fragment, preProcessedSource);
				if (preProcessedSource.empty())
					return false;

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[i]->setStrings(&preProcessedCStr, 1);

				std::cout << "[GLSL] Parsing Fragment Shader" << '\n';
				if (!ParseGLSL(glslShaders[i].get()))
					return false;

				std::cout << "[GLSL] Linking Fragment Shader" << '\n';
				if (!LinkGLSL(glslShaders[i].get(), program))
					return false;

				std::cout << "[SPIRV] Converting GLSL -> SPIR-V" << '\n';
				shader.SubShaderSources[i].SPIRV = ConvertToSPIRV(glslShaders[i].get(), ShaderStage::Fragment, program);
				break;
			}

			case 5:
			{
				std::cout << "[GLSL] Pre-Processing Compute Shader" << '\n';
				glslShaders[i] = PreProcessGLSLForConversion(shader.SubShaderSources[i].Source.c_str(), ShaderStage::Compute, preProcessedSource);
				if (preProcessedSource.empty())
					return false;

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[i]->setStrings(&preProcessedCStr, 1);

				std::cout << "[GLSL] Parsing Compute Shader" << '\n';
				if (!ParseGLSL(glslShaders[i].get()))
					return false;

				std::cout << "[GLSL] Linking Compute Shader" << '\n';
				if (!LinkGLSL(glslShaders[i].get(), program))
					return false;

				std::cout << "[SPIRV] Converting GLSL -> SPIR-V" << '\n';
				shader.SubShaderSources[i].SPIRV = ConvertToSPIRV(glslShaders[i].get(), ShaderStage::Compute, program);
				break;
			}

			//TODO

			default:
				break;
			}
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<glslang::TShader> PreProcessGLSLForConversion(const char* source, const ShaderStage stage, std::string& preProcessedSource)
{
	std::unique_ptr<glslang::TShader> shader = nullptr;
	
	if (stage == ShaderStage::Vertex)
	{
		shader = std::make_unique<glslang::TShader>(EShLangVertex);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangVertex, glslang::EShClientVulkan, 460);
	}
	else if (stage == ShaderStage::TessellationControl)
	{
		shader = std::make_unique<glslang::TShader>(EShLangTessControl);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangTessControl, glslang::EShClientVulkan, 460);
	}
	else if (stage == ShaderStage::TessellationEvaluation)
	{
		shader = std::make_unique<glslang::TShader>(EShLangTessEvaluation);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangTessEvaluation, glslang::EShClientVulkan, 460);
	}
	else if (stage == ShaderStage::Geometry)
	{
		shader = std::make_unique<glslang::TShader>(EShLangGeometry);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangGeometry, glslang::EShClientVulkan, 460);
	}
	else if (stage == ShaderStage::Fragment)
	{
		shader = std::make_unique<glslang::TShader>(EShLangFragment);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangFragment, glslang::EShClientVulkan, 460);
	}
	else if (stage == ShaderStage::Compute)
	{
		shader = std::make_unique<glslang::TShader>(EShLangCompute);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangCompute, glslang::EShClientVulkan, 460);
	}
	//TODO RayTracing

	shader->setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
	shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);
	glslang::TShader::ForbidIncluder includer;
	constexpr TBuiltInResource DefaultTBuiltInResource = 
	{
		/* .MaxLights = */ 32,
		/* .MaxClipPlanes = */ 6,
		/* .MaxTextureUnits = */ 32,
		/* .MaxTextureCoords = */ 32,
		/* .MaxVertexAttribs = */ 64,
		/* .MaxVertexUniformComponents = */ 4096,
		/* .MaxVaryingFloats = */ 64,
		/* .MaxVertexTextureImageUnits = */ 32,
		/* .MaxCombinedTextureImageUnits = */ 80,
		/* .MaxTextureImageUnits = */ 32,
		/* .MaxFragmentUniformComponents = */ 4096,
		/* .MaxDrawBuffers = */ 32,
		/* .MaxVertexUniformVectors = */ 128,
		/* .MaxVaryingVectors = */ 8,
		/* .MaxFragmentUniformVectors = */ 16,
		/* .MaxVertexOutputVectors = */ 16,
		/* .MaxFragmentInputVectors = */ 15,
		/* .MinProgramTexelOffset = */ -8,
		/* .MaxProgramTexelOffset = */ 7,
		/* .MaxClipDistances = */ 8,
		/* .MaxComputeWorkGroupCountX = */ 65535,
		/* .MaxComputeWorkGroupCountY = */ 65535,
		/* .MaxComputeWorkGroupCountZ = */ 65535,
		/* .MaxComputeWorkGroupSizeX = */ 1024,
		/* .MaxComputeWorkGroupSizeY = */ 1024,
		/* .MaxComputeWorkGroupSizeZ = */ 64,
		/* .MaxComputeUniformComponents = */ 1024,
		/* .MaxComputeTextureImageUnits = */ 16,
		/* .MaxComputeImageUniforms = */ 8,
		/* .MaxComputeAtomicCounters = */ 8,
		/* .MaxComputeAtomicCounterBuffers = */ 1,
		/* .MaxVaryingComponents = */ 60,
		/* .MaxVertexOutputComponents = */ 64,
		/* .MaxGeometryInputComponents = */ 64,
		/* .MaxGeometryOutputComponents = */ 128,
		/* .MaxFragmentInputComponents = */ 128,
		/* .MaxImageUnits = */ 8,
		/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
		/* .MaxCombinedShaderOutputResources = */ 8,
		/* .MaxImageSamples = */ 0,
		/* .MaxVertexImageUniforms = */ 0,
		/* .MaxTessControlImageUniforms = */ 0,
		/* .MaxTessEvaluationImageUniforms = */ 0,
		/* .MaxGeometryImageUniforms = */ 0,
		/* .MaxFragmentImageUniforms = */ 8,
		/* .MaxCombinedImageUniforms = */ 8,
		/* .MaxGeometryTextureImageUnits = */ 16,
		/* .MaxGeometryOutputVertices = */ 256,
		/* .MaxGeometryTotalOutputComponents = */ 1024,
		/* .MaxGeometryUniformComponents = */ 1024,
		/* .MaxGeometryVaryingComponents = */ 64,
		/* .MaxTessControlInputComponents = */ 128,
		/* .MaxTessControlOutputComponents = */ 128,
		/* .MaxTessControlTextureImageUnits = */ 16,
		/* .MaxTessControlUniformComponents = */ 1024,
		/* .MaxTessControlTotalOutputComponents = */ 4096,
		/* .MaxTessEvaluationInputComponents = */ 128,
		/* .MaxTessEvaluationOutputComponents = */ 128,
		/* .MaxTessEvaluationTextureImageUnits = */ 16,
		/* .MaxTessEvaluationUniformComponents = */ 1024,
		/* .MaxTessPatchComponents = */ 120,
		/* .MaxPatchVertices = */ 32,
		/* .MaxTessGenLevel = */ 64,
		/* .MaxViewports = */ 16,
		/* .MaxVertexAtomicCounters = */ 0,
		/* .MaxTessControlAtomicCounters = */ 0,
		/* .MaxTessEvaluationAtomicCounters = */ 0,
		/* .MaxGeometryAtomicCounters = */ 0,
		/* .MaxFragmentAtomicCounters = */ 8,
		/* .MaxCombinedAtomicCounters = */ 8,
		/* .MaxAtomicCounterBindings = */ 1,
		/* .MaxVertexAtomicCounterBuffers = */ 0,
		/* .MaxTessControlAtomicCounterBuffers = */ 0,
		/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
		/* .MaxGeometryAtomicCounterBuffers = */ 0,
		/* .MaxFragmentAtomicCounterBuffers = */ 1,
		/* .MaxCombinedAtomicCounterBuffers = */ 1,
		/* .MaxAtomicCounterBufferSize = */ 16384,
		/* .MaxTransformFeedbackBuffers = */ 4,
		/* .MaxTransformFeedbackInterleavedComponents = */ 64,
		/* .MaxCullDistances = */ 8,
		/* .MaxCombinedClipAndCullDistances = */ 8,
		/* .MaxSamples = */ 4,
		/* .maxMeshOutputVerticesNV = */ 256,
		/* .maxMeshOutputPrimitivesNV = */ 512,
		/* .maxMeshWorkGroupSizeX_NV = */ 32,
		/* .maxMeshWorkGroupSizeY_NV = */ 1,
		/* .maxMeshWorkGroupSizeZ_NV = */ 1,
		/* .maxTaskWorkGroupSizeX_NV = */ 32,
		/* .maxTaskWorkGroupSizeY_NV = */ 1,
		/* .maxTaskWorkGroupSizeZ_NV = */ 1,
		/* .maxMeshViewCountNV = */ 4,

		/* TLimits */
		/* .nonInductiveForLoops = */ true,
		/* .whileLoops = */ true,
		/* .doWhileLoops = */ true,
		/* .generalUniformIndexing = */ true,
		/* .generalAttributeMatrixVectorIndexing = */ true,
		/* .generalVaryingIndexing = */ true,
		/* .generalSamplerIndexing = */ true,
		/* .generalVariableIndexing = */ true,
		/* .generalConstantMatrixVectorIndexing = */ true
	};
	if (!shader->preprocess(&DefaultTBuiltInResource,
		460,
		ECoreProfile,
		true,
		true,
		static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules),
		&preProcessedSource,
		includer))
	{
		std::cout << "[GLSL] GLSL -> SPIR-V Preprocessing failed!" << '\n';
		std::cout << "[GLSL] " << shader->getInfoLog() << '\n';
		std::cout << "[GLSL] " << shader->getInfoDebugLog() << '\n';

		return nullptr;
	}

	return shader;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ParseGLSL(glslang::TShader* shader)
{
	constexpr TBuiltInResource DefaultTBuiltInResource = 
	{
		/* .MaxLights = */ 32,
		/* .MaxClipPlanes = */ 6,
		/* .MaxTextureUnits = */ 32,
		/* .MaxTextureCoords = */ 32,
		/* .MaxVertexAttribs = */ 64,
		/* .MaxVertexUniformComponents = */ 4096,
		/* .MaxVaryingFloats = */ 64,
		/* .MaxVertexTextureImageUnits = */ 32,
		/* .MaxCombinedTextureImageUnits = */ 80,
		/* .MaxTextureImageUnits = */ 32,
		/* .MaxFragmentUniformComponents = */ 4096,
		/* .MaxDrawBuffers = */ 32,
		/* .MaxVertexUniformVectors = */ 128,
		/* .MaxVaryingVectors = */ 8,
		/* .MaxFragmentUniformVectors = */ 16,
		/* .MaxVertexOutputVectors = */ 16,
		/* .MaxFragmentInputVectors = */ 15,
		/* .MinProgramTexelOffset = */ -8,
		/* .MaxProgramTexelOffset = */ 7,
		/* .MaxClipDistances = */ 8,
		/* .MaxComputeWorkGroupCountX = */ 65535,
		/* .MaxComputeWorkGroupCountY = */ 65535,
		/* .MaxComputeWorkGroupCountZ = */ 65535,
		/* .MaxComputeWorkGroupSizeX = */ 1024,
		/* .MaxComputeWorkGroupSizeY = */ 1024,
		/* .MaxComputeWorkGroupSizeZ = */ 64,
		/* .MaxComputeUniformComponents = */ 1024,
		/* .MaxComputeTextureImageUnits = */ 16,
		/* .MaxComputeImageUniforms = */ 8,
		/* .MaxComputeAtomicCounters = */ 8,
		/* .MaxComputeAtomicCounterBuffers = */ 1,
		/* .MaxVaryingComponents = */ 60,
		/* .MaxVertexOutputComponents = */ 64,
		/* .MaxGeometryInputComponents = */ 64,
		/* .MaxGeometryOutputComponents = */ 128,
		/* .MaxFragmentInputComponents = */ 128,
		/* .MaxImageUnits = */ 8,
		/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
		/* .MaxCombinedShaderOutputResources = */ 8,
		/* .MaxImageSamples = */ 0,
		/* .MaxVertexImageUniforms = */ 0,
		/* .MaxTessControlImageUniforms = */ 0,
		/* .MaxTessEvaluationImageUniforms = */ 0,
		/* .MaxGeometryImageUniforms = */ 0,
		/* .MaxFragmentImageUniforms = */ 8,
		/* .MaxCombinedImageUniforms = */ 8,
		/* .MaxGeometryTextureImageUnits = */ 16,
		/* .MaxGeometryOutputVertices = */ 256,
		/* .MaxGeometryTotalOutputComponents = */ 1024,
		/* .MaxGeometryUniformComponents = */ 1024,
		/* .MaxGeometryVaryingComponents = */ 64,
		/* .MaxTessControlInputComponents = */ 128,
		/* .MaxTessControlOutputComponents = */ 128,
		/* .MaxTessControlTextureImageUnits = */ 16,
		/* .MaxTessControlUniformComponents = */ 1024,
		/* .MaxTessControlTotalOutputComponents = */ 4096,
		/* .MaxTessEvaluationInputComponents = */ 128,
		/* .MaxTessEvaluationOutputComponents = */ 128,
		/* .MaxTessEvaluationTextureImageUnits = */ 16,
		/* .MaxTessEvaluationUniformComponents = */ 1024,
		/* .MaxTessPatchComponents = */ 120,
		/* .MaxPatchVertices = */ 32,
		/* .MaxTessGenLevel = */ 64,
		/* .MaxViewports = */ 16,
		/* .MaxVertexAtomicCounters = */ 0,
		/* .MaxTessControlAtomicCounters = */ 0,
		/* .MaxTessEvaluationAtomicCounters = */ 0,
		/* .MaxGeometryAtomicCounters = */ 0,
		/* .MaxFragmentAtomicCounters = */ 8,
		/* .MaxCombinedAtomicCounters = */ 8,
		/* .MaxAtomicCounterBindings = */ 1,
		/* .MaxVertexAtomicCounterBuffers = */ 0,
		/* .MaxTessControlAtomicCounterBuffers = */ 0,
		/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
		/* .MaxGeometryAtomicCounterBuffers = */ 0,
		/* .MaxFragmentAtomicCounterBuffers = */ 1,
		/* .MaxCombinedAtomicCounterBuffers = */ 1,
		/* .MaxAtomicCounterBufferSize = */ 16384,
		/* .MaxTransformFeedbackBuffers = */ 4,
		/* .MaxTransformFeedbackInterleavedComponents = */ 64,
		/* .MaxCullDistances = */ 8,
		/* .MaxCombinedClipAndCullDistances = */ 8,
		/* .MaxSamples = */ 4,
		/* .maxMeshOutputVerticesNV = */ 256,
		/* .maxMeshOutputPrimitivesNV = */ 512,
		/* .maxMeshWorkGroupSizeX_NV = */ 32,
		/* .maxMeshWorkGroupSizeY_NV = */ 1,
		/* .maxMeshWorkGroupSizeZ_NV = */ 1,
		/* .maxTaskWorkGroupSizeX_NV = */ 32,
		/* .maxTaskWorkGroupSizeY_NV = */ 1,
		/* .maxTaskWorkGroupSizeZ_NV = */ 1,
		/* .maxMeshViewCountNV = */ 4,

		/* TLimits */
		/* .nonInductiveForLoops = */ true,
		/* .whileLoops = */ true,
		/* .doWhileLoops = */ true,
		/* .generalUniformIndexing = */ true,
		/* .generalAttributeMatrixVectorIndexing = */ true,
		/* .generalVaryingIndexing = */ true,
		/* .generalSamplerIndexing = */ true,
		/* .generalVariableIndexing = */ true,
		/* .generalConstantMatrixVectorIndexing = */ true
	};
	
	if (!shader->parse(&DefaultTBuiltInResource, 460, true, static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		std::cout << "[GLSL] Parsing failed: " << '\n';
		std::cout << "[GLSL] " << shader->getInfoLog() << '\n';
		std::cout << "[GLSL] " << shader->getInfoDebugLog() << '\n';

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool LinkGLSL(glslang::TShader* shader, glslang::TProgram& program)
{
	if (shader)
		program.addShader(shader);

	if (!program.link(static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		std::cout << "[GLSL] Linking failed: " << '\n';
		std::cout << "[GLSL] " << program.getInfoLog() << '\n';
		std::cout << "[GLSL] " << program.getInfoDebugLog() << '\n';

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint32_t> ConvertToSPIRV(glslang::TShader* shader, const ShaderStage stage, glslang::TProgram& program)
{
	std::vector<uint32_t> SPIRV{};

	if (shader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
		spvOptions.disableOptimizer = false;
		spvOptions.optimizeSize = true;

		switch(stage)
		{
		case ShaderStage::Vertex:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangVertex), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				std::cout << "[SPIRV] Vertex Shader: " << logger.getAllMessages() << '\n';
			break;

		case ShaderStage::TessellationControl:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangTessControl), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				std::cout << "[SPIRV] Vertex Shader: " << logger.getAllMessages() << '\n';
			break;

		case ShaderStage::TessellationEvaluation:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangTessEvaluation), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				std::cout << "[SPIRV] Vertex Shader: " << logger.getAllMessages() << '\n';
			break;

		case ShaderStage::Geometry:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangGeometry), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				std::cout << "[SPIRV] Vertex Shader: " << logger.getAllMessages() << '\n';
			break;

		case ShaderStage::Fragment:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangFragment), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				std::cout << "[SPIRV] Vertex Shader: " << logger.getAllMessages() << '\n';
			break;

		case ShaderStage::Compute:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangCompute), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				std::cout << "[SPIRV] Vertex Shader: " << logger.getAllMessages() << '\n';
			break;

			//TODO RayTracing
			
		default:
			break;
		}
	}

	return SPIRV;
}

//-------------------------------------------------------------------------------------------------------------------//

void SaveSPIRV(Shader& shader)
{
	const std::string filepath = shader.FilePath + ".spirv";

	std::cout << "[SPIRV] Saving Shader: \"" << filepath << "\"" << '\n';
	
	std::ofstream file(filepath, std::ios::binary);
	if (file.is_open())
	{
		uint32_t SPIRVSubShadersCount = 0;
		for(const auto& subShader : shader.SubShaderSources)
		{
			if (!subShader.SPIRV.empty())
				++SPIRVSubShadersCount;
		}
		file.write(reinterpret_cast<char*>(&SPIRVSubShadersCount), sizeof(uint32_t));
		
		for(uint32_t i = 0; i < shader.SubShaderSources.size(); ++i)
		{
			if(!shader.SubShaderSources[i].SPIRV.empty())
			{
				uint32_t SPIRVSize = static_cast<uint32_t>(shader.SubShaderSources[i].SPIRV.size());
				uint32_t type = -1;
				switch(i)
				{
				case 0:
					type = static_cast<uint32_t>(ShaderStage::Vertex);
					break;

				case 1:
					type = static_cast<uint32_t>(ShaderStage::TessellationControl);
					break;

				case 2:
					type = static_cast<uint32_t>(ShaderStage::TessellationEvaluation);
					break;

				case 3:
					type = static_cast<uint32_t>(ShaderStage::Geometry);
					break;

				case 4:
					type = static_cast<uint32_t>(ShaderStage::Fragment);
					break;

				case 5:
					type = static_cast<uint32_t>(ShaderStage::Compute);
					break;

				//TODO RayTracing
					
				default:
					break;
				}

				file.write(reinterpret_cast<char*>(&SPIRVSize), sizeof(uint32_t));
				file.write(reinterpret_cast<char*>(&type), sizeof(uint32_t));
				file.write(reinterpret_cast<char*>(shader.SubShaderSources[i].SPIRV.data()), shader.SubShaderSources[i].SPIRV.size() * sizeof(uint32_t));
			}
		}
		
		file.close();
	}
	else
		std::cout << "Couldn't open file: " << filepath << '\n';	
}