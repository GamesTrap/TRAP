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

const std::array<ShaderStageData, ToUnderlying(TRAP::Graphics::RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> ShaderStages
{
	{
		{TRAP::Graphics::RendererAPI::ShaderStage::Vertex, "Vertex", EShLanguage::EShLangVertex},
		{TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl, "TessellationControl", EShLanguage::EShLangTessControl},
		{TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation, "TessellationEvaluation", EShLanguage::EShLangTessEvaluation},
		{TRAP::Graphics::RendererAPI::ShaderStage::Geometry, "Geometry", EShLanguage::EShLangGeometry},
		{TRAP::Graphics::RendererAPI::ShaderStage::Fragment, "Fragment", EShLanguage::EShLangFragment},
		{TRAP::Graphics::RendererAPI::ShaderStage::Compute, "Compute", EShLanguage::EShLangCompute}
	}
};

//-------------------------------------------------------------------------------------------------------------------//

std::string_view ShaderStageToString(const TRAP::Graphics::RendererAPI::ShaderStage stage)
{
	const auto it = std::find_if(ShaderStages.begin(), ShaderStages.end(),
	                             [stage](const ShaderStageData& element){return stage == element.Stage;});
	return it->StageString;
}

//-------------------------------------------------------------------------------------------------------------------//

EShLanguage ShaderStageToEShLanguage(const TRAP::Graphics::RendererAPI::ShaderStage stage)
{
	const auto it = std::find_if(ShaderStages.begin(), ShaderStages.end(),
	                             [stage](const auto& element){return stage == element.Stage;});
	return it->StageGLSLang;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Shader::s_glslangInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Shader::Reload()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(m_filepath.empty())
		return false;

	//Shutdown the current shader
	Shutdown();

	//Try to load shader from file and fill the BinaryShaderDesc
	//Basically does more or less the same as PreInit does
	std::string glslSource;
	bool isSPIRV = false;
	RendererAPI::BinaryShaderDesc desc;
	std::vector<uint8_t> SPIRVSource{};

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
		TP_WARN(Log::ShaderPrefix, "Skipping unrecognized file \"", m_filepath, "\"");
		return false;
	}

	m_shaderStages = RendererAPI::ShaderStage::None;
	if (!isSPIRV)
	{
		std::array<std::string, ToUnderlying(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
		if (!PreProcessGLSL(glslSource, shaders, m_shaderStages, &m_macros) || !ValidateShaderStages(m_shaderStages))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", m_name, "\" using fallback shader");
			m_valid = false;
			return false;
		}

		desc = ConvertGLSLToSPIRV(shaders, m_shaderStages);
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

[[nodiscard]] std::string TRAP::Graphics::Shader::GetName() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::filesystem::path TRAP::Graphics::Shader::GetFilePath() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::ShaderStage TRAP::Graphics::Shader::GetShaderStages() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_shaderStages;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::vector<TRAP::Graphics::Shader::Macro>& TRAP::Graphics::Shader::GetMacros() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_macros;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::Shader::GetRootSignature() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_rootSignature;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::array<TRAP::Scope<TRAP::Graphics::DescriptorSet>,
                               TRAP::Graphics::RendererAPI::MaxDescriptorSets>& TRAP::Graphics::Shader::GetDescriptorSets() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_descriptorSets;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Shader::IsShaderValid() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_valid;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::string& name,
                                                                                       const std::filesystem::path& filePath,
																		               const std::vector<Macro>* const userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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
		const auto hotReloadingFileWatcher = TRAP::Application::GetHotReloadingFileWatcher();

		if(hotReloadingFileWatcher && !filePath.empty())
		{
			const auto folderPath = FileSystem::GetFolderPath(filePath);
			if(folderPath)
				hotReloadingFileWatcher->get().AddFolder(*folderPath);
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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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
		const auto hotReloadingFileWatcher = TRAP::Application::GetHotReloadingFileWatcher();

		if(hotReloadingFileWatcher && !filePath.empty())
		{
			const auto folderPath = FileSystem::GetFolderPath(filePath);
			if(folderPath)
				hotReloadingFileWatcher->get().AddFolder(*folderPath);
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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	std::array<std::string, ToUnderlying(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
	RendererAPI::ShaderStage shaderStages = RendererAPI::ShaderStage::None;
	if(!PreProcessGLSL(glslSource, shaders, shaderStages, userMacros))
	{
		TP_WARN(Log::ShaderPrefix, "Shader: \"", name, "\" using fallback shader");
		return nullptr;
	}
	if(!ValidateShaderStages(shaderStages))
	{
		TP_WARN(Log::ShaderPrefix, "Shader: \"", name, "\" using fallback shader");
		return nullptr;
	}

	const RendererAPI::BinaryShaderDesc desc = ConvertGLSLToSPIRV(shaders, shaderStages);
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

[[nodiscard]] bool TRAP::Graphics::Shader::CheckTRAPShaderMagicNumber(const std::filesystem::path& filePath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//Check Shader Magic Number
	if (!FileSystem::Exists(filePath))
		return false;

	std::ifstream file(filePath, std::ios::binary);

	if(!file.is_open())
	{
		TP_ERROR(Log::FileSystemPrefix, "Couldn't open file: ", filePath.u8string());
		return false;
	}

	std::string magicNumber(8, '\0');
	file.read(magicNumber.data(), NumericCast<std::streamsize>(magicNumber.size()));
	file.close();

	return magicNumber == ShaderMagicNumber;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Shader::PreProcessGLSL(const std::string& glslSource,
                                                          std::array<std::string, ToUnderlying(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
                                                          RendererAPI::ShaderStage& shaderStages,
											              const std::vector<Macro>* const userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	RendererAPI::ShaderStage currentShaderStage = RendererAPI::ShaderStage::None;
	const std::vector<std::string> lines = Utils::String::GetLines(glslSource);

	//Go through every line of the shader source
	for(std::size_t i = 0; i < lines.size(); ++i)
	{
		//Optimization lines converted to lower case
		const std::string lowerLine = Utils::String::ToLower(lines[i]);

		//Search for a shader type tag
		if(Utils::String::StartsWith(lowerLine, "#shader"))
		{
			//Detect shader type
			if (lowerLine.find("vertex") != std::string::npos)
				currentShaderStage = RendererAPI::ShaderStage::Vertex;
			else if (lowerLine.find("fragment") != std::string::npos ||
				     lowerLine.find("pixel") != std::string::npos)
				currentShaderStage = RendererAPI::ShaderStage::Fragment;
			else if (lowerLine.find("geometry") != std::string::npos)
				currentShaderStage = RendererAPI::ShaderStage::Geometry;
			else if (lowerLine.find("tessellation") != std::string::npos)
			{
				//Either Control or Evaluation
				if (lowerLine.find("control") != std::string::npos)
					currentShaderStage = RendererAPI::ShaderStage::TessellationControl;
				else if (lowerLine.find("evaluation") != std::string::npos)
					currentShaderStage = RendererAPI::ShaderStage::TessellationEvaluation;
			}
			else if (lowerLine.find("compute") != std::string::npos)
				currentShaderStage = RendererAPI::ShaderStage::Compute;
			//TODO RayTracing Shaders i.e. "RayGen" "AnyHit" "ClosestHit" "Miss" "Intersection" ("Callable")

			//Check for duplicate "#shader XXX" defines
			if ((shaderStages & currentShaderStage) != RendererAPI::ShaderStage::None)
			{
				TP_ERROR(Log::ShaderGLSLPrefix, "Found duplicate \"#shader\" define: ", lines[i]);
				return false;
			}

			shaderStages |= currentShaderStage;
		}
		else if(lowerLine.find("#version") != std::string::npos) //Check for unnecessary "#version" define
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

			const auto it = stageToIndex.find(currentShaderStage);
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

	for(std::size_t i = 0; i < shaders.size(); i++)
	{
		if (Utils::String::ToLower(shaders[i]).find("main") == std::string::npos)
		{
			if ((ShaderStages[i].Stage & shaderStages) != RendererAPI::ShaderStage::None)
			{
				TP_ERROR(Log::ShaderGLSLPrefix, ShaderStages[i].StageString, " shader couldn't find \"main\" function!");
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
			if(userMacros != nullptr)
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

[[nodiscard]] bool TRAP::Graphics::Shader::ParseGLSLang(glslang::TShader& shader)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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

[[nodiscard]] bool TRAP::Graphics::Shader::ValidateShaderStages(const RendererAPI::ShaderStage& shaderStages)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	//Check if any Shader Stage is set
	if (RendererAPI::ShaderStage::None == shaderStages)
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "No shader stage found!");
		return false;
	}

	//Check for "Normal" Shader Stages combined with Compute
	if((((RendererAPI::ShaderStage::Vertex & shaderStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::Fragment & shaderStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::TessellationControl & shaderStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::TessellationEvaluation & shaderStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::Geometry & shaderStages) != RendererAPI::ShaderStage::None)) &&
		((RendererAPI::ShaderStage::Compute & shaderStages) != RendererAPI::ShaderStage::None))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Rasterizer shader stages combined with compute stage!");
		return false;
	}

	//Check for "Normal" Shader Stages combined with RayTracing
	if((((RendererAPI::ShaderStage::Vertex & shaderStages) != RendererAPI::ShaderStage::None) ||
	    ((RendererAPI::ShaderStage::Fragment & shaderStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::TessellationControl & shaderStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::TessellationEvaluation & shaderStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::Geometry & shaderStages) != RendererAPI::ShaderStage::None)) &&
		((RendererAPI::ShaderStage::RayTracing & shaderStages) != RendererAPI::ShaderStage::None))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Rasterizer shader stages combined with ray tracing stage!");
		return false;
	}

	//Check for Compute Shader Stage combined with RayTracing
	if (((RendererAPI::ShaderStage::Compute & shaderStages) != RendererAPI::ShaderStage::None) &&
		((RendererAPI::ShaderStage::RayTracing & shaderStages) != RendererAPI::ShaderStage::None))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Compute shader stage combined with ray tracing stage!");
		return false;
	}

	//Check for Vertex Shader Stage & required Fragment/Pixel Shader Stage
	if(((RendererAPI::ShaderStage::Vertex & shaderStages) != RendererAPI::ShaderStage::None) &&
	   (((RendererAPI::ShaderStage::Fragment & shaderStages)) == RendererAPI::ShaderStage::None))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Only vertex shader stage provided! Missing fragment/pixel shader stage");
		return false;
	}
	//Check for Fragment/Pixel Shader Stage & required Vertex Shader Stage
	if(((RendererAPI::ShaderStage::Fragment & shaderStages) != RendererAPI::ShaderStage::None) &&
	   (((RendererAPI::ShaderStage::Vertex & shaderStages)) == RendererAPI::ShaderStage::None))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Only fragment/pixel shader stage provided! Missing vertex shader stage");
		return false;
	}

	//Shader Stages should be valid
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::BinaryShaderDesc TRAP::Graphics::Shader::ConvertGLSLToSPIRV(const std::array<std::string, ToUnderlying(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
	                                                                                                   const RendererAPI::ShaderStage& shaderStages)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!s_glslangInitialized)
	{
		if (!glslang::InitializeProcess())
			return{};
		s_glslangInitialized = true;
	}

	// std::vector<glslang::TShader> glslShaders{};
	RendererAPI::BinaryShaderDesc desc{};
	desc.Stages = shaderStages;

	for(std::size_t i = 0; i < shaders.size(); ++i)
	{
		if(shaders[i].empty())
			continue;

		const char* const shaderCStr = shaders[i].c_str();
		glslang::TShader glslShader(ShaderStageToEShLanguage(ShaderStages[i].Stage));
		glslShader.setStrings(&shaderCStr, 1);
		glslShader.setEnvInput(glslang::EShSourceGlsl, ShaderStageToEShLanguage(ShaderStages[i].Stage), glslang::EShClientVulkan, 460);

	//TODO RayTracing

		glslShader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
		glslShader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);

#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::ShaderGLSLPrefix, "Parsing ", ShaderStages[i].StageString, " shader");
#endif /*ENABLE_GRAPHICS_DEBUG*/
		if (!ParseGLSLang(glslShader))
			return{};

#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::ShaderGLSLPrefix, "Linking ", ShaderStages[i].StageString, " shader");
#endif /*ENABLE_GRAPHICS_DEBUG*/
		glslang::TProgram program;
		if (!LinkGLSLang(glslShader, program))
			return{};

#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::ShaderSPIRVPrefix, "Converting GLSL -> SPIR-V");
#endif /*ENABLE_GRAPHICS_DEBUG*/
		const std::vector<uint32_t> SPIRV = ConvertToSPIRV(ShaderStages[i].Stage, program);

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

[[nodiscard]] std::vector<uint32_t> TRAP::Graphics::Shader::ConvertToSPIRV(const RendererAPI::ShaderStage stage,
															               glslang::TProgram& program)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	std::vector<uint32_t> SPIRV{};

	spv::SpvBuildLogger logger{};
	glslang::SpvOptions spvOptions{};

#ifdef TRAP_DEBUG
	spvOptions.validate = true;
	spvOptions.disableOptimizer = true;
	spvOptions.optimizeSize = false;
#else
	spvOptions.disableOptimizer = false;
	spvOptions.optimizeSize = true;
#endif

	glslang::GlslangToSpv(*program.getIntermediate(ShaderStageToEShLanguage(stage)), SPIRV, &logger, &spvOptions);
	if (logger.getAllMessages().length() > 0)
		TP_ERROR(Log::ShaderSPIRVPrefix, ShaderStageToString(stage), " shader: ", logger.getAllMessages());

	//TODO RayTracing shaders

	return SPIRV;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::BinaryShaderDesc TRAP::Graphics::Shader::LoadSPIRV(std::vector<uint8_t>& SPIRV)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::ShaderSPIRVPrefix, "Loading SPIRV");
#endif /*ENABLE_GRAPHICS_DEBUG*/

	if(SPIRV.empty() || SPIRV.size() < (ShaderHeaderOffset + sizeof(SPIRVMagicNumber)))
		return {};

	const uint32_t magicNumber = Utils::Memory::ConvertByte<uint32_t>(SPIRV.data() + ShaderHeaderOffset);

	//Check endianness of byte stream
	if (magicNumber != SPIRVMagicNumber)
	{
		Utils::Memory::SwapBytes(SPIRV.begin(), SPIRV.end()); //Convert endianness if needed
		if (magicNumber != SPIRVMagicNumber) //Recheck if SPIRV Magic Number is present
			return {};
	}

	RendererAPI::BinaryShaderDesc desc{};
	std::size_t index = ShaderMagicNumber.size() + sizeof(uint32_t);
	const uint8_t SPIRVSubShaderCount = SPIRV[index++];

	for(uint32_t i = 0; i < SPIRVSubShaderCount; ++i)
	{
		std::size_t SPIRVSize = Utils::Memory::ConvertByte<std::size_t>(SPIRV.data() + NumericCast<std::ptrdiff_t>(index));
		index += sizeof(std::size_t);

		const RendererAPI::ShaderStage stage = static_cast<RendererAPI::ShaderStage>(SPIRV[index++]);
		desc.Stages |= stage;

		const uint32_t spvMagicNumber  = Utils::Memory::ConvertByte<uint32_t>(SPIRV.data() + NumericCast<std::ptrdiff_t>(index));
		if(spvMagicNumber != SPIRVMagicNumber || (SPIRV.size() - index) < SPIRVSize)
			return {};

		switch(stage)
		{
		case RendererAPI::ShaderStage::Vertex:
			desc.Vertex.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<std::ptrdiff_t>(index), SPIRV.begin() + NumericCast<std::ptrdiff_t>(index) + NumericCast<std::ptrdiff_t>(SPIRVSize * sizeof(uint32_t)), desc.Vertex.ByteCode.begin());
			index += SPIRVSize * sizeof(uint32_t);
			break;

		case RendererAPI::ShaderStage::TessellationControl:
			desc.TessellationControl.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<std::ptrdiff_t>(index), SPIRV.begin() + NumericCast<std::ptrdiff_t>(index) + NumericCast<std::ptrdiff_t>(SPIRVSize * sizeof(uint32_t)), desc.TessellationControl.ByteCode.begin());
			index += SPIRVSize * sizeof(uint32_t);
			break;

		case RendererAPI::ShaderStage::TessellationEvaluation:
			desc.TessellationEvaluation.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<std::ptrdiff_t>(index), SPIRV.begin() + NumericCast<std::ptrdiff_t>(index) + NumericCast<std::ptrdiff_t>(SPIRVSize * sizeof(uint32_t)), desc.TessellationEvaluation.ByteCode.begin());
			index += SPIRVSize * sizeof(uint32_t);
			break;

		case RendererAPI::ShaderStage::Geometry:
			desc.Geometry.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<std::ptrdiff_t>(index), SPIRV.begin() + NumericCast<std::ptrdiff_t>(index) + NumericCast<std::ptrdiff_t>(SPIRVSize * sizeof(uint32_t)), desc.Geometry.ByteCode.begin());
			index += SPIRVSize * sizeof(uint32_t);
			break;

		case RendererAPI::ShaderStage::Fragment:
			desc.Fragment.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<std::ptrdiff_t>(index), SPIRV.begin() + NumericCast<std::ptrdiff_t>(index) + NumericCast<std::ptrdiff_t>(SPIRVSize * sizeof(uint32_t)), desc.Fragment.ByteCode.begin());
			index += SPIRVSize * sizeof(uint32_t);
			break;

		case RendererAPI::ShaderStage::Compute:
			desc.Compute.ByteCode.resize(SPIRVSize);
			Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<std::ptrdiff_t>(index), SPIRV.begin() + NumericCast<std::ptrdiff_t>(index) + NumericCast<std::ptrdiff_t>(SPIRVSize * sizeof(uint32_t)), desc.Compute.ByteCode.begin());
			index += SPIRVSize * sizeof(uint32_t);
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

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Shader::IsFileEndingSupported(const std::filesystem::path& filePath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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
		TP_ERROR(Log::ShaderPrefix, "File: \"", filePath.u8string(), "\" suffix is not supported!");
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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	std::string glslSource;
	bool isSPIRV = false;
	std::vector<uint8_t> SPIRVSource{};
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
		TP_WARN(Log::ShaderPrefix, "Skipping unrecognized file \"", filePath, "\"");
		return false;
	}

	RendererAPI::ShaderStage shaderStages = RendererAPI::ShaderStage::None;
	if (!isSPIRV)
	{
		std::array<std::string, ToUnderlying(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
		if (!PreProcessGLSL(glslSource, shaders, shaderStages, userMacros))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader");
			if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
				outFailShader = TRAP::MakeRef<TRAP::Graphics::API::VulkanShader>(name, filePath, userMacros);

			return false;
		}
		if (!ValidateShaderStages(shaderStages))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader");
			if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
				outFailShader = TRAP::MakeRef<TRAP::Graphics::API::VulkanShader>(name, filePath, userMacros);

			return false;
		}

		outShaderDesc = ConvertGLSLToSPIRV(shaders, shaderStages);
	}
	else
		outShaderDesc = LoadSPIRV(SPIRVSource);

	if (outShaderDesc.Stages == RendererAPI::ShaderStage::None)
	{
		if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
			outFailShader = TRAP::MakeRef<TRAP::Graphics::API::VulkanShader>(name, filePath, userMacros, shaderStages);

		return false;
	}

	return true;
}