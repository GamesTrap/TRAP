#include "TRAPPCH.h"
#include "Shader.h"

#include "DummyShader.h"
#include "FS/FS.h"
#include "Graphics/API/Vulkan/Objects/VulkanShader.h"
#include "Utils/String/String.h"

static const std::unordered_map<TRAP::Graphics::RendererAPI::ShaderStage, std::string> StageToStr
{
	{TRAP::Graphics::RendererAPI::ShaderStage::Vertex, "Vertex"},
	{TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl, "TessellationControl"},
	{TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation, "TessellationEvaluation"},
	{TRAP::Graphics::RendererAPI::ShaderStage::Geometry, "Geometry"},
	{TRAP::Graphics::RendererAPI::ShaderStage::Fragment, "Fragment"},
	{TRAP::Graphics::RendererAPI::ShaderStage::Compute, "Compute"},
	{TRAP::Graphics::RendererAPI::ShaderStage::RayTracing, "RayTracing"}
};

static const std::unordered_map<TRAP::Graphics::RendererAPI::ShaderStage, EShLanguage> StageToEShLang
{
	{TRAP::Graphics::RendererAPI::ShaderStage::Vertex, EShLangVertex},
	{TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl, EShLangTessControl},
	{TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation, EShLangTessEvaluation},
	{TRAP::Graphics::RendererAPI::ShaderStage::Geometry, EShLangGeometry},
	{TRAP::Graphics::RendererAPI::ShaderStage::Fragment, EShLangFragment},
	{TRAP::Graphics::RendererAPI::ShaderStage::Compute, EShLangCompute}
};

static const std::vector<TRAP::Graphics::RendererAPI::ShaderStage> IndexToStage
{
	TRAP::Graphics::RendererAPI::ShaderStage::Vertex,
	TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl,
	TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation,
	TRAP::Graphics::RendererAPI::ShaderStage::Geometry,
	TRAP::Graphics::RendererAPI::ShaderStage::Fragment,
	TRAP::Graphics::RendererAPI::ShaderStage::Compute,
	TRAP::Graphics::RendererAPI::ShaderStage::RayTracing
};

