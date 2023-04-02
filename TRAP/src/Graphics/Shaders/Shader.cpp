#include "TRAPPCH.h"
#include "Shader.h"

#include "Application.h"
#include "FileSystem/FileSystem.h"
#include "Graphics/API/Vulkan/Objects/VulkanShader.h"
#include "Utils/String/String.h"
#include "Utils/Memory.h"

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
	std::vector<uint32_t> SPIRVSource{};

	if(!IsFileEndingSupported(m_filepath))
		return false;

	isSPIRV = CheckSPIRVMagicNumber(m_filepath);

	if (!isSPIRV)
	{
		const auto loadedData = FileSystem::ReadTextFile(m_filepath);
		if(loadedData)
			glslSource = *loadedData;
	}
	else
	{
		const auto loadedData = FileSystem::ReadFile(m_filepath);
		if(loadedData)
		{
			SPIRVSource.resize((*loadedData).size() / sizeof(uint32_t));
			Utils::Memory::ConvertBytes((*loadedData).begin(), (*loadedData).end(), SPIRVSource.begin());
			// SPIRVSource = Convert8To32(*loadedData);
		}
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
		std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
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
		if(TRAP::Application::IsHotReloadingEnabled())
		{
			if(!filePath.empty())
			{
				const auto folderPath = FileSystem::GetFolderPath(filePath);
				if(folderPath)
					TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
			}
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
		if(TRAP::Application::IsHotReloadingEnabled())
		{
			if(!filePath.empty())
			{
				const auto folderPath = FileSystem::GetFolderPath(filePath);
				if(folderPath)
					TRAP::Application::GetHotReloadingFileWatcher()->AddFolder(*folderPath);
			}
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

	std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
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

[[nodiscard]] bool TRAP::Graphics::Shader::CheckSPIRVMagicNumber(const std::filesystem::path& filePath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//Check SPIRV Magic Number
	if (!FileSystem::Exists(filePath))
		return false;

	std::ifstream file(filePath, std::ios::binary);

	if(!file.is_open())
	{
		TP_ERROR(Log::FileSystemPrefix, "Couldn't open file: ", filePath.u8string());
		return false;
	}

	//SPIRV Magic Number:
	//0x07230203

	uint32_t magicNumber = 0;
	file.seekg(sizeof(uint32_t) * 3, std::ios::cur); //Skip 3 uint32_ts
	file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //SPIRV Magic Number
	file.close();

	bool isMagic = magicNumber == 0x07230203;
	if (!isMagic) //Test again with swapped endianness
	{
		Utils::Memory::SwapBytes(magicNumber);
		isMagic = magicNumber == 0x07230203;
	}

	return isMagic;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Shader::PreProcessGLSL(const std::string& glslSource,
                                                          std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
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
			if (static_cast<uint32_t>(shaderStages & currentShaderStage) != 0u)
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
			if (static_cast<uint32_t>(IndexToStage[i] & shaderStages) != 0u)
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

[[nodiscard]] TRAP::Scope<glslang::TShader> TRAP::Graphics::Shader::PreProcessGLSLForSPIRVConversion(const char* const source,
	                                                                              	                 const RendererAPI::ShaderStage stage,
	                                                                              	                 std::string& preProcessedSource)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP::Scope<glslang::TShader> shader = TRAP::MakeScope<glslang::TShader>(StageToEShLang.at(stage));
	shader->setStrings(&source, 1);
	shader->setEnvInput(glslang::EShSourceGlsl, StageToEShLang.at(stage), glslang::EShClientVulkan, 460);

	//TODO RayTracing

	if(!shader)
		return nullptr;

	shader->setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
	shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);
	glslang::TShader::ForbidIncluder includer;

	const TBuiltInResource* const DefaultTBuiltInResource = GetDefaultResources();

	if(!shader->preprocess(DefaultTBuiltInResource, 460, ECoreProfile, true, true,
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

[[nodiscard]] bool TRAP::Graphics::Shader::ParseGLSLang(glslang::TShader* const shader)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const TBuiltInResource* const DefaultTBuiltInResource = GetDefaultResources();

	if(!shader->parse(DefaultTBuiltInResource, 460, true,
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

[[nodiscard]] bool TRAP::Graphics::Shader::LinkGLSLang(glslang::TShader* const shader, glslang::TProgram& program)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(shader != nullptr)
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
	if(((static_cast<uint32_t>(RendererAPI::ShaderStage::Vertex & shaderStages) != 0u) ||
		(static_cast<uint32_t>(RendererAPI::ShaderStage::Fragment & shaderStages) != 0u) ||
		(static_cast<uint32_t>(RendererAPI::ShaderStage::TessellationControl & shaderStages) != 0u) ||
		(static_cast<uint32_t>(RendererAPI::ShaderStage::TessellationEvaluation & shaderStages) != 0u) ||
		(static_cast<uint32_t>(RendererAPI::ShaderStage::Geometry & shaderStages) != 0u)) &&
		(static_cast<uint32_t>(RendererAPI::ShaderStage::Compute & shaderStages) != 0u))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Rasterizer shader stages combined with compute stage!");
		return false;
	}

	//Check for "Normal" Shader Stages combined with RayTracing
	if(((static_cast<uint32_t>(RendererAPI::ShaderStage::Vertex & shaderStages) != 0u) ||
	    (static_cast<uint32_t>(RendererAPI::ShaderStage::Fragment & shaderStages) != 0u) ||
		(static_cast<uint32_t>(RendererAPI::ShaderStage::TessellationControl & shaderStages) != 0u) ||
		(static_cast<uint32_t>(RendererAPI::ShaderStage::TessellationEvaluation & shaderStages) != 0u) ||
		(static_cast<uint32_t>(RendererAPI::ShaderStage::Geometry & shaderStages) != 0u)) &&
		(static_cast<uint32_t>(RendererAPI::ShaderStage::RayTracing & shaderStages) != 0u))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Rasterizer shader stages combined with ray tracing stage!");
		return false;
	}

	//Check for Compute Shader Stage combined with RayTracing
	if ((static_cast<uint32_t>(RendererAPI::ShaderStage::Compute & shaderStages) != 0u) &&
		(static_cast<uint32_t>(RendererAPI::ShaderStage::RayTracing & shaderStages) != 0u))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Compute shader stage combined with ray tracing stage!");
		return false;
	}

	//Check for Vertex Shader Stage & required Fragment/Pixel Shader Stage
	if((static_cast<uint32_t>(RendererAPI::ShaderStage::Vertex & shaderStages) != 0u) &&
	   ((static_cast<uint32_t>(RendererAPI::ShaderStage::Fragment & shaderStages)) == 0u))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Only vertex shader stage provided! Missing fragment/pixel shader stage");
		return false;
	}
	//Check for Fragment/Pixel Shader Stage & required Vertex Shader Stage
	if((static_cast<uint32_t>(RendererAPI::ShaderStage::Fragment & shaderStages) != 0u) &&
	   ((static_cast<uint32_t>(RendererAPI::ShaderStage::Vertex & shaderStages)) == 0u))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Only fragment/pixel shader stage provided! Missing vertex shader stage");
		return false;
	}

	//Shader Stages should be valid
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::BinaryShaderDesc TRAP::Graphics::Shader::ConvertGLSLToSPIRV(const std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
	                                                                                                   const RendererAPI::ShaderStage& shaderStages)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!s_glslangInitialized)
	{
		if (!glslang::InitializeProcess())
			return{};
		s_glslangInitialized = true;
	}

	std::array<TRAP::Scope<glslang::TShader>, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> glslShaders{};
	RendererAPI::BinaryShaderDesc desc{};
	desc.Stages = shaderStages;

	for(std::size_t i = 0; i < shaders.size(); ++i)
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

		const char* const preProcessedCStr = preProcessedSource.c_str();
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

	glslang::GlslangToSpv(*program.getIntermediate(StageToEShLang.at(stage)), SPIRV, &logger, &spvOptions);
	if (logger.getAllMessages().length() > 0)
		TP_ERROR(Log::ShaderSPIRVPrefix, StageToStr.at(stage), " shader: ", logger.getAllMessages());

	//TODO RayTracing shaders

	return SPIRV;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::BinaryShaderDesc TRAP::Graphics::Shader::LoadSPIRV(std::vector<uint32_t>& SPIRV)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::ShaderSPIRVPrefix, "Loading SPIRV");
#endif

	//Check endianness of byte strea
	bool needsEndianSwap = false;
	if (SPIRV[3] != 0x07230203)
		needsEndianSwap = true;
	if (needsEndianSwap) //Convert endianness if needed
		Utils::Memory::SwapBytes(SPIRV.begin(), SPIRV.end());

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
	std::vector<uint32_t> SPIRVSource{};
	if (!filePath.empty())
	{
		if(!IsFileEndingSupported(filePath))
			return false;

		isSPIRV = CheckSPIRVMagicNumber(filePath);

		if (!isSPIRV)
		{
			const auto loadedData = FileSystem::ReadTextFile(filePath);
			if(loadedData)
				glslSource = *loadedData;
		}
		else
		{
			const auto loadedData = FileSystem::ReadFile(filePath);
			if(loadedData)
			{
				SPIRVSource.resize((*loadedData).size() / sizeof(uint32_t));
				Utils::Memory::ConvertBytes((*loadedData).begin(), (*loadedData).end(), SPIRVSource.begin());
				//SPIRVSource = Convert8To32(*loadedData);
			}
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
		std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
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