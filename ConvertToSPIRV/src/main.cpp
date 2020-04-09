#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <array>

#include <glslang/Public/ShaderLang.h>
#include <GlslangToSpv.h>
#include <Logger.h>

enum class ShaderType
{
	Unknown = 0,
	Vertex = 1,
	Fragment = 2,
	Geometry = 3,
	Tessellation_Control = 4,
	Tessellation_Evaluation = 5,
	Compute = 6
};

struct SubShader
{
	std::string Source;
	ShaderType Type;
	std::vector<uint32_t> SPIRV{};
};
struct Shader
{
	std::string FilePath;
	std::string Source;
	std::array<SubShader, 6> SubShaderSources{};
};

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
std::array<SubShader, 6> PreProcessGLSL(Shader& shader);
bool CompileGLSLToSPIRV(Shader& shader);
std::unique_ptr<glslang::TShader> PreProcessGLSLForSPIRV(const char* source, ShaderType shaderType, std::string& preProcessedSource);
bool ParseGLSL(glslang::TShader* shader);
bool LinkGLSL(glslang::TShader* VShader,
	      glslang::TShader* FShader,
	      glslang::TShader* GShader,
	      glslang::TShader* TCShader,
	      glslang::TShader* TEShader,
	      glslang::TShader* CShader,
	      glslang::TProgram& program);
std::vector<std::vector<uint32_t>> ConvertToSPIRV(glslang::TShader* VShader,
	                                              glslang::TShader* FShader,
	                                              glslang::TShader* GShader,
	                                              glslang::TShader* TCShader,
	                                              glslang::TShader* TEShader,
	                                              glslang::TShader* CShader,
	                                              glslang::TProgram& program);
void SaveSPIRV(Shader& shader);

//-------------------------------------------------------------------------------------------------------------------//