bool TRAP::Graphics::Shader::s_glslangInitialized = false;
std::array<std::string, 2> TRAP::Graphics::Shader::SupportedShaderFormatSuffixes{".shader", ".spirv"};

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::Shader::GetName() const
{
	TP_PROFILE_FUNCTION();

	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::filesystem::path& TRAP::Graphics::Shader::GetFilePath() const
{
	TP_PROFILE_FUNCTION();

	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::ShaderStage TRAP::Graphics::Shader::GetShaderStages() const
{
	return m_shaderStages;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::Shader::GetRootSignature() const
{
	return m_rootSignature;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<TRAP::Graphics::DescriptorSet*,
                 TRAP::Graphics::RendererAPI::MaxDescriptorSets>& TRAP::Graphics::Shader::GetDescriptorSets() const
{
	return m_descriptorSets;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::string& name,
                                                                           const std::filesystem::path& filePath,
																		   const std::vector<Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN(Log::ShaderPrefix, "Name is empty! Using filename as mame!");
		return CreateFromFile(filePath, userMacros);
	}

	std::string glslSource;
	bool isSPIRV = false;
	RendererAPI::BinaryShaderDesc desc;
	std::vector<uint32_t> SPIRVSource{};
	if (!filePath.empty())
	{
		std::string fEnding = Utils::String::ToLower(FS::GetFileEnding(filePath));
		bool supportedFormat = false;
		for(const auto& fmt : SupportedShaderFormatSuffixes)
		{
			if(fEnding == fmt)
			{
				supportedFormat = true;
				break;
			}
		}

		if(!supportedFormat)
		{
			TP_ERROR(Log::ShaderPrefix, "File: \"", filePath.generic_u8string(), "\" suffix is not supported!");
			TP_WARN(Log::ShaderPrefix, "Shader using fallback shader: \"FallbackGraphics\"");
			return nullptr;
		}

		isSPIRV = CheckSPIRVMagicNumber(filePath);

		if (!isSPIRV)
			glslSource = FS::ReadTextFile(filePath);
		else
			SPIRVSource = Convert8To32(FS::ReadFile(filePath));
	}

	if(isSPIRV)
	{
		if(!filePath.empty() && SPIRVSource.empty())
			TP_ERROR(Log::ShaderSPIRVPrefix, "Couldn't load shader: \"", name, "\"!");
	}
	else
	{
		if (!filePath.empty() && glslSource.empty())
			TP_ERROR(Log::ShaderGLSLPrefix, "Couldn't load shader: \"", name, "\"!");
	}

	if ((glslSource.empty() && !isSPIRV) || (SPIRVSource.empty() && isSPIRV))
	{
		TP_WARN(Log::ShaderPrefix, "Shader using fallback shader: \"FallbackGraphics\"");
		return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);
	}

	if (!isSPIRV)
	{
		RendererAPI::ShaderStage shaderStages = RendererAPI::ShaderStage::None;
		std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
		if (!PreProcessGLSL(glslSource, shaders, shaderStages, userMacros))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackGraphics\"");
			return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);
		}
		if (!ValidateShaderStages(shaderStages))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackGraphics\"");
			return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);
		}

		desc = ConvertGLSLToSPIRV(shaders, shaderStages);
	}
	else
		desc = LoadSPIRV(SPIRVSource);

	if (desc.Stages == RendererAPI::ShaderStage::None)
		return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		Scope<API::VulkanShader> result = MakeScope<API::VulkanShader>(name, desc);

		result->m_filepath = filePath;

		//Hot reloading
		if(TRAP::Application::IsHotReloadingEnabled())
			TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(FS::GetFolderPath(filePath));

		return result;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::filesystem::path& filePath,
                                                                           const std::vector<Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	std::string glslSource;
	std::string name;
	bool isSPIRV = false;
	RendererAPI::BinaryShaderDesc desc;
	std::vector<uint32_t> SPIRVSource{};
	if (!filePath.empty())
	{
		std::string fEnding = Utils::String::ToLower(FS::GetFileEnding(filePath));
		bool supportedFormat = false;
		for(const auto& fmt : SupportedShaderFormatSuffixes)
		{
			if(fEnding == fmt)
			{
				supportedFormat = true;
				break;
			}
		}

		if(!supportedFormat)
		{
			TP_ERROR(Log::ShaderPrefix, "File: \"", filePath.generic_u8string(), "\" suffix is not supported!");
			TP_WARN(Log::ShaderPrefix, "Shader using fallback shader: \"FallbackGraphics\"");
			return nullptr;
		}

		isSPIRV = CheckSPIRVMagicNumber(filePath);

		if (!isSPIRV)
			glslSource = FS::ReadTextFile(filePath);
		else
			SPIRVSource = Convert8To32(FS::ReadFile(filePath));

		name = FS::GetFileName(filePath);
	}

	if (isSPIRV)
	{
		if (!filePath.empty() && SPIRVSource.empty())
			TP_ERROR(Log::ShaderSPIRVPrefix, "Couldn't load shader: \"", name, "\"!");
	}
	else
	{
		if (!filePath.empty() && glslSource.empty())
			TP_ERROR(Log::ShaderGLSLPrefix, "Couldn't load shader: \"", name, "\"!");
	}

	if ((glslSource.empty() && !isSPIRV) || (SPIRVSource.empty() && isSPIRV))
	{
		TP_WARN(Log::ShaderPrefix, "Shader using fallback shader: \"FallbackGraphics\"");
		return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);
	}

	if(!isSPIRV)
	{
		RendererAPI::ShaderStage shaderStages = RendererAPI::ShaderStage::None;
		std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
		if (!PreProcessGLSL(glslSource, shaders, shaderStages, userMacros))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackGraphics\"");
			return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);
		}
		if (!ValidateShaderStages(shaderStages))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackGraphics\"");
			return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);
		}

		desc = ConvertGLSLToSPIRV(shaders, shaderStages);
	}
	else
		desc = LoadSPIRV(SPIRVSource);

	if (desc.Stages == RendererAPI::ShaderStage::None)
		return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		Scope<API::VulkanShader> result = MakeScope<API::VulkanShader>(name, desc);

		result->m_filepath = filePath;

		//Hot reloading
		if(TRAP::Application::IsHotReloadingEnabled())
			TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(FS::GetFolderPath(filePath));

		return result;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::string& name,
                                                                           const std::filesystem::path& filePath,
																		   const RendererAPI::ShaderStage stages,
																		   const std::vector<Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	std::string glslSource;
	bool isSPIRV = false;
	RendererAPI::BinaryShaderDesc desc;
	std::vector<uint32_t> SPIRVSource{};
	if (!filePath.empty())
	{
		std::string fEnding = Utils::String::ToLower(FS::GetFileEnding(filePath));
		bool supportedFormat = false;
		for(const auto& fmt : SupportedShaderFormatSuffixes)
		{
			if(fEnding == fmt)
			{
				supportedFormat = true;
				break;
			}
		}

		if(!supportedFormat)
		{
			TP_ERROR(Log::ShaderPrefix, "File: \"", filePath.generic_u8string(), "\" suffix is not supported!");
			TP_WARN(Log::ShaderPrefix, "Shader using fallback shader: \"FallbackGraphics\"");
			return nullptr;
		}

		isSPIRV = CheckSPIRVMagicNumber(filePath);

		if (!isSPIRV)
			glslSource = FS::ReadTextFile(filePath);
		else
			SPIRVSource = Convert8To32(FS::ReadFile(filePath));
	}

	if(isSPIRV)
	{
		if(!filePath.empty() && SPIRVSource.empty())
			TP_ERROR(Log::ShaderSPIRVPrefix, "Couldn't load shader: \"", name, "\"!");
	}
	else
	{
		if (!filePath.empty() && glslSource.empty())
			TP_ERROR(Log::ShaderGLSLPrefix, "Couldn't load shader: \"", name, "\"!");
	}

	if ((glslSource.empty() && !isSPIRV) || (SPIRVSource.empty() && isSPIRV))
	{
		if(static_cast<bool>(stages & RendererAPI::ShaderStage::Compute))
			TP_WARN(Log::ShaderPrefix, "Shader using fallback shader: \"FallbackCompute\"");
		else
			TP_WARN(Log::ShaderPrefix, "Shader using fallback shader: \"FallbackGraphics\"");

		return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath, stages);
	}

	if (!isSPIRV)
	{
		RendererAPI::ShaderStage shaderStages = RendererAPI::ShaderStage::None;
		std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
		if (!PreProcessGLSL(glslSource, shaders, shaderStages, userMacros))
		{
			if(static_cast<bool>(stages & RendererAPI::ShaderStage::Compute))
				TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackCompute\"");
			else
				TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackGraphics\"");

			return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath, stages);
		}
		if (!ValidateShaderStages(shaderStages))
		{
			if(static_cast<bool>(stages & RendererAPI::ShaderStage::Compute))
				TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackCompute\"");
			else
				TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackGraphics\"");

			return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath, stages);
		}

		desc = ConvertGLSLToSPIRV(shaders, shaderStages);
	}
	else
		desc = LoadSPIRV(SPIRVSource);

	if (desc.Stages == RendererAPI::ShaderStage::None)
	{
		if(static_cast<bool>(stages & RendererAPI::ShaderStage::Compute))
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackCompute\"");
		else
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackGraphics\"");

		return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath, stages);
	}

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		Scope<API::VulkanShader> result = MakeScope<API::VulkanShader>(name, desc);

		result->m_filepath = filePath;

		return result;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromSource(const std::string& name,
                                                                             const std::string& glslSource,
																			 const std::vector<Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
	RendererAPI::ShaderStage shaderStages = RendererAPI::ShaderStage::None;
	if(!PreProcessGLSL(glslSource, shaders, shaderStages, userMacros))
	{
		TP_WARN(Log::ShaderPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackGraphics\"");
		return nullptr;
	}
	if(!ValidateShaderStages(shaderStages))
	{
		TP_WARN(Log::ShaderPrefix, "Shader: \"", name, "\" using fallback shader: \"FallbackGraphics\"");
		return nullptr;
	}

	RendererAPI::BinaryShaderDesc desc = ConvertGLSLToSPIRV(shaders, shaderStages);
	if (desc.Stages == RendererAPI::ShaderStage::None)
		return nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeScope<API::VulkanShader>(name, desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Shader::CheckSPIRVMagicNumber(const std::filesystem::path& filePath)
{
	//Check SPIRV Magic Number
	if (!FS::FileOrFolderExists(filePath))
		return false;

	std::ifstream file(filePath, std::ios::binary);

	if(!file.is_open())
	{
		TP_ERROR(Log::FileSystemPrefix, "Couldn't open file: ", filePath.generic_u8string());
		return false;
	}

	//SPIRV Magic Number:
	//0x07230203

	uint32_t magicNumber = 0;
	file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //Number of SubShaders
	file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //Size of the current SubShader in uint32_ts
	file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //Type of the current SubShader
	file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //SPIRV Magic Number
	file.close();

	return magicNumber == 0x07230203;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint32_t> TRAP::Graphics::Shader::Convert8To32(const std::vector<uint8_t>& source)
{
	//BUG Depends on Endianness ?!
	std::vector<uint32_t> data{};
	data.resize(source.size() / 4);

	std::size_t j = 0;
	for(std::size_t i = 0; i < source.size(); i += 4)
	{
		const uint32_t val = source[i] |
			                 (source[i + 1] << 8) |
			                 (source[i + 2] << 16) |
			                 (source[i + 3] << 24);
		data[j++] = val;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Shader::PreProcessGLSL(const std::string& glslSource,
                                            std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
                                            RendererAPI::ShaderStage& shaderStages,
											const std::vector<Macro>* userMacros)
{
	RendererAPI::ShaderStage currentShaderStage = RendererAPI::ShaderStage::None;
	std::vector<std::string> lines = Utils::String::GetLines(glslSource);

	//Go through every line of the shader source
	for(std::size_t i = 0; i < lines.size(); ++i)
	{
		//Optimization lines converted to lower case
		std::string lowerLine = Utils::String::ToLower(lines[i]);

		//Search for a shader type tag
		if(Utils::String::StartsWith(lowerLine, "#shader"))
		{
			//Detect shader type
			if (Utils::String::FindToken(lowerLine, "vertex"))
				currentShaderStage = RendererAPI::ShaderStage::Vertex;
			else if (Utils::String::FindToken(lowerLine, "fragment") ||
				     Utils::String::FindToken(lowerLine, "pixel"))
				currentShaderStage = RendererAPI::ShaderStage::Fragment;
			else if (Utils::String::FindToken(lowerLine, "geometry"))
				currentShaderStage = RendererAPI::ShaderStage::Geometry;
			else if (Utils::String::FindToken(lowerLine, "tessellation"))
			{
				//Either Control or Evaluation
				if (Utils::String::FindToken(lowerLine, "control"))
					currentShaderStage = RendererAPI::ShaderStage::TessellationControl;
				else if (Utils::String::FindToken(lowerLine, "evaluation"))
					currentShaderStage = RendererAPI::ShaderStage::TessellationEvaluation;
			}
			else if (Utils::String::FindToken(lowerLine, "compute"))
				currentShaderStage = RendererAPI::ShaderStage::Compute;
			//TODO RayTracing Shaders i.e. "RayGen" "AnyHit" "ClosestHit" "Miss" "Intersection" ("Callable")

			//Check for duplicate "#shader XXX" defines
			if (static_cast<uint32_t>(shaderStages & currentShaderStage))
			{
				TP_ERROR(Log::ShaderGLSLPrefix, "Found duplicate \"#shader\" define: ", lines[i]);
				return false;
			}

			shaderStages |= currentShaderStage;
		}
		else if(Utils::String::FindToken(lowerLine, "#version")) //Check for unnecessary "#version" define
			TP_WARN(Log::ShaderGLSLPrefix, "Found tag: \"", lines[i], "\" this is unnecessary! Skipping line: ", i);
		else if(currentShaderStage != RendererAPI::ShaderStage::None) //Add shader code to detected shader stage
		{
			static const std::unordered_map<RendererAPI::ShaderStage, uint32_t> stageToIndex
			{
				{RendererAPI::ShaderStage::Vertex, 0},
				{RendererAPI::ShaderStage::TessellationControl, 1},
				{RendererAPI::ShaderStage::TessellationEvaluation, 2},
				{RendererAPI::ShaderStage::Geometry, 3},
				{RendererAPI::ShaderStage::Fragment, 4},
				{RendererAPI::ShaderStage::Compute, 5},
				{RendererAPI::ShaderStage::RayTracing, 6}
			};

			auto it = stageToIndex.find(currentShaderStage);
			if(it != stageToIndex.end())
			{
				if(currentShaderStage == RendererAPI::ShaderStage::RayTracing)
				{
					TP_WARN(Log::ShaderGLSLPrefix, "RayTracing shader support is WIP!");
					return false;
				}
				if(currentShaderStage == RendererAPI::ShaderStage::None)
				{
					TP_ERROR(Log::ShaderGLSLPrefix, "Unsupported shader type!");
					return false;
				}

				shaders[it->second].append(lines[i] + '\n');
			}
		}
	}

	for(uint32_t i = 0; i < shaders.size(); i++)
	{
		if (Utils::String::ToLower(shaders[i]).find("main") == std::string::npos)
		{
			if (static_cast<uint32_t>(IndexToStage[i] & shaderStages))
			{
				TP_ERROR(Log::ShaderGLSLPrefix, StageToStr.at(IndexToStage[i]), " shader couldn't find \"main\" function!");
				return false;
			}
		}

		std::string preprocessed;
		if (!shaders[i].empty() && TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		{
			//Found main function
			//Add GLSL version before any shader code &
			//Add Descriptor defines
			preprocessed = "#version 460 core\n";
			for(const Macro& macro : s_defaultShaderMacrosVulkan)
				preprocessed += "#define " + macro.Definition + " " + macro.Value + '\n';
		}
		else if(!shaders[i].empty() &&
		        TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::Vulkan)
		{
			//Found main function
			//Add GLSL version before any shader code
			preprocessed = "#version 460 core\n";
		}

		if(!shaders[i].empty())
		{
			if(userMacros)
			{
				for(const Macro& macro : *userMacros)
					preprocessed += "#define " + macro.Definition + " " + macro.Value + '\n';
			}

			//Add preprocessed macros to shader
			shaders[i] = preprocessed + shaders[i];
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<glslang::TShader> TRAP::Graphics::Shader::PreProcessGLSLForSPIRVConversion(const char* source,
	                                                                              	   const RendererAPI::ShaderStage stage,
	                                                                              	   std::string& preProcessedSource)
{
	TRAP::Scope<glslang::TShader> shader = TRAP::MakeScope<glslang::TShader>(StageToEShLang.at(stage));
	shader->setStrings(&source, 1);
	shader->setEnvInput(glslang::EShSourceGlsl, StageToEShLang.at(stage), glslang::EShClientVulkan, 460);

	//TODO RayTracing

	if(!shader)
		return nullptr;

	shader->setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
	shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);
	glslang::TShader::ForbidIncluder includer;
	constexpr static TBuiltInResource DefaultTBuiltInResource = GetDefaultTBuiltInResource();

	if(!shader->preprocess(&DefaultTBuiltInResource, 460, ECoreProfile, true, true,
		                   static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules),
		                   &preProcessedSource, includer))
	{
		TP_ERROR(Log::ShaderSPIRVPrefix, "GLSL -> SPIR-V conversion preprocessing failed!");
		TP_ERROR(Log::ShaderSPIRVPrefix, shader->getInfoLog());
		TP_ERROR(Log::ShaderSPIRVPrefix, shader->getInfoDebugLog());

		return nullptr;
	}

	return shader;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Shader::ParseGLSLang(glslang::TShader* shader)
{
	constexpr static TBuiltInResource DefaultTBuiltInResource = GetDefaultTBuiltInResource();

	if(!shader->parse(&DefaultTBuiltInResource, 460, true,
	                  static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Parsing failed: ");
		TP_ERROR(Log::ShaderGLSLPrefix, shader->getInfoLog());
		TP_ERROR(Log::ShaderGLSLPrefix, shader->getInfoDebugLog());

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Shader::LinkGLSLang(glslang::TShader* shader, glslang::TProgram& program)
{
	if(shader)
		program.addShader(shader);

	if(!program.link(static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Linking failed: ");
		TP_ERROR(Log::ShaderGLSLPrefix, program.getInfoLog());
		TP_ERROR(Log::ShaderGLSLPrefix, program.getInfoDebugLog());

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Shader::ValidateShaderStages(const RendererAPI::ShaderStage& shaderStages)
{
	//Check if any Shader Stage is set
	if (RendererAPI::ShaderStage::None == shaderStages)
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "No shader stage found!");
		return false;
	}

	//Check for "Normal" Shader Stages combined with Compute
	if((static_cast<uint32_t>(RendererAPI::ShaderStage::Vertex & shaderStages) ||
		static_cast<uint32_t>(RendererAPI::ShaderStage::Fragment & shaderStages) ||
		static_cast<uint32_t>(RendererAPI::ShaderStage::TessellationControl & shaderStages) ||
		static_cast<uint32_t>(RendererAPI::ShaderStage::TessellationEvaluation & shaderStages) ||
		static_cast<uint32_t>(RendererAPI::ShaderStage::Geometry & shaderStages)) &&
		static_cast<uint32_t>(RendererAPI::ShaderStage::Compute & shaderStages))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Rasterizer shader stages combined with compute stage!");
		return false;
	}

	//Check for "Normal" Shader Stages combined with RayTracing
	if((static_cast<uint32_t>(RendererAPI::ShaderStage::Vertex & shaderStages) ||
	    static_cast<uint32_t>(RendererAPI::ShaderStage::Fragment & shaderStages) ||
		static_cast<uint32_t>(RendererAPI::ShaderStage::TessellationControl & shaderStages) ||
		static_cast<uint32_t>(RendererAPI::ShaderStage::TessellationEvaluation & shaderStages) ||
		static_cast<uint32_t>(RendererAPI::ShaderStage::Geometry & shaderStages)) &&
		static_cast<uint32_t>(RendererAPI::ShaderStage::RayTracing & shaderStages))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Rasterizer shader stages combined with ray tracing stage!");
		return false;
	}

	//Check for Compute Shader Stage combined with RayTracing
	if (static_cast<uint32_t>(RendererAPI::ShaderStage::Compute & shaderStages) &&
		static_cast<uint32_t>(RendererAPI::ShaderStage::RayTracing & shaderStages))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Compute shader stage combined with ray tracing stage!");
		return false;
	}

	//Check for Vertex Shader Stage & required Fragment/Pixel Shader Stage
	if(static_cast<uint32_t>(RendererAPI::ShaderStage::Vertex & shaderStages) &&
	   !(static_cast<uint32_t>(RendererAPI::ShaderStage::Fragment & shaderStages)))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Only vertex shader stage provided! Missing fragment/pixel shader stage");
		return false;
	}
	//Check for Fragment/Pixel Shader Stage & required Vertex Shader Stage
	if(static_cast<uint32_t>(RendererAPI::ShaderStage::Fragment & shaderStages) &&
	   !(static_cast<uint32_t>(RendererAPI::ShaderStage::Vertex & shaderStages)))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Only fragment/pixel shader stage provided! Missing vertex shader stage");
		return false;
	}

	//Shader Stages should be valid
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::BinaryShaderDesc TRAP::Graphics::Shader::ConvertGLSLToSPIRV(const std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
	                                                                                     const RendererAPI::ShaderStage& shaderStages)
{
	if(!s_glslangInitialized)
	{
		if (!glslang::InitializeProcess())
			return{};
		s_glslangInitialized = true;
	}

	std::array<TRAP::Scope<glslang::TShader>, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> glslShaders{};
	RendererAPI::BinaryShaderDesc desc{};
	desc.Stages = shaderStages;

	for(uint32_t i = 0; i < shaders.size(); ++i)
	{
		if(shaders[i].empty())
			continue;

		#ifdef ENABLE_GRAPHICS_DEBUG
			TP_DEBUG(Log::ShaderGLSLPrefix, "Pre-Processing ", StageToStr.at(IndexToStage[i]), " shader");
		#endif
			std::string preProcessedSource;
			glslShaders[i] = PreProcessGLSLForSPIRVConversion(shaders[i].data(), IndexToStage[i],
														      preProcessedSource);
			if (preProcessedSource.empty())
				return{};

			const char* preProcessedCStr = preProcessedSource.c_str();
			glslShaders[i]->setStrings(&preProcessedCStr, 1);

		#ifdef ENABLE_GRAPHICS_DEBUG
			TP_DEBUG(Log::ShaderGLSLPrefix, "Parsing ", StageToStr.at(IndexToStage[i]), " shader");
		#endif
			if (!ParseGLSLang(glslShaders[i].get()))
				return{};

		#ifdef ENABLE_GRAPHICS_DEBUG
			TP_DEBUG(Log::ShaderGLSLPrefix, "Linking ", StageToStr.at(IndexToStage[i]), " shader");
		#endif
			glslang::TProgram program;
			if (!LinkGLSLang(glslShaders[i].get(), program))
				return{};

		#ifdef ENABLE_GRAPHICS_DEBUG
			TP_DEBUG(Log::ShaderSPIRVPrefix, "Converting GLSL -> SPIR-V");
		#endif
			const std::vector<uint32_t> SPIRV = ConvertToSPIRV(IndexToStage[i], program);

		switch(i)
		{
		case 0:
			desc.Vertex = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			break;

		case 1:
			desc.TessellationControl = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			break;

		case 2:
			desc.TessellationEvaluation = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			break;

		case 3:
			desc.Geometry = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			break;

		case 4:
			desc.Fragment = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			break;

		case 5:
			desc.Compute = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			break;

			//TODO RayTracing Shaders

		default:
			break;
		}
	}

	return desc;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint32_t> TRAP::Graphics::Shader::ConvertToSPIRV(const RendererAPI::ShaderStage stage,
															 glslang::TProgram& program)
{
	std::vector<uint32_t> SPIRV{};

	spv::SpvBuildLogger logger{};
	glslang::SpvOptions spvOptions{};

#ifdef TRAP_DEBUG
	spvOptions.validate = true;
	spvOptions.disableOptimizer = true;
#else
	spvOptions.disableOptimizer = false;
	spvOptions.optimizeSize = true;
#endif

	glslang::GlslangToSpv(*program.getIntermediate(StageToEShLang.at(stage)), SPIRV, &logger, &spvOptions);
	if (logger.getAllMessages().length() > 0)
		TP_ERROR(Log::ShaderSPIRVPrefix, StageToStr.at(stage), " shader: ", logger.getAllMessages());

	//TODO RayTracing shaders

	return SPIRV;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::BinaryShaderDesc TRAP::Graphics::Shader::LoadSPIRV(const std::vector<uint32_t>& SPIRV)
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::ShaderSPIRVPrefix, "Loading SPIRV");
#endif

	RendererAPI::BinaryShaderDesc desc{};
	uint32_t index = 0;
	const uint32_t SPIRVSubShaderCount = SPIRV[index++];

	for(uint32_t i = 0; i < SPIRVSubShaderCount; ++i)
	{
		const uint32_t SPIRVSize = SPIRV[index++];
		const RendererAPI::ShaderStage stage = static_cast<RendererAPI::ShaderStage>(SPIRV[index++]);
		desc.Stages |= stage;

		switch(stage)
		{
		case RendererAPI::ShaderStage::Vertex:
			desc.Vertex.ByteCode = std::vector<uint32_t>(SPIRV.begin() + index, SPIRV.begin() + index + SPIRVSize);
			index += SPIRVSize;
			break;

		case RendererAPI::ShaderStage::TessellationControl:
			desc.TessellationControl.ByteCode = std::vector<uint32_t>(SPIRV.begin() + index,
			                                                          SPIRV.begin() + index + SPIRVSize);
			index += SPIRVSize;
			break;

		case RendererAPI::ShaderStage::TessellationEvaluation:
			desc.TessellationEvaluation.ByteCode = std::vector<uint32_t>(SPIRV.begin() + index,
			                                                             SPIRV.begin() + index + SPIRVSize);
			index += SPIRVSize;
			break;

		case RendererAPI::ShaderStage::Geometry:
			desc.Geometry.ByteCode = std::vector<uint32_t>(SPIRV.begin() + index, SPIRV.begin() + index + SPIRVSize);
			index += SPIRVSize;
			break;

		case RendererAPI::ShaderStage::Fragment:
			desc.Fragment.ByteCode = std::vector<uint32_t>(SPIRV.begin() + index, SPIRV.begin() + index + SPIRVSize);
			index += SPIRVSize;
			break;

		case RendererAPI::ShaderStage::Compute:
			desc.Compute.ByteCode = std::vector<uint32_t>(SPIRV.begin() + index, SPIRV.begin() + index + SPIRVSize);
			index += SPIRVSize;
			break;

		//case RendererAPI::ShaderStage::RayTracing:
			//TODO RayTracing

		case RendererAPI::ShaderStage::None:
		case RendererAPI::ShaderStage::SHADER_STAGE_COUNT:
		default:
			break;
		}
	}

	return desc;
}