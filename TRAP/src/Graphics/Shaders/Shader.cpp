#include "TRAPPCH.h"
#include "Shader.h"

#include "Application.h"
#include "FileSystem/FileSystem.h"
#include "Graphics/API/Vulkan/Objects/VulkanShader.h"
#include "Utils/String/String.h"
#include "Utils/Memory.h"

namespace
{
	constexpr u32 SPIRVMagicNumber = 0x07230203u;
	constexpr std::string_view ShaderMagicNumber = "TRAP_SPV";
	constexpr usize ShaderHeaderOffset = ShaderMagicNumber.size() + sizeof(u32) + sizeof(u8) + sizeof(usize) + sizeof(u8);

	constinit bool GlslangInitialized = false;

	//Macros which are always provided by default.
	std::array<TRAP::Graphics::Shader::Macro, 2> DefaultShaderMacrosVulkan
	{
		{
			{"UpdateFreqStatic", "set = 0"},
			{"UpdateFreqDynamic", "set = 1"}
		}
	};

	//-------------------------------------------------------------------------------------------------------------------//

	struct ShaderStageData
	{
		TRAP::Graphics::RendererAPI::ShaderStage Stage;
		std::string_view StageString;
		EShLanguage StageGLSLang;
	};

	constexpr std::array<ShaderStageData, std::to_underlying(TRAP::Graphics::RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> ShaderStages
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

	/// @brief Check if given file contains the TRAP Shader magic number.
	/// @param filePath File path of the shader.
	/// @return True if file has TRAP Shader magic number, false otherwise.
	[[nodiscard]] bool CheckTRAPShaderMagicNumber(const std::filesystem::path& filePath)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

		//Check Shader Magic Number
		if (!TRAP::FileSystem::Exists(filePath))
			return false;

		std::ifstream file(filePath, std::ios::binary);

		if(!file.is_open())
		{
			TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open file: ", filePath);
			return false;
		}

		std::string magicNumber(8, '\0');
		file.read(magicNumber.data(), NumericCast<std::streamsize>(magicNumber.size()));
		file.close();

		return magicNumber == ShaderMagicNumber;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Pre process GLSL source code.
	///
	/// 1. Splits different shader types like vertex, fragment, etc.
	/// 2. Checks for duplicate shader types.
	/// 3. Automatically adds #version directive.
	/// 4. Inject default macros.
	/// 4. Inject user defined macros.
	/// @param glslSource GLSL source code.
	/// @param shaders Output: Splitted shaders.
	/// @param userMacros Optional: User defined macros.
	/// @return True if pre processing was successful, false otherwise.
	[[nodiscard]] bool PreProcessGLSL(const std::string& glslSource,
	                                  std::vector<std::pair<std::string, TRAP::Graphics::RendererAPI::ShaderStage>>& shaders,
									  const std::span<const TRAP::Graphics::Shader::Macro> userMacros)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

		TRAP::Graphics::RendererAPI::ShaderStage currentShaderStage = TRAP::Graphics::RendererAPI::ShaderStage::None;
		const std::vector<std::string> lines = TRAP::Utils::String::GetLines(glslSource);

		//Go through every line of the shader source
		for(usize i = 0; i < lines.size(); ++i)
		{
			//Optimization lines converted to lower case
			const std::string lowerLine = TRAP::Utils::String::ToLower(lines[i]);

			//Search for a shader type tag
			if(lowerLine.starts_with("#shader"))
			{
				//Detect shader type
				if (TRAP::Utils::String::Contains(lowerLine, "vertex"))
					currentShaderStage = TRAP::Graphics::RendererAPI::ShaderStage::Vertex;
				else if (TRAP::Utils::String::Contains(lowerLine, "fragment") ||
						TRAP::Utils::String::Contains(lowerLine, "pixel"))
					currentShaderStage = TRAP::Graphics::RendererAPI::ShaderStage::Fragment;
				else if (TRAP::Utils::String::Contains(lowerLine, "geometry"))
					currentShaderStage = TRAP::Graphics::RendererAPI::ShaderStage::Geometry;
				else if (TRAP::Utils::String::Contains(lowerLine, "tessellation"))
				{
					//Either Control or Evaluation
					if (TRAP::Utils::String::Contains(lowerLine, "control"))
						currentShaderStage = TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl;
					else if (TRAP::Utils::String::Contains(lowerLine, "evaluation"))
						currentShaderStage = TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation;
				}
				else if (TRAP::Utils::String::Contains(lowerLine, "compute"))
					currentShaderStage = TRAP::Graphics::RendererAPI::ShaderStage::Compute;
				//TODO RayTracing Shaders i.e. "RayGen" "AnyHit" "ClosestHit" "Miss" "Intersection" ("Callable")

				//Check for duplicate "#shader XXX" defines
				if (std::ranges::any_of(shaders,
					[currentShaderStage](const auto& element){return (element.second & currentShaderStage) != TRAP::Graphics::RendererAPI::ShaderStage::None;}))
				{
					TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "Found duplicate \"#shader\" define: ", lines[i]);
					return false;
				}

				shaders.emplace_back("", currentShaderStage);
			}
			else if(TRAP::Utils::String::Contains(lowerLine, "#version")) //Check for unnecessary "#version" define
				TP_WARN(TRAP::Log::ShaderGLSLPrefix, "Found tag: \"", lines[i], "\" this is unnecessary! Skipping line: ", i);
			else if(currentShaderStage != TRAP::Graphics::RendererAPI::ShaderStage::None) //Add shader code to detected shader stage
			{
				if(currentShaderStage == TRAP::Graphics::RendererAPI::ShaderStage::RayTracing)
				{
					TP_WARN(TRAP::Log::ShaderGLSLPrefix, "RayTracing shader support is WIP!");
					return false;
				}
				if(currentShaderStage == TRAP::Graphics::RendererAPI::ShaderStage::None)
				{
					TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "Unsupported shader type!");
					return false;
				}

				shaders.back().first.append(lines[i] + '\n');
			}
		}

		for(auto& [shaderCode, shaderStage] : shaders)
		{
			if (!TRAP::Utils::String::Contains(TRAP::Utils::String::ToLower(shaderCode), "main"))
			{
				TP_ERROR(TRAP::Log::ShaderGLSLPrefix, ShaderStageToString(shaderStage), " shader couldn't find \"main\" function!");
				return false;
			}

			std::string preprocessed;
			if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
			{
				//Found main function
				//Add GLSL version before any shader code &
				//Add Descriptor defines
				preprocessed = "#version 460 core\n";
				for(const TRAP::Graphics::Shader::Macro& macro : DefaultShaderMacrosVulkan)
					preprocessed += fmt::format("#define {} {}\n", macro.Definition, macro.Value);
			}
			else if(TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::Vulkan)
			{
				//Found main function
				//Add GLSL version before any shader code
				preprocessed = "#version 460 core\n";
			}

			for(const TRAP::Graphics::Shader::Macro& macro : userMacros)
				preprocessed += fmt::format("#define {} {}\n", macro.Definition, macro.Value);

			//Add preprocessed macros to shader
			shaderCode = preprocessed + shaderCode;
		}

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Parse a glslang::TShader object.
	/// @param shader glslang::TShader object.
	/// @return True if parsing was successful, false otherwise.
	[[nodiscard]] bool ParseGLSLang(glslang::TShader& shader)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

		const TBuiltInResource* const DefaultTBuiltInResource = GetDefaultResources();

		if(!shader.parse(DefaultTBuiltInResource, 460, true,
						static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
		{
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "Parsing failed: ");
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, shader.getInfoLog());
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, shader.getInfoDebugLog());

			return false;
		}

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Link a glslang::TShader to a glslang::TProgram object.
	/// @param shader glslang::TShader object.
	/// @param program glslang::TProgram object to link with.
	/// @return True if linking was successful, false otherwise.
	[[nodiscard]] bool LinkGLSLang(glslang::TShader& shader, glslang::TProgram& program)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

		program.addShader(&shader);

		if(!program.link(static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
		{
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "Linking failed: ");
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, program.getInfoLog());
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, program.getInfoDebugLog());

			return false;
		}

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Validate that the given shader stages are a valid combination.
	///
	/// 1. Checks if Rasterizer shaders are combined with Compute shader.
	/// 2. Checks if Rasterizer shaders are combined with RayTracing shaders
	/// 3. Checks if Compute shader is combined with RayTracing shaders.
	/// 4. Checks if Vertex and Fragment shaders are combined.
	/// @param shaders Shader stages to validate.
	/// @return True if validation was successful, false otherwise.
	[[nodiscard]] constexpr bool ValidateShaderStages(const std::vector<std::pair<std::string, TRAP::Graphics::RendererAPI::ShaderStage>>& shaders)
	{
		TRAP::Graphics::RendererAPI::ShaderStage combinedStages = TRAP::Graphics::RendererAPI::ShaderStage::None;
		for(const auto& [glsl, stage] : shaders)
			combinedStages |= stage;

		//Check if any Shader Stage is set
		if (TRAP::Graphics::RendererAPI::ShaderStage::None == combinedStages)
		{
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "No shader stage found!");
			return false;
		}

		//Check for "Normal" Shader Stages combined with Compute
		if((((TRAP::Graphics::RendererAPI::ShaderStage::Vertex & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) ||
			((TRAP::Graphics::RendererAPI::ShaderStage::Fragment & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) ||
			((TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) ||
			((TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) ||
			((TRAP::Graphics::RendererAPI::ShaderStage::Geometry & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None)) &&
			((TRAP::Graphics::RendererAPI::ShaderStage::Compute & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None))
		{
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "Rasterizer shader stages combined with compute stage!");
			return false;
		}

		//Check for "Normal" Shader Stages combined with RayTracing
		if((((TRAP::Graphics::RendererAPI::ShaderStage::Vertex & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) ||
			((TRAP::Graphics::RendererAPI::ShaderStage::Fragment & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) ||
			((TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) ||
			((TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) ||
			((TRAP::Graphics::RendererAPI::ShaderStage::Geometry & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None)) &&
			((TRAP::Graphics::RendererAPI::ShaderStage::RayTracing & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None))
		{
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "Rasterizer shader stages combined with ray tracing stage!");
			return false;
		}

		//Check for Compute Shader Stage combined with RayTracing
		if (((TRAP::Graphics::RendererAPI::ShaderStage::Compute & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) &&
			((TRAP::Graphics::RendererAPI::ShaderStage::RayTracing & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None))
		{
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "Compute shader stage combined with ray tracing stage!");
			return false;
		}

		//Check for Vertex Shader Stage & required Fragment/Pixel Shader Stage
		if(((TRAP::Graphics::RendererAPI::ShaderStage::Vertex & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) &&
		   (((TRAP::Graphics::RendererAPI::ShaderStage::Fragment & combinedStages)) == TRAP::Graphics::RendererAPI::ShaderStage::None))
		{
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "Only vertex shader stage provided! Missing fragment/pixel shader stage");
			return false;
		}
		//Check for Fragment/Pixel Shader Stage & required Vertex Shader Stage
		if(((TRAP::Graphics::RendererAPI::ShaderStage::Fragment & combinedStages) != TRAP::Graphics::RendererAPI::ShaderStage::None) &&
		   (((TRAP::Graphics::RendererAPI::ShaderStage::Vertex & combinedStages)) == TRAP::Graphics::RendererAPI::ShaderStage::None))
		{
			TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "Only fragment/pixel shader stage provided! Missing vertex shader stage");
			return false;
		}

		//Shader Stages should be valid
		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert a glslang::TProgram object to SPIRV binary data.
	/// @param stage Shader stage to convert.
	/// @param program glslang::TProgram object to convert.
	/// @return SPIRV binary data on success, empty vector otherwise.
	[[nodiscard]] std::vector<u32> ConvertToSPIRV(const TRAP::Graphics::RendererAPI::ShaderStage stage,
	                                              const glslang::TProgram& program)
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
			TP_ERROR(TRAP::Log::ShaderSPIRVPrefix, ShaderStageToString(stage), " shader: ", logger.getAllMessages());

		//TODO RayTracing shaders

		return SPIRV;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert GLSL shaders to SPIRV.
	/// @param shaders GLSL shader(s) to convert.
	/// @return RendererAPI::BinaryShaderDesc containing SPIRV binary data.
	[[nodiscard]] TRAP::Graphics::RendererAPI::BinaryShaderDesc ConvertGLSLToSPIRV(const std::vector<std::pair<std::string, TRAP::Graphics::RendererAPI::ShaderStage>>& shaders)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

		if(!GlslangInitialized)
		{
			if (!glslang::InitializeProcess())
				return{};
			GlslangInitialized = true;
		}

		TRAP::Graphics::RendererAPI::BinaryShaderDesc desc{};

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
			TP_DEBUG(TRAP::Log::ShaderGLSLPrefix, "Parsing ", ShaderStageToString(stage), " shader");
	#endif /*ENABLE_GRAPHICS_DEBUG*/
			if (!ParseGLSLang(glslShader))
				return{};

	#ifdef ENABLE_GRAPHICS_DEBUG
			TP_DEBUG(TRAP::Log::ShaderGLSLPrefix, "Linking ", ShaderStageToString(stage), " shader");
	#endif /*ENABLE_GRAPHICS_DEBUG*/
			glslang::TProgram program;
			if (!LinkGLSLang(glslShader, program))
				return{};

	#ifdef ENABLE_GRAPHICS_DEBUG
			TP_DEBUG(TRAP::Log::ShaderSPIRVPrefix, "Converting GLSL -> SPIR-V");
	#endif /*ENABLE_GRAPHICS_DEBUG*/
			const std::vector<u32> SPIRV = ConvertToSPIRV(stage, program);

			if((stage & TRAP::Graphics::RendererAPI::ShaderStage::Vertex) != TRAP::Graphics::RendererAPI::ShaderStage::None)
				desc.Vertex = TRAP::Graphics::RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			else if((stage & TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl) != TRAP::Graphics::RendererAPI::ShaderStage::None)
				desc.TessellationControl = TRAP::Graphics::RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			else if((stage & TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation) != TRAP::Graphics::RendererAPI::ShaderStage::None)
				desc.TessellationEvaluation = TRAP::Graphics::RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			else if((stage & TRAP::Graphics::RendererAPI::ShaderStage::Geometry) != TRAP::Graphics::RendererAPI::ShaderStage::None)
				desc.Geometry = TRAP::Graphics::RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			else if((stage & TRAP::Graphics::RendererAPI::ShaderStage::Fragment) != TRAP::Graphics::RendererAPI::ShaderStage::None)
				desc.Fragment = TRAP::Graphics::RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
			else if((stage & TRAP::Graphics::RendererAPI::ShaderStage::Compute) != TRAP::Graphics::RendererAPI::ShaderStage::None)
				desc.Compute = TRAP::Graphics::RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
		}

		return desc;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Load SPIRV binary data into RendererAPI::BinaryShaderDesc.
	/// @param SPIRV SPIRV binary data.
	/// @return RendererAPI::BinaryShaderDesc containing loaded SPIRV binary data.
	[[nodiscard]] TRAP::Graphics::RendererAPI::BinaryShaderDesc LoadSPIRV(std::vector<u8>& SPIRV)
	{
	#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(TRAP::Log::ShaderSPIRVPrefix, "Loading SPIRV");
	#endif /*ENABLE_GRAPHICS_DEBUG*/

		if(SPIRV.empty() || SPIRV.size() < (ShaderHeaderOffset + sizeof(SPIRVMagicNumber)))
			return {};

		const u32 magicNumber = TRAP::Utils::Memory::ConvertByte<u32>(SPIRV.data() + ShaderHeaderOffset);

		//Check endianness of byte stream
		if (magicNumber != SPIRVMagicNumber)
		{
			TRAP::Utils::Memory::SwapBytes(SPIRV.begin(), SPIRV.end()); //Convert endianness if needed
			if (magicNumber != SPIRVMagicNumber) //Recheck if SPIRV Magic Number is present
				return {};
		}

		TRAP::Graphics::RendererAPI::BinaryShaderDesc desc{};
		usize index = ShaderMagicNumber.size() + sizeof(u32);
		const u8 SPIRVSubShaderCount = SPIRV[index++];

		for(u32 i = 0; i < SPIRVSubShaderCount; ++i)
		{
			usize SPIRVSize = TRAP::Utils::Memory::ConvertByte<usize>(SPIRV.data() + NumericCast<isize>(index));
			index += sizeof(usize);

			const TRAP::Graphics::RendererAPI::ShaderStage stage = static_cast<TRAP::Graphics::RendererAPI::ShaderStage>(SPIRV[index++]);
			desc.Stages |= stage;

			const u32 spvMagicNumber = TRAP::Utils::Memory::ConvertByte<u32>(SPIRV.data() + NumericCast<isize>(index));
			if(spvMagicNumber != SPIRVMagicNumber || (SPIRV.size() - index) < SPIRVSize)
				return {};

			switch(stage)
			{
			case TRAP::Graphics::RendererAPI::ShaderStage::Vertex:
				desc.Vertex.ByteCode.resize(SPIRVSize);
				TRAP::Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.Vertex.ByteCode.begin());
				index += SPIRVSize * sizeof(u32);
				break;

			case TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl:
				desc.TessellationControl.ByteCode.resize(SPIRVSize);
				TRAP::Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.TessellationControl.ByteCode.begin());
				index += SPIRVSize * sizeof(u32);
				break;

			case TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation:
				desc.TessellationEvaluation.ByteCode.resize(SPIRVSize);
				TRAP::Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.TessellationEvaluation.ByteCode.begin());
				index += SPIRVSize * sizeof(u32);
				break;

			case TRAP::Graphics::RendererAPI::ShaderStage::Geometry:
				desc.Geometry.ByteCode.resize(SPIRVSize);
				TRAP::Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.Geometry.ByteCode.begin());
				index += SPIRVSize * sizeof(u32);
				break;

			case TRAP::Graphics::RendererAPI::ShaderStage::Fragment:
				desc.Fragment.ByteCode.resize(SPIRVSize);
				TRAP::Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.Fragment.ByteCode.begin());
				index += SPIRVSize * sizeof(u32);
				break;

			case TRAP::Graphics::RendererAPI::ShaderStage::Compute:
				desc.Compute.ByteCode.resize(SPIRVSize);
				TRAP::Utils::Memory::ConvertBytes(SPIRV.begin() + NumericCast<isize>(index), SPIRV.begin() + NumericCast<isize>(index) + NumericCast<isize>(SPIRVSize * sizeof(u32)), desc.Compute.ByteCode.begin());
				index += SPIRVSize * sizeof(u32);
				break;

			//case TRAP::Graphics::RendererAPI::ShaderStage::RayTracing:
				//TODO RayTracing

			case TRAP::Graphics::RendererAPI::ShaderStage::None:
				[[fallthrough]];
			case TRAP::Graphics::RendererAPI::ShaderStage::SHADER_STAGE_COUNT:
				[[fallthrough]];
			default:
				break;
			}
		}

		return desc;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check if the ending of the given path is a supported shader file ending.
	/// @param filePath File path to check.
	/// @return True if file ending is supported, false otherwise.
	[[nodiscard]] bool IsFileEndingSupported(const std::filesystem::path& filePath)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

		const auto fileEnding = TRAP::FileSystem::GetFileEnding(filePath);
		if(!fileEnding)
			return false;

		const std::string fileEndingLower = TRAP::Utils::String::ToLower(*fileEnding);
		bool supportedFormat = false;
		for(const auto& fmt : TRAP::Graphics::Shader::SupportedShaderFormatSuffixes)
		{
			if(fileEndingLower == fmt)
			{
				supportedFormat = true;
				break;
			}
		}

		if(!supportedFormat)
		{
			TP_ERROR(TRAP::Log::ShaderPrefix, "File: ", filePath, " suffix is not supported!");
			TP_WARN(TRAP::Log::ShaderPrefix, "Skipping unrecognized file");
			return false;
		}

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Shader pre initialization.
	///
	/// 1. Checks if file type is supported shader type.
	/// 2. Loads shader file from disk.
	/// 3. If GLSL convert to SPIRV else loads SPIRV code.
	/// 4. Check for invalid shader stages.
	/// @param name Name of the shader.
	/// @param filePath File path of the shader.
	/// @param userMacros Optional user provided macros.
	/// @param outShaderDesc Output binary shader description.
	/// @param outFailShader Optional Output used if pre initialization failed.
	/// @return True on successful pre initialization, false otherwise.
	/// If false outFailShader may be filled with a fail shader.
	[[nodiscard]] bool PreInit(const std::string& name, const std::filesystem::path& filePath,
	                           const std::vector<TRAP::Graphics::Shader::Macro>& userMacros,
							   TRAP::Graphics::RendererAPI::BinaryShaderDesc& outShaderDesc,
							   TRAP::Ref<TRAP::Graphics::Shader>& outFailShader)
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
				const auto loadedData = TRAP::FileSystem::ReadTextFile(filePath);
				if(loadedData)
					glslSource = *loadedData;
			}
			else
			{
				auto loadedData = TRAP::FileSystem::ReadFile(filePath);
				if(loadedData)
					SPIRVSource = std::move(*loadedData);
			}
		}

		if(isSPIRV)
		{
			if(!filePath.empty() && SPIRVSource.empty())
				TP_ERROR(TRAP::Log::ShaderSPIRVPrefix, "Couldn't load shader: \"", name, "\"!");
		}
		else
		{
			if (!filePath.empty() && glslSource.empty())
				TP_ERROR(TRAP::Log::ShaderGLSLPrefix, "Couldn't load shader: \"", name, "\"!");
		}

		if ((glslSource.empty() && !isSPIRV) || (SPIRVSource.empty() && isSPIRV))
		{
			TP_WARN(TRAP::Log::ShaderPrefix, "Skipping unrecognized file ", filePath);
			return false;
		}

		if (!isSPIRV)
		{
			std::vector<std::pair<std::string, TRAP::Graphics::RendererAPI::ShaderStage>> shaders{};
			if (!PreProcessGLSL(glslSource, shaders, userMacros))
			{
				TP_WARN(TRAP::Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader");
				if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
					outFailShader = TRAP::MakeRef<TRAP::Graphics::API::VulkanShader>(name, filePath, userMacros);

				return false;
			}
			if (!ValidateShaderStages(shaders))
			{
				TP_WARN(TRAP::Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback shader");
				if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
					outFailShader = TRAP::MakeRef<TRAP::Graphics::API::VulkanShader>(name, filePath, userMacros);

				return false;
			}

			outShaderDesc = ConvertGLSLToSPIRV(shaders);
		}
		else
			outShaderDesc = LoadSPIRV(SPIRVSource);

		if (outShaderDesc.Stages == TRAP::Graphics::RendererAPI::ShaderStage::None)
		{
			if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
				outFailShader = TRAP::MakeRef<TRAP::Graphics::API::VulkanShader>(name, filePath, userMacros);

			return false;
		}

		return true;
	}
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
		if (!PreProcessGLSL(glslSource, shaders, m_macros) || !ValidateShaderStages(shaders))
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
																		               const std::vector<Macro>& userMacros)
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
                                                                                       const std::vector<Macro>& userMacros)
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
																		                 const std::vector<Macro>& userMacros)
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
                               const std::vector<Macro>& userMacros, const std::filesystem::path& filepath)
	: m_name(std::move(name)), m_filepath(filepath), m_shaderStages(stages), m_macros(userMacros), m_valid(valid)
{
}