int main(const int argc, char* argv[])
{
	if (CheckForParameters(argc))
	{
		std::vector<Shader> shaders = LoadShaderSources(argc, argv);

		for(auto& shader : shaders)
		{
			shader.SubShaderSources = PreProcessGLSL(shader);
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
			if (line.back() == '\r')
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
	while ((t = strstr(t, token.c_str())))
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
		std::cout << "Usage: Drag and Drop a file or a folder on the .exe file to convert TRAP GLSL or HLSL Shaders to SPIRV" << '\n';

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

std::array<SubShader, 6> PreProcessGLSL(Shader& shader)
{
	std::array<SubShader, 6> shaderSources{};
	
	std::vector<std::string> lines = GetLines(shader.Source);
	std::array<std::string, 6> GLSLShaderSources{};
	ShaderType type = ShaderType::Unknown;

	//Get Shader Type
	for (uint32_t i = 1; i < lines.size(); i++)
	{
		if (StartsWith(ToLower(lines[i]), "#shader"))
		{
			if (FindToken(ToLower(lines[i]), "vertex"))
				type = ShaderType::Vertex;
			else if (FindToken(ToLower(lines[i]), "fragment"))
				type = ShaderType::Fragment;
			else if (FindToken(ToLower(lines[i]), "geometry"))
				type = ShaderType::Geometry;
			else if (FindToken(ToLower(lines[i]), "tessellation"))
			{
				if (FindToken(ToLower(lines[i]), "control"))
					type = ShaderType::Tessellation_Control;
				else if (FindToken(ToLower(lines[i]), "evaluation"))
					type = ShaderType::Tessellation_Evaluation;
			}
			else if (FindToken(ToLower(lines[i]), "compute"))
				type = ShaderType::Compute;
		}
		else if (StartsWith(ToLower(lines[i]), "#version"))
			std::cout << "[GLSL] Found Tag: \"" << lines[i] << "\" this is unnecessary! Skipping Line: " << i;
		else if (type != ShaderType::Unknown)
		{
			GLSLShaderSources[static_cast<int32_t>(type) - 1].append(lines[i]);
			GLSLShaderSources[static_cast<int32_t>(type) - 1].append("\n");
		}
	}

	for (uint32_t i = 0; i < GLSLShaderSources.size(); i++)
	{
		if (!GLSLShaderSources[i].empty())
		{
			if (ToLower(GLSLShaderSources[i]).find("main") == std::string::npos)
			{
				switch(static_cast<ShaderType>(i + 1))
				{
				case ShaderType::Vertex:
					std::cout << "[GLSL] Vertex Shader: Couldn't find \"main\" function!" << '\n';
					break;

				case ShaderType::Fragment:
					std::cout << "[GLSL] Fragment Shader: Couldn't find \"main\" function!" << '\n';
					break;

				case ShaderType::Geometry:
					std::cout << "[GLSL] Geometry Shader: Couldn't find \"main\" function!" << '\n';
					break;

				case ShaderType::Tessellation_Control:
					std::cout << "[GLSL] Tessellation Control Shader: Couldn't find \"main\" function!" << '\n';
					break;

				case ShaderType::Tessellation_Evaluation:
					std::cout << "[GLSL] Tessellation Evaluation Shader: Couldn't find \"main\" function!" << '\n';
					break;

				case ShaderType::Compute:
					std::cout << "[GLSL] Compute Shader: Couldn't find \"main\" function!" << '\n';
					break;
					
				default:
					break;
				}
				
				GLSLShaderSources[i] = "";
			}
			else //Found main function
				GLSLShaderSources[i] = "#version 460 core\n" + GLSLShaderSources[i];
		}
	}

	if (!GLSLShaderSources[0].empty())
	{
		std::cout << "[GLSL] Adding Vertex Shader to \"" << shader.FilePath << "\"" << '\n';
		shaderSources[0] = SubShader{GLSLShaderSources[0], ShaderType::Vertex};
	}
	if (!GLSLShaderSources[1].empty())
	{
		std::cout << "[GLSL] Adding Fragment Shader to \"" << shader.FilePath << "\"" << '\n';
		shaderSources[1] = SubShader{ GLSLShaderSources[1], ShaderType::Fragment };
	}
	if (!GLSLShaderSources[2].empty())
	{
		std::cout << "[GLSL] Adding Geometry Shader to \"" << shader.FilePath << "\"" << '\n';
		shaderSources[2] = SubShader{ GLSLShaderSources[2], ShaderType::Geometry };
	}
	if (!GLSLShaderSources[3].empty())
	{
		std::cout << "[GLSL] Adding Tessellation Control Shader to \"" << shader.FilePath << "\"" << '\n';
		shaderSources[3] = SubShader{ GLSLShaderSources[3], ShaderType::Tessellation_Control };
	}
	if (!GLSLShaderSources[4].empty())
	{
		std::cout << "[GLSL] Adding Tessellation Evaluation Shader to \"" << shader.FilePath << "\"" << '\n';
		shaderSources[4] = SubShader{ GLSLShaderSources[4], ShaderType::Tessellation_Evaluation };
	}
	if (!GLSLShaderSources[5].empty())
	{
		std::cout << "[GLSL] Adding Compute Shader to \"" << shader.FilePath << "\"" << '\n';
		shaderSources[5] = SubShader{ GLSLShaderSources[5], ShaderType::Compute };
	}

	return shaderSources;
}

//-------------------------------------------------------------------------------------------------------------------//

bool CompileGLSLToSPIRV(Shader& shader)
{
	if (!s_glslangInitialized)
	{
		glslang::InitializeProcess();
		s_glslangInitialized = true;
	}

	glslang::TProgram program;
	std::unique_ptr<glslang::TShader> VShader = nullptr;
	std::unique_ptr<glslang::TShader> FShader = nullptr;
	std::unique_ptr<glslang::TShader> GShader = nullptr;
	std::unique_ptr<glslang::TShader> TCShader = nullptr;
	std::unique_ptr<glslang::TShader> TEShader = nullptr;
	std::unique_ptr<glslang::TShader> CShader = nullptr;

	if (!shader.SubShaderSources[0].Source.empty())
	{
		const char* VSSource = shader.SubShaderSources[0].Source.c_str();
		std::cout << "[GLSL] Pre-Processing Vertex Shader" << '\n';
		std::string preProcessedSource;
		VShader = PreProcessGLSLForSPIRV(VSSource, ShaderType::Vertex, preProcessedSource);
		if (preProcessedSource.empty() || !VShader)
			return false;
		const char* preProcessedCStr = preProcessedSource.c_str();
		VShader->setStrings(&preProcessedCStr, 1);

		std::cout << "[GLSL] Parsing Vertex Shader" << '\n';
		if (!ParseGLSL(VShader.get()))
			return false;
	}

	if (!shader.SubShaderSources[1].Source.empty())
	{
		const char* FSSource = shader.SubShaderSources[1].Source.c_str();
		std::cout << "[GLSL] Pre-Processing Fragment Shader" << '\n';
		std::string preProcessedSource;
		FShader = PreProcessGLSLForSPIRV(FSSource, ShaderType::Fragment, preProcessedSource);
		if (preProcessedSource.empty())
			return false;
		const char* preProcessedCStr = preProcessedSource.c_str();
		FShader->setStrings(&preProcessedCStr, 1);

		std::cout << "[GLSL] Parsing Fragment Shader" << '\n';
		if (!ParseGLSL(FShader.get()))
			return false;
	}

	if (!shader.SubShaderSources[2].Source.empty())
	{
		const char* GSSource = shader.SubShaderSources[2].Source.c_str();
		std::cout << "[GLSL] Pre-Processing Geometry Shader" << '\n';
		std::string preProcessedSource;
		GShader = PreProcessGLSLForSPIRV(GSSource, ShaderType::Geometry, preProcessedSource);
		if (preProcessedSource.empty())
			return false;
		const char* preProcessedCStr = preProcessedSource.c_str();
		GShader->setStrings(&preProcessedCStr, 1);

		std::cout << "[GLSL] Parsing Geometry Shader" << '\n';
		if (!ParseGLSL(GShader.get()))
			return false;
	};

	if (!shader.SubShaderSources[3].Source.empty())
	{
		const char* TCSSource = shader.SubShaderSources[3].Source.c_str();
		std::cout << "[GLSL] Pre-Processing Tessellation Control Shader" << '\n';
		std::string preProcessedSource;
		TCShader = PreProcessGLSLForSPIRV(TCSSource, ShaderType::Tessellation_Control, preProcessedSource);
		if (preProcessedSource.empty())
			return false;
		const char* preProcessedCStr = preProcessedSource.c_str();
		TCShader->setStrings(&preProcessedCStr, 1);

		std::cout << "[GLSL] Parsing Tessellation Control Shader" << '\n';
		if (!ParseGLSL(TCShader.get()))
			return false;
	}

	if (!shader.SubShaderSources[4].Source.empty())
	{
		const char* TESSource = shader.SubShaderSources[4].Source.c_str();
		std::cout << "[GLSL] Pre-Processing Tessellation Evaluation Shader" << '\n';
		std::string preProcessedSource;
		TEShader = PreProcessGLSLForSPIRV(TESSource, ShaderType::Tessellation_Evaluation, preProcessedSource);
		if (preProcessedSource.empty())
			return false;
		const char* preProcessedCStr = preProcessedSource.c_str();
		TEShader->setStrings(&preProcessedCStr, 1);

		std::cout << "[GLSL] Parsing Tessellation Evaluation Shader" << '\n';
		if (!ParseGLSL(TEShader.get()))
			return false;
	}

	if (!shader.SubShaderSources[5].Source.empty())
	{
		const char* CSSource = shader.SubShaderSources[5].Source.c_str();
		std::cout << "[GLSL] Pre-Processing Compute Shader" << '\n';
		std::string preProcessedSource;
		CShader = PreProcessGLSLForSPIRV(CSSource, ShaderType::Compute, preProcessedSource);
		if (preProcessedSource.empty())
			return false;
		const char* preProcessedCStr = preProcessedSource.c_str();
		CShader->setStrings(&preProcessedCStr, 1);

		std::cout << "[GLSL] Parsing Compute Shader" << '\n';
		if (!ParseGLSL(CShader.get()))
			return false;
	}

	std::cout << "[GLSL] Linking Shaders" << '\n';
	if (!LinkGLSL(VShader.get(), FShader.get(), GShader.get(), TCShader.get(), TEShader.get(), CShader.get(), program))
		return false;

	std::cout << "[SPIRV] Converting Shader to SPIRV" << '\n';
	std::vector<std::vector<uint32_t>> SPIRV = ConvertToSPIRV(VShader.get(), FShader.get(), GShader.get(), TCShader.get(), TEShader.get(), CShader.get(), program);

	if (!shader.SubShaderSources[0].Source.empty())
		shader.SubShaderSources[0].SPIRV = SPIRV[0];
	if (!shader.SubShaderSources[1].Source.empty())
		shader.SubShaderSources[1].SPIRV = SPIRV[1];
	if (!shader.SubShaderSources[2].Source.empty())
		shader.SubShaderSources[2].SPIRV = SPIRV[2];
	if (!shader.SubShaderSources[3].Source.empty())
		shader.SubShaderSources[3].SPIRV = SPIRV[3];
	if (!shader.SubShaderSources[4].Source.empty())
		shader.SubShaderSources[4].SPIRV = SPIRV[4];
	if (!shader.SubShaderSources[5].Source.empty())
		shader.SubShaderSources[5].SPIRV = SPIRV[5];

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<glslang::TShader> PreProcessGLSLForSPIRV(const char* source, const ShaderType shaderType, std::string& preProcessedSource)
{
	std::unique_ptr<glslang::TShader> shader;
	if (shaderType == ShaderType::Vertex)
	{
		shader = std::make_unique<glslang::TShader>(EShLangVertex);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangVertex, glslang::EShClientVulkan, 460);
	}
	else if (shaderType == ShaderType::Fragment)
	{
		shader = std::make_unique<glslang::TShader>(EShLangFragment);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangFragment, glslang::EShClientVulkan, 460);
	}
	else if (shaderType == ShaderType::Geometry)
	{
		shader = std::make_unique<glslang::TShader>(EShLangGeometry);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangGeometry, glslang::EShClientVulkan, 460);
	}
	else if (shaderType == ShaderType::Tessellation_Control)
	{
		shader = std::make_unique<glslang::TShader>(EShLangTessControl);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangTessControl, glslang::EShClientVulkan, 460);
	}
	else if (shaderType == ShaderType::Tessellation_Evaluation)
	{
		shader = std::make_unique<glslang::TShader>(EShLangTessEvaluation);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangTessEvaluation, glslang::EShClientVulkan, 460);
	}
	else if (shaderType == ShaderType::Compute)
	{
		shader = std::make_unique<glslang::TShader>(EShLangCompute);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangCompute, glslang::EShClientVulkan, 460);
	}

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

		/* .limits = */
		{
			/* .nonInductiveForLoops = */ 1,
			/* .whileLoops = */ 1,
			/* .doWhileLoops = */ 1,
			/* .generalUniformIndexing = */ 1,
			/* .generalAttributeMatrixVectorIndexing = */ 1,
			/* .generalVaryingIndexing = */ 1,
			/* .generalSamplerIndexing = */ 1,
			/* .generalVariableIndexing = */ 1,
			/* .generalConstantMatrixVectorIndexing = */ 1,
		}
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
		std::cout << "[GLSL] Preprocessing failed!" << '\n';
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

		/* .limits = */
		{
			/* .nonInductiveForLoops = */ 1,
			/* .whileLoops = */ 1,
			/* .doWhileLoops = */ 1,
			/* .generalUniformIndexing = */ 1,
			/* .generalAttributeMatrixVectorIndexing = */ 1,
			/* .generalVaryingIndexing = */ 1,
			/* .generalSamplerIndexing = */ 1,
			/* .generalVariableIndexing = */ 1,
			/* .generalConstantMatrixVectorIndexing = */ 1,
		}
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

