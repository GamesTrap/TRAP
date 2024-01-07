#include "TRAPPCH.h"
#include "Shader.h"

#include "Application.h"
#include "FileSystem/FileSystem.h"
#include "Graphics/API/Vulkan/Objects/VulkanShader.h"
#include "Utils/String/String.h"
#include "Utils/Memory.h"

struct ShaderStageData
{
	TRAP::Graphics::RendererAPI::ShaderStage Stage;
	std::string_view StageString;
	EShLanguage StageGLSLang;
};

static constexpr std::array<ShaderStageData, std::to_underlying(TRAP::Graphics::RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> ShaderStages
{
	{
		{TRAP::Graphics::RendererAPI::ShaderStage::Vertex, "Vertex", EShLanguage::EShLangVertex},
		{TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl, "TessellationControl", EShLanguage::EShLangTessControl},
		{TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation, "TessellationEvaluation", EShLanguage::EShLangTessEvaluation},
		{TRAP::Graphics::RendererAPI::ShaderStage::Geometry, "Geometry", EShLanguage::EShLangGeometry},
		{TRAP::Graphics::RendererAPI::ShaderStage::Fragment, "Fragment", EShLanguage::EShLangFragment},
		{TRAP::Graphics::RendererAPI::ShaderStage::Compute, "Compute", EShLanguage::EShLangCompute},
		{TRAP::Graphics::RendererAPI::ShaderStage::RayTracing, "RayTracing", EShLanguage::EShLangCount} //TODO RayTracing Shader support
	}
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr std::string_view ShaderStageToString(const TRAP::Graphics::RendererAPI::ShaderStage stage)
{
	const auto it = std::ranges::find_if(ShaderStages,
	                             [stage](const ShaderStageData& element){return stage == element.Stage;});
	return it->StageString;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr EShLanguage ShaderStageToEShLanguage(const TRAP::Graphics::RendererAPI::ShaderStage stage)
{
	const auto it = std::ranges::find_if(ShaderStages,
	                             [stage](const auto& element){return stage == element.Stage;});
	return it->StageGLSLang;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Shader::Reload()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(m_filepath.empty())
		return false;

	//Shutdown the current shader
	Shutdown();

	//Try to load shader from file and fill the BinaryShaderDesc
	//Basically does more or less the same as PreInit does
	std::string glslSource;
	bool isSPIRV = false;
	RendererAPI::BinaryShaderDesc desc;
	std::vector<u8> SPIRVSource{};

	if(!IsFileEndingSupported(m_filepath))
		return false;

	isSPIRV = CheckTRAPShaderMagicNumber(m_filepath);

	if (!isSPIRV)
	{
		const auto loadedData = FileSystem::ReadTextFile(m_filepath);
		if(loadedData)
			glslSource = *loadedData;
	}
	else
	{
		auto loadedData = FileSystem::ReadFile(m_filepath);
		if(loadedData)
			SPIRVSource = std::move(*loadedData);
	}

	if(isSPIRV && SPIRVSource.empty())
		TP_ERROR(Log::ShaderSPIRVPrefix, "Couldn't load shader: \"", m_name, "\"!");
	else if (glslSource.empty())
		TP_ERROR(Log::ShaderGLSLPrefix, "Couldn't load shader: \"", m_name, "\"!");

	if ((glslSource.empty() && !isSPIRV) || (SPIRVSource.empty() && isSPIRV))
	{
		TP_WARN(Log::ShaderPrefix, "Skipping unrecognized file ", m_filepath);
		return false;
	}

	m_shaderStages = RendererAPI::ShaderStage::None;
	if (!isSPIRV)
	{
		std::vector<std::pair<std::string, RendererAPI::ShaderStage>> shaders{};
		if (!PreProcessGLSL(glslSource, shaders, &m_macros) || !ValidateShaderStages(shaders))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", m_name, "\" using fallback shader");
			m_valid = false;
			return false;
		}

		desc = ConvertGLSLToSPIRV(shaders);
	}
	else
		desc = LoadSPIRV(SPIRVSource);

	if (desc.Stages == RendererAPI::ShaderStage::None)
	{
		m_valid = false;
		return false;
	}

	m_valid = true;
	m_shaderStages = desc.Stages;

	//Init the new shader
	Init(desc);

	return true;
}
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::filesystem::path TRAP::Graphics::Shader::GetFilePath() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::Shader::GetRootSignature() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_rootSignature;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::string& name,
                                                                                       const std::filesystem::path& filePath,
																		               const std::vector<Macro>* const userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(name.empty())
	{
		TP_WARN(Log::ShaderPrefix, "Name is empty! Using filename as mame!");
		return CreateFromFile(filePath, userMacros);
	}

	RendererAPI::BinaryShaderDesc desc{};
	Ref<Shader> failShader = nullptr;
	if(!PreInit(name, filePath, userMacros, desc, failShader))
		return failShader;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		Ref<API::VulkanShader> result = MakeRef<API::VulkanShader>(name, filePath, desc, userMacros);

		//Hot reloading
		const auto hotReloadingFileSystemWatcher = TRAP::Application::GetHotReloadingFileSystemWatcher();

		if(hotReloadingFileSystemWatcher && !filePath.empty())
		{
			const auto folderPath = FileSystem::GetFolderPath(filePath);
			if(folderPath)
				hotReloadingFileSystemWatcher->get().AddFolder(*folderPath);
		}

		return result;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Shader::CreateFromFile(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::filesystem::path& filePath,
                                                                                       const std::vector<Macro>* const userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	RendererAPI::BinaryShaderDesc desc{};
	Ref<Shader> failShader = nullptr;
	const auto name = FileSystem::GetFileNameWithoutEnding(filePath);
	if(!name)
	{
		TRAP_ASSERT(false, "Shader::CreateFromFile(): Name is empty!");
		TP_ERROR(Log::ShaderPrefix, "Name is empty!");
		return nullptr;
	}

	if(!PreInit(*name, filePath, userMacros, desc, failShader))
		return failShader;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		Ref<API::VulkanShader> result = MakeRef<API::VulkanShader>(*name, filePath, desc, userMacros);

		//Hot reloading
		const auto hotReloadingFileSystemWatcher = TRAP::Application::GetHotReloadingFileSystemWatcher();

		if(hotReloadingFileSystemWatcher && !filePath.empty())
		{
			const auto folderPath = FileSystem::GetFolderPath(filePath);
			if(folderPath)
				hotReloadingFileSystemWatcher->get().AddFolder(*folderPath);
		}

		return result;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Shader::CreateFromFile(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromSource(const std::string& name,
                                                                                         const std::string& glslSource,
																		                 const std::vector<Macro>* const userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	std::vector<std::pair<std::string, RendererAPI::ShaderStage>> shaders{};
	if(!PreProcessGLSL(glslSource, shaders, userMacros) || !ValidateShaderStages(shaders))
	{
		TP_WARN(Log::ShaderPrefix, "Shader: \"", name, "\" using fallback shader");
		return nullptr;
	}

	const RendererAPI::BinaryShaderDesc desc = ConvertGLSLToSPIRV(shaders);
	if (desc.Stages == RendererAPI::ShaderStage::None)
		return nullptr;

	switch (RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return MakeRef<API::VulkanShader>(name, desc, userMacros);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Shader::CreateFromSource(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader::Shader(std::string name, const bool valid, const RendererAPI::ShaderStage stages,
                               const std::vector<Macro>* const userMacros, const std::filesystem::path& filepath)
	: m_name(std::move(name)), m_filepath(filepath), m_shaderStages(stages), m_valid(valid)
{
	if(userMacros != nullptr)
		m_macros = *userMacros;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Shader::CheckTRAPShaderMagicNumber(const std::filesystem::path& filePath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	//Check Shader Magic Number
	if (!FileSystem::Exists(filePath))
		return false;

	std::ifstream file(filePath, std::ios::binary);

	if(!file.is_open())
	{
		TP_ERROR(Log::FileSystemPrefix, "Couldn't open file: ", filePath);
		return false;
	}

	std::string magicNumber(8, '\0');
	file.read(magicNumber.data(), NumericCast<std::streamsize>(magicNumber.size()));
	file.close();

	return magicNumber == ShaderMagicNumber;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Shader::PreProcessGLSL(const std::string& glslSource,
                                                          std::vector<std::pair<std::string, RendererAPI::ShaderStage>>& shaders,
											              const std::vector<Macro>* const userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	RendererAPI::ShaderStage currentShaderStage = RendererAPI::ShaderStage::None;
	const std::vector<std::string> lines = Utils::String::GetLines(glslSource);

	//Go through every line of the shader source
	for(usize i = 0; i < lines.size(); ++i)
	{
		//Optimization lines converted to lower case
		const std::string lowerLine = Utils::String::ToLower(lines[i]);

		//Search for a shader type tag
		if(lowerLine.starts_with("#shader"))
		{
			//Detect shader type
			if (Utils::String::Contains(lowerLine, "vertex"))
				currentShaderStage = RendererAPI::ShaderStage::Vertex;
			else if (Utils::String::Contains(lowerLine, "fragment") ||
				     Utils::String::Contains(lowerLine, "pixel"))
				currentShaderStage = RendererAPI::ShaderStage::Fragment;
			else if (Utils::String::Contains(lowerLine, "geometry"))
				currentShaderStage = RendererAPI::ShaderStage::Geometry;
			else if (Utils::String::Contains(lowerLine, "tessellation"))
			{
				//Either Control or Evaluation
				if (Utils::String::Contains(lowerLine, "control"))
					currentShaderStage = RendererAPI::ShaderStage::TessellationControl;
				else if (Utils::String::Contains(lowerLine, "evaluation"))
					currentShaderStage = RendererAPI::ShaderStage::TessellationEvaluation;
			}
			else if (Utils::String::Contains(lowerLine, "compute"))
				currentShaderStage = RendererAPI::ShaderStage::Compute;
			//TODO RayTracing Shaders i.e. "RayGen" "AnyHit" "ClosestHit" "Miss" "Intersection" ("Callable")

			//Check for duplicate "#shader XXX" defines
			if (std::ranges::any_of(shaders,
			    [currentShaderStage](const auto& element){return (element.second & currentShaderStage) != RendererAPI::ShaderStage::None;}))
			{
				TP_ERROR(Log::ShaderGLSLPrefix, "Found duplicate \"#shader\" define: ", lines[i]);
				return false;
			}

			shaders.emplace_back("", currentShaderStage);
		}
		else if(Utils::String::Contains(lowerLine, "#version")) //Check for unnecessary "#version" define
			TP_WARN(Log::ShaderGLSLPrefix, "Found tag: \"", lines[i], "\" this is unnecessary! Skipping line: ", i);
		else if(currentShaderStage != RendererAPI::ShaderStage::None) //Add shader code to detected shader stage
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

			shaders.back().first.append(lines[i] + '\n');
		}
	}

	for(auto& [shaderCode, shaderStage] : shaders)
	{
		if (!Utils::String::Contains(Utils::String::ToLower(shaderCode), "main"))
		{
			TP_ERROR(Log::ShaderGLSLPrefix, ShaderStageToString(shaderStage), " shader couldn't find \"main\" function!");
			return false;
		}

		std::string preprocessed;
		if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		{
			//Found main function
			//Add GLSL version before any shader code &
			//Add Descriptor defines
			preprocessed = "#version 460 core\n";
			for(const Macro& macro : s_defaultShaderMacrosVulkan)
				preprocessed += fmt::format("#define {} {}\n", macro.Definition, macro.Value);
		}
		else if(TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::Vulkan)
		{
			//Found main function
			//Add GLSL version before any shader code
			preprocessed = "#version 460 core\n";
		}

		if(userMacros != nullptr)
		{
			for(const Macro& macro : *userMacros)
				preprocessed += fmt::format("#define {} {}\n", macro.Definition, macro.Value);
		}

		//Add preprocessed macros to shader
		shaderCode = preprocessed + shaderCode;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Shader::ParseGLSLang(glslang::TShader& shader)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	const TBuiltInResource* const DefaultTBuiltInResource = GetDefaultResources();

	if(!shader.parse(DefaultTBuiltInResource, 460, true,
	                  static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Parsing failed: ");
		TP_ERROR(Log::ShaderGLSLPrefix, shader.getInfoLog());
		TP_ERROR(Log::ShaderGLSLPrefix, shader.getInfoDebugLog());

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Shader::LinkGLSLang(glslang::TShader& shader, glslang::TProgram& program)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	program.addShader(&shader);

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

[[nodiscard]] TRAP::Graphics::RendererAPI::BinaryShaderDesc TRAP::Graphics::Shader::ConvertGLSLToSPIRV(const std::vector<std::pair<std::string, RendererAPI::ShaderStage>>& shaders)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(!s_glslangInitialized)
	{
		if (!glslang::InitializeProcess())
			return{};
		s_glslangInitialized = true;
	}

	RendererAPI::BinaryShaderDesc desc{};

	for(const auto& [glsl, stage] : shaders)
	{
		desc.Stages |= stage;

		const char* const shaderCStr = glsl.c_str();
		glslang::TShader glslShader(ShaderStageToEShLanguage(stage));
		glslShader.setStrings(&shaderCStr, 1);
		glslShader.setEnvInput(glslang::EShSourceGlsl, ShaderStageToEShLanguage(stage), glslang::EShClientVulkan, 460);

	//TODO RayTracing

		glslShader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
		glslShader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_4);

#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::ShaderGLSLPrefix, "Parsing ", ShaderStageToString(stage), " shader");
#endif /*ENABLE_GRAPHICS_DEBUG*/
		if (!ParseGLSLang(glslShader))
			return{};

#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::ShaderGLSLPrefix, "Linking ", ShaderStageToString(stage), " shader");
#endif /*ENABLE_GRAPHICS_DEBUG*/
		glslang::TProgram program;
		if (!LinkGLSLang(glslShader, program))
			return{};

#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::ShaderSPIRVPrefix, "Converting GLSL -> SPIR-V");
#endif /*ENABLE_GRAPHICS_DEBUG*/
		const std::vector<u32> SPIRV = ConvertToSPIRV(stage, program);

		if((stage & RendererAPI::ShaderStage::Vertex) != RendererAPI::ShaderStage::None)
			desc.Vertex = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
		else if((stage & RendererAPI::ShaderStage::TessellationControl) != RendererAPI::ShaderStage::None)
			desc.TessellationControl = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
		else if((stage & RendererAPI::ShaderStage::TessellationEvaluation) != RendererAPI::ShaderStage::None)
			desc.TessellationEvaluation = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
		else if((stage & RendererAPI::ShaderStage::Geometry) != RendererAPI::ShaderStage::None)
			desc.Geometry = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
		else if((stage & RendererAPI::ShaderStage::Fragment) != RendererAPI::ShaderStage::None)
			desc.Fragment = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
		else if((stage & RendererAPI::ShaderStage::Compute) != RendererAPI::ShaderStage::None)
			desc.Compute = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
	}

	return desc;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<u32> TRAP::Graphics::Shader::ConvertToSPIRV(const RendererAPI::ShaderStage stage,
															               glslang::TProgram& program)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	std::vector<u32> SPIRV{};

	spv::SpvBuildLogger logger{};
	glslang::SpvOptions spvOptions{};

#ifdef TRAP_DEBUG
	spvOptions.validate = true;
#endif /*TRAP_DEBUG*/

	glslang::GlslangToSpv(*program.getIntermediate(ShaderStageToEShLanguage(stage)), SPIRV, &logger, &spvOptions);
	if (logger.getAllMessages().length() > 0)
		TP_ERROR(Log::ShaderSPIRVPrefix, ShaderStageToString(stage), " shader: ", logger.getAllMessages());

	//TODO RayTracing shaders

	return SPIRV;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Shader::IsFileEndingSupported(const std::filesystem::path& filePath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	const auto fileEnding = FileSystem::GetFileEnding(filePath);
	if(!fileEnding)
		return false;

	const std::string fileEndingLower = Utils::String::ToLower(*fileEnding);
	bool supportedFormat = false;
	for(const auto& fmt : SupportedShaderFormatSuffixes)
	{
		if(fileEndingLower == fmt)
		{
			supportedFormat = true;
			break;
		}
	}

	if(!supportedFormat)
	{
		TP_ERROR(Log::ShaderPrefix, "File: ", filePath, " suffix is not supported!");
		TP_WARN(Log::ShaderPrefix, "Skipping unrecognized file");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Shader::PreInit(const std::string& name, const std::filesystem::path& filePath,
                                                   const std::vector<Macro>* const userMacros,
									               RendererAPI::BinaryShaderDesc& outShaderDesc, Ref<Shader>& outFailShader)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	std::string glslSource;
	bool isSPIRV = false;
	std::vector<u8> SPIRVSource{};
	if (!filePath.empty())
	{
		if(!IsFileEndingSupported(filePath))
			return false;

		isSPIRV = CheckTRAPShaderMagicNumber(filePath);

		if (!isSPIRV)
		{
			const auto loadedData = FileSystem::ReadTextFile(filePath);
			if(loadedData)
				glslSource = *loadedData;
		}
		else
		{
			auto loadedData = FileSystem::ReadFile(filePath);
			if(loadedData)
				SPIRVSource = std::move(*loadedData);
		}
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
		TP_WARN(Log::ShaderPrefix, "Skipping unrecognized file ", filePath);
		return false;
	}

	if (!isSPIRV)
	{
		std::vector<std::pair<std::string, RendererAPI::ShaderStage>> shaders{};
		if (!PreProcessGLSL(glslSource, shaders, userMacros))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader");
			if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
				outFailShader = TRAP::MakeRef<TRAP::Graphics::API::VulkanShader>(name, filePath, userMacros);

			return false;
		}
		if (!ValidateShaderStages(shaders))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader");
			if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
				outFailShader = TRAP::MakeRef<TRAP::Graphics::API::VulkanShader>(name, filePath, userMacros);

			return false;
		}

		outShaderDesc = ConvertGLSLToSPIRV(shaders);
	}
	else
		outShaderDesc = LoadSPIRV(SPIRVSource);

	if (outShaderDesc.Stages == RendererAPI::ShaderStage::None)
	{
		if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
			outFailShader = TRAP::MakeRef<TRAP::Graphics::API::VulkanShader>(name, filePath, userMacros);

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::BinaryShaderDesc TRAP::Graphics::Shader::LoadSPIRV(std::vector<u8>& SPIRV)
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::ShaderSPIRVPrefix, "Loading SPIRV");
#endif /*ENABLE_GRAPHICS_DEBUG*/

	if(SPIRV.empty() || SPIRV.size() < (ShaderHeaderOffset + sizeof(SPIRVMagicNumber)))
		return {};

	const u32 magicNumber = Utils::Memory::ConvertByte<u32>(SPIRV.data() + ShaderHeaderOffset);

	//Check endianness of byte stream
	if (magicNumber != SPIRVMagicNumber)
	{
		Utils::Memory::SwapBytes(SPIRV.begin(), SPIRV.end()); //Convert endianness if needed
		if (magicNumber != SPIRVMagicNumber) //Recheck if SPIRV Magic Number is present
			return {};
	}

	RendererAPI::BinaryShaderDesc desc{};
	usize index = ShaderMagicNumber.size() + sizeof(u32);
	const u8 SPIRVSubShaderCount = SPIRV[index++];

	for(u32 i = 0; i < SPIRVSubShaderCount; ++i)
	{
		usize SPIRVSize = Utils::Memory::ConvertByte<usize>(SPIRV.data() + NumericCast<isize>(index));
		index += sizeof(usize);

		const RendererAPI::ShaderStage stage = static_cast<RendererAPI::ShaderStage>(SPIRV[index++]);
		desc.Stages |= stage;

		const u32 spvMagicNumber  = Utils::Memory::ConvertByte<u32>(SPIRV.data() + NumericCast<isize>(index));
		if(spvMagicNumber != SPIRVMagicNumber || (SPIRV.size() - index) < SPIRVSize)
			return {};

		switch(stage)
		{
		case RendererAPI::ShaderStage::Vertex:
			desc.Vertex.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.Vertex.ByteCode.begin());
			index += SPIRVSize * sizeof(u32);
			break;

		case RendererAPI::ShaderStage::TessellationControl:
			desc.TessellationControl.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.TessellationControl.ByteCode.begin());
			index += SPIRVSize * sizeof(u32);
			break;

		case RendererAPI::ShaderStage::TessellationEvaluation:
			desc.TessellationEvaluation.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.TessellationEvaluation.ByteCode.begin());
			index += SPIRVSize * sizeof(u32);
			break;

		case RendererAPI::ShaderStage::Geometry:
			desc.Geometry.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.Geometry.ByteCode.begin());
			index += SPIRVSize * sizeof(u32);
			break;

		case RendererAPI::ShaderStage::Fragment:
			desc.Fragment.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.Fragment.ByteCode.begin());
			index += SPIRVSize * sizeof(u32);
			break;

		case RendererAPI::ShaderStage::Compute:
			desc.Compute.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.Compute.ByteCode.begin());
			index += SPIRVSize * sizeof(u32);
			break;

		//case RendererAPI::ShaderStage::RayTracing:
			//TODO RayTracing

		case RendererAPI::ShaderStage::None:
			[[fallthrough]];
		case RendererAPI::ShaderStage::SHADER_STAGE_COUNT:
			[[fallthrough]];
		default:
			break;
		}
	}

	return desc;
}