bool LinkGLSL(glslang::TShader* VShader,
          glslang::TShader* FShader,
          glslang::TShader* GShader,
          glslang::TShader* TCShader,
          glslang::TShader* TEShader,
          glslang::TShader* CShader,
          glslang::TProgram& program)
{
	if (VShader)
		program.addShader(VShader);
	if (FShader)
		program.addShader(FShader);
	if (GShader)
		program.addShader(GShader);
	if (TCShader)
		program.addShader(TCShader);
	if (TEShader)
		program.addShader(TEShader);
	if (CShader)
		program.addShader(CShader);

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

std::vector<std::vector<uint32_t>> ConvertToSPIRV(glslang::TShader* VShader,
                                                  glslang::TShader* FShader,
                                                  glslang::TShader* GShader,
                                                  glslang::TShader* TCShader,
                                                  glslang::TShader* TEShader,
                                                  glslang::TShader* CShader,
                                                  glslang::TProgram& program)
{
	std::vector<std::vector<uint32_t>> SPIRV(6);

	if (VShader)
	{
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;
		GlslangToSpv(*program.getIntermediate(EShLangVertex), SPIRV[0], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			std::cout << "[SPIRV] Vertex Shader: " << logger.getAllMessages() << '\n';
	}

	if (FShader)
	{
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;
		GlslangToSpv(*program.getIntermediate(EShLangFragment), SPIRV[1], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			std::cout << "[SPIRV] Fragment Shader: " << logger.getAllMessages() << '\n';
	}

	if (GShader)
	{
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;
		GlslangToSpv(*program.getIntermediate(EShLangGeometry), SPIRV[2], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			std::cout << "[SPIRV] Geometry Shader: " << logger.getAllMessages() << '\n';
	}

	if (TCShader)
	{
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;
		GlslangToSpv(*program.getIntermediate(EShLangTessControl), SPIRV[3], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			std::cout << "[SPIRV] Tessellation Control Shader: " << logger.getAllMessages() << '\n';
	}

	if (TEShader)
	{
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;
		GlslangToSpv(*program.getIntermediate(EShLangTessEvaluation), SPIRV[4], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			std::cout << "[SPIRV] Tessellation Evaluation Shader: " << logger.getAllMessages() << '\n';
	}

	if (CShader)
	{
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;
		GlslangToSpv(*program.getIntermediate(EShLangCompute), SPIRV[5], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			std::cout << "[SPIRV] Compute Shader: " << logger.getAllMessages() << '\n';
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
		
		for(auto& subShader : shader.SubShaderSources)
		{
			if(!subShader.SPIRV.empty())
			{
				uint32_t SPIRVSize = static_cast<uint32_t>(subShader.SPIRV.size());
				uint32_t type = static_cast<uint32_t>(subShader.Type);

				file.write(reinterpret_cast<char*>(&SPIRVSize), sizeof(uint32_t));
				file.write(reinterpret_cast<char*>(&type), sizeof(uint32_t));
				file.write(reinterpret_cast<char*>(subShader.SPIRV.data()), subShader.SPIRV.size() * sizeof(uint32_t));
			}
		}
		
		file.close();
	}
	else
		std::cout << "Couldn't open file: " << filepath << '\n';	
}