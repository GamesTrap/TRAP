#include "TRAPPCH.h"
#include "Shader.h"


#include "DummyShader.h"
#include "VFS/VFS.h"
#include "Graphics/API/Vulkan/Objects/VulkanShader.h"
#include "Utils/String/String.h"

bool TRAP::Graphics::Shader::s_glslangInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader::Shader()
	: m_name(), m_filepath(), m_shaderStages()
{
	TRAP::Graphics::RendererAPI::AddShaderToGraphicsRootSignature(this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Shader::~Shader()
{
	TRAP::Graphics::RendererAPI::RemoveShaderFromGraphicsRootSignature(this);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::Shader::GetName() const
{
	TP_PROFILE_FUNCTION();

	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::Shader::GetFilePath() const
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

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::string& name, const std::string& filePath, const std::vector<Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN(Log::ShaderPrefix, "Name is empty! Using Filename as Shader Name!");
		return CreateFromFile(filePath, userMacros);
	}
	
	std::string glslSource;
	std::string VFSFilePath;
	bool isSPIRV = false;
	RendererAPI::BinaryShaderDesc desc{};
	std::vector<uint32_t> SPIRVSource{};
	if (!filePath.empty())
	{
		if (Utils::String::ToLower(Utils::String::GetSuffix(filePath)) != "spirv" && Utils::String::ToLower(Utils::String::GetSuffix(filePath)) != "shader")
		{
			TP_ERROR(Log::ShaderPrefix, "File: \"", filePath, "\" suffix is not \"*.spirv\" or \"*.shader\"!");
			TP_WARN(Log::ShaderPrefix, "Shader using fallback Shader: \"Fallback\"");
			return nullptr;
		}
		
		isSPIRV = CheckSPIRVMagicNumber(filePath);
		
		if (!isSPIRV)
			glslSource = VFS::ReadTextFile(filePath, true);
		else
			SPIRVSource = Convert8To32(VFS::ReadFile(filePath, true));

		VFSFilePath = VFS::MakeVirtualPathCompatible(filePath);
	}

	if(isSPIRV)
	{
		if(!filePath.empty() && SPIRVSource.empty())
			TP_ERROR(Log::ShaderSPIRVPrefix, "Couldn't load Shader: \"", name, "\"!");
	}
	else
	{
		if (!filePath.empty() && glslSource.empty())
			TP_ERROR(Log::ShaderGLSLPrefix, "Couldn't load Shader: \"", name, "\"!");
	}

	if ((glslSource.empty() && !isSPIRV) || (SPIRVSource.empty() && isSPIRV))
	{
		TP_WARN(Log::ShaderPrefix, "Shader using fallback Shader: \"Fallback\"");
		return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);
	}

	if (!isSPIRV)
	{
		RendererAPI::ShaderStage shaderStages = RendererAPI::ShaderStage::None;
		std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
		if (!PreProcessGLSL(glslSource, shaders, shaderStages, userMacros))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback Shader: \"Fallback\"");
			return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);
		}
		if (!ValidateShaderStages(shaderStages))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback Shader: \"Fallback\"");
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
			
		result->m_filepath = VFSFilePath;
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

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::string& filePath, const std::vector<Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	std::string glslSource;
	std::string VFSFilePath;
	std::string name;
	bool isSPIRV = false;
	RendererAPI::BinaryShaderDesc desc{};
	std::vector<uint32_t> SPIRVSource{};
	if (!filePath.empty())
	{
		if (Utils::String::ToLower(Utils::String::GetSuffix(filePath)) != "spirv" && Utils::String::ToLower(Utils::String::GetSuffix(filePath)) != "shader")
		{
			TP_ERROR(Log::ShaderPrefix, "File: \"", filePath, "\" suffix is not \"*.spirv\" or \"*.shader\"!");
			TP_WARN(Log::ShaderPrefix, "Shader using fallback Shader: \"Fallback\"");
			return nullptr;
		}
		
		isSPIRV = CheckSPIRVMagicNumber(filePath);

		if (!isSPIRV)
			glslSource = VFS::ReadTextFile(filePath, true);
		else
			SPIRVSource = Convert8To32(VFS::ReadFile(filePath, true));
		
		VFSFilePath = VFS::MakeVirtualPathCompatible(filePath);
		name = VFS::GetFileName(VFSFilePath);
	}

	if (isSPIRV)
	{
		if (!filePath.empty() && SPIRVSource.empty())
			TP_ERROR(Log::ShaderSPIRVPrefix, "Couldn't load Shader: \"", name, "\"!");
	}
	else
	{
		if (!filePath.empty() && glslSource.empty())
			TP_ERROR(Log::ShaderGLSLPrefix, "Couldn't load Shader: \"", name, "\"!");
	}

	if ((glslSource.empty() && !isSPIRV) || (SPIRVSource.empty() && isSPIRV))
	{
		TP_WARN(Log::ShaderPrefix, "Shader using fallback Shader: \"Fallback\"");
		return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);
	}

	if(!isSPIRV)
	{
		RendererAPI::ShaderStage shaderStages = RendererAPI::ShaderStage::None;
		std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
		if (!PreProcessGLSL(glslSource, shaders, shaderStages, userMacros))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback Shader: \"Fallback\"");
			return TRAP::MakeScope<TRAP::Graphics::DummyShader>(name, filePath);
		}
		if (!ValidateShaderStages(shaderStages))
		{
			TP_WARN(Log::ShaderGLSLPrefix, "Shader: \"", name, "\" using fallback Shader: \"Fallback\"");
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

		result->m_filepath = VFSFilePath;
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

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromSource(const std::string& name, const std::string& glslSource, const std::vector<Macro>* userMacros)
{
	TP_PROFILE_FUNCTION();

	std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> shaders{};
	RendererAPI::ShaderStage shaderStages = RendererAPI::ShaderStage::None;
	if(!PreProcessGLSL(glslSource, shaders, shaderStages, userMacros))
	{
		TP_WARN(Log::ShaderPrefix, "Shader: \"", name, "\" using fallback Shader: \"Fallback\"");
		return nullptr;
	}
	if(!ValidateShaderStages(shaderStages))
	{
		TP_WARN(Log::ShaderPrefix, "Shader: \"", name, "\" using fallback Shader: \"Fallback\"");
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

bool TRAP::Graphics::Shader::CheckSPIRVMagicNumber(const std::string_view filePath)
{
	//Check SPIRV Magic Number
	std::filesystem::path physicalPath;
	if (VFS::ResolveReadPhysicalPath(filePath, physicalPath, true))
	{
		if (VFS::FileOrFolderExists(physicalPath, true))
		{
			std::ifstream file(physicalPath, std::ios::binary);
			if (file.is_open())
			{
				//SPIRV Magic Number:
				//0x07230203

				uint32_t magicNumber = 0;
				file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //Number of SubShaders
				file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //Size of the current SubShader in uint32_ts
				file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //Type of the current SubShader
				file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //SPIRV Magic Number
				file.close();

				if (magicNumber == 0x07230203)
					return true;

				return false;
			}

			TP_ERROR(Log::FileSystemPrefix, "Could not open File: ", physicalPath);
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint32_t> TRAP::Graphics::Shader::Convert8To32(const std::vector<uint8_t>& source)
{
	//BUG Depends on Endianness ?!
	std::vector<uint32_t> data{};
	data.resize(source.size() / 4);

	uint32_t j = 0;
	for(uint32_t i = 0; i < source.size(); i += 4)
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
	for(uint32_t i = 0; i < lines.size(); ++i)
	{
		//Optimization lines converted to lower case
		std::string lowerLine = Utils::String::ToLower(lines[i]);
		
		//Search for a shader type tag
		if(Utils::String::StartsWith(lowerLine, "#shader"))
		{
			//Detect shader type
			if (Utils::String::FindToken(lowerLine, "vertex"))
			{
				currentShaderStage = RendererAPI::ShaderStage::Vertex;
			}
			else if (Utils::String::FindToken(lowerLine, "fragment") ||
				Utils::String::FindToken(lowerLine, "pixel"))
			{
				currentShaderStage = RendererAPI::ShaderStage::Fragment;
			}
			else if (Utils::String::FindToken(lowerLine, "geometry"))
			{
				currentShaderStage = RendererAPI::ShaderStage::Geometry;
			}
			else if (Utils::String::FindToken(lowerLine, "tessellation"))
			{
				//Either Control or Evaluation
				if (Utils::String::FindToken(lowerLine, "control"))
				{
					currentShaderStage = RendererAPI::ShaderStage::TessellationControl;
				}
				else if (Utils::String::FindToken(lowerLine, "evaluation"))
				{
					currentShaderStage = RendererAPI::ShaderStage::TessellationEvaluation;
				}
			}
			else if (Utils::String::FindToken(lowerLine, "compute"))
			{
				currentShaderStage = RendererAPI::ShaderStage::Compute;
			}
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
			TP_WARN(Log::ShaderGLSLPrefix, "Found Tag: \"", lines[i], "\" this is unnecessary! Skipping Line: ", i);
		else if(currentShaderStage != RendererAPI::ShaderStage::None) //Add shader code to detected shader stage
		{
			switch(currentShaderStage)
			{
			case RendererAPI::ShaderStage::Vertex:
				shaders[0].append(lines[i] + '\n');
				break;

			case RendererAPI::ShaderStage::TessellationControl:
			//case RendererAPI::ShaderStage::Hull:
				shaders[1].append(lines[i] + '\n');
				break;

			case RendererAPI::ShaderStage::TessellationEvaluation:
			//case RendererAPI::ShaderStage::Domain:
				shaders[2].append(lines[i] + '\n');
				break;

			case RendererAPI::ShaderStage::Geometry:
				shaders[3].append(lines[i] + '\n');
				break;

			case RendererAPI::ShaderStage::Fragment:
				shaders[4].append(lines[i] + '\n');
				break;

			case RendererAPI::ShaderStage::Compute:
				shaders[5].append(lines[i] + '\n');
				break;

			case RendererAPI::ShaderStage::RayTracing:
				TP_WARN(Log::ShaderGLSLPrefix, "RayTracing Shader support is WIP!");
				return false;
				break;
				
			case RendererAPI::ShaderStage::AllGraphics:
			case RendererAPI::ShaderStage::SHADER_STAGE_COUNT:
			case RendererAPI::ShaderStage::None:
			default:
				TP_ERROR(Log::ShaderGLSLPrefix, "Unsupported Shader Type!");
				break;
			}
		}
	}

	for(uint32_t i = 0; i < shaders.size(); i++)
	{
		if (Utils::String::ToLower(shaders[i]).find("main") == std::string::npos)
		{
			switch (i)
			{
			case 0:
				if (static_cast<uint32_t>(RendererAPI::ShaderStage::Vertex & shaderStages))
				{
					TP_ERROR(Log::ShaderGLSLPrefix, "Vertex Shader Couldn't find \"main\" function!");
					return false;
				}
				break;

			case 1:
				if (static_cast<uint32_t>(RendererAPI::ShaderStage::TessellationControl & shaderStages))
				{
					TP_ERROR(Log::ShaderGLSLPrefix, "TessellationControl Shader Couldn't find \"main\" function!");
					return false;
				}
				break;

			case 2:
				if (static_cast<uint32_t>(RendererAPI::ShaderStage::TessellationEvaluation & shaderStages))
				{
					TP_ERROR(Log::ShaderGLSLPrefix, "TessellationEvaluation Shader Couldn't find \"main\" function!");
					return false;
				}
				break;

			case 3:
				if (static_cast<uint32_t>(RendererAPI::ShaderStage::Geometry & shaderStages))
				{
					TP_ERROR(Log::ShaderGLSLPrefix, "Geometry Shader Couldn't find \"main\" function!");
					return false;
				}
				break;

			case 4:
				if (static_cast<uint32_t>(RendererAPI::ShaderStage::Fragment & shaderStages))
				{
					TP_ERROR(Log::ShaderGLSLPrefix, "Fragment Shader Couldn't find \"main\" function!");
					return false;
				}
				break;

			case 5:
				if (static_cast<uint32_t>(RendererAPI::ShaderStage::Compute & shaderStages))
				{
					TP_ERROR(Log::ShaderGLSLPrefix, "Compute Shader Couldn't find \"main\" function!");
					return false;
				}
				break;

			default:
				break;
			}
		}

		std::string preprocessed = "";
		if (!shaders[i].empty() && TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		{
			//Found main function
			//Add GLSL version before any shader code &
			//Add Descriptor defines
			preprocessed = "#version 460 core\n";
			for(const Macro& macro : s_defaultShaderMacrosVulkan)
				preprocessed += "#define " + macro.Definition + " " + macro.Value + '\n';
		}
		else if(!shaders[i].empty() && TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::Vulkan)
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

TRAP::Scope<glslang::TShader> TRAP::Graphics::Shader::PreProcessGLSLForConversion(const char* source,
	const RendererAPI::ShaderStage stage,
	std::string& preProcessedSource)
{
	TRAP::Scope<glslang::TShader> shader;

	if(stage == RendererAPI::ShaderStage::Vertex)
	{
		shader = TRAP::MakeScope<glslang::TShader>(EShLangVertex);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangVertex, glslang::EShClientVulkan, 460);
	}
	else if (stage == RendererAPI::ShaderStage::TessellationControl)
	{
		shader = TRAP::MakeScope<glslang::TShader>(EShLangTessControl);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangTessControl, glslang::EShClientVulkan, 460);
	}
	else if (stage == RendererAPI::ShaderStage::TessellationEvaluation)
	{
		shader = TRAP::MakeScope<glslang::TShader>(EShLangTessEvaluation);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangTessEvaluation, glslang::EShClientVulkan, 460);
	}
	else if (stage == RendererAPI::ShaderStage::Geometry)
	{
		shader = TRAP::MakeScope<glslang::TShader>(EShLangGeometry);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangGeometry, glslang::EShClientVulkan, 460);
	}
	else if (stage == RendererAPI::ShaderStage::Fragment)
	{
		shader = TRAP::MakeScope<glslang::TShader>(EShLangFragment);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangFragment, glslang::EShClientVulkan, 460);
	}
	else if (stage == RendererAPI::ShaderStage::Compute)
	{
		shader = TRAP::MakeScope<glslang::TShader>(EShLangCompute);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangCompute, glslang::EShClientVulkan, 460);
	}
	//TODO RayTracing

	if(shader)
	{
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

		if(!shader->preprocess(&DefaultTBuiltInResource,
			460,
			ECoreProfile,
			true,
			true,
			static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules),
			&preProcessedSource,
			includer))
		{
			TP_ERROR(Log::ShaderSPIRVPrefix, "GLSL -> SPIR-V conversion preprocessing failed!");
			TP_ERROR(Log::ShaderSPIRVPrefix, shader->getInfoDebugLog());
			TP_ERROR(Log::ShaderSPIRVPrefix, shader->getInfoDebugLog());
			
			return nullptr;
		}

		return shader;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Shader::ParseGLSLang(glslang::TShader* shader)
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

	if(!shader->parse(&DefaultTBuiltInResource, 460, true, static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
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
		TP_ERROR(Log::ShaderGLSLPrefix, "No Shader Stage found!");
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
		TP_ERROR(Log::ShaderGLSLPrefix, "Rasterizer Shader Stages combined with Compute stage!");
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
		TP_ERROR(Log::ShaderGLSLPrefix, "Rasterizer Shader Stages combined with RayTracing stage!");
		return false;
	}

	//Check for Compute Shader Stage combined with RayTracing
	if (static_cast<uint32_t>(RendererAPI::ShaderStage::Compute & shaderStages) &&
		static_cast<uint32_t>(RendererAPI::ShaderStage::RayTracing & shaderStages))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Compute Shader Stage combined with RayTracing stage!");
		return false;
	}

	//Check for Vertex Shader Stage & required Fragment/Pixel Shader Stage
	if(static_cast<uint32_t>(RendererAPI::ShaderStage::Vertex & shaderStages) &&
	   !(static_cast<uint32_t>(RendererAPI::ShaderStage::Fragment & shaderStages)))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Only Vertex Shader Stage provided! Missing Fragment/Pixel Shader Stage");
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
		if(!shaders[i].empty())
		{
			glslang::TProgram program;
			
			std::string preProcessedSource;

			switch(i)
			{
			case 0:
			{
				TP_DEBUG(Log::ShaderGLSLPrefix, "Pre-Processing Vertex Shader");
				glslShaders[0] = PreProcessGLSLForConversion(shaders[0].data(), RendererAPI::ShaderStage::Vertex, preProcessedSource);
				if (preProcessedSource.empty())
					return{};
				
				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[0]->setStrings(&preProcessedCStr, 1);
				
				TP_DEBUG(Log::ShaderGLSLPrefix, "Parsing Vertex Shader");
				if (!ParseGLSLang(glslShaders[0].get()))
					return{};
				
				TP_DEBUG(Log::ShaderGLSLPrefix, "Linking Vertex Shader");
				if (!LinkGLSLang(glslShaders[0].get(), program))
					return{};

				TP_DEBUG(Log::ShaderSPIRVPrefix, "Converting GLSL -> SPIR-V");
				const std::vector<uint32_t> SPIRV = ConvertToSPIRV(glslShaders[0].get(), RendererAPI::ShaderStage::Vertex, program);
				
				desc.Vertex = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
				break;
			}

			case 1:
			{
				TP_DEBUG(Log::ShaderGLSLPrefix, "Pre-Processing TessellationControl Shader");
				glslShaders[1] = PreProcessGLSLForConversion(shaders[1].data(), RendererAPI::ShaderStage::TessellationControl, preProcessedSource);
				if (preProcessedSource.empty())
					return{};

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[1]->setStrings(&preProcessedCStr, 1);

				TP_DEBUG(Log::ShaderGLSLPrefix, "Parsing TessellationControl Shader");
				if (!ParseGLSLang(glslShaders[1].get()))
					return{};

				TP_DEBUG(Log::ShaderGLSLPrefix, "Linking TessellationControl Shader");
				if (!LinkGLSLang(glslShaders[1].get(), program))
					return{};

				TP_DEBUG(Log::ShaderSPIRVPrefix, "Converting GLSL -> SPIR-V");
				const std::vector<uint32_t> SPIRV = ConvertToSPIRV(glslShaders[1].get(), RendererAPI::ShaderStage::TessellationControl, program);

				desc.TessellationControl = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
				break;
			}

			case 2:
			{
				TP_DEBUG(Log::ShaderGLSLPrefix, "Pre-Processing TessellationEvaluation Shader");
				glslShaders[2] = PreProcessGLSLForConversion(shaders[2].data(), RendererAPI::ShaderStage::TessellationEvaluation, preProcessedSource);
				if (preProcessedSource.empty())
					return{};

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[2]->setStrings(&preProcessedCStr, 1);

				TP_DEBUG(Log::ShaderGLSLPrefix, "Parsing TessellationEvaluation Shader");
				if (!ParseGLSLang(glslShaders[2].get()))
					return{};

				TP_DEBUG(Log::ShaderGLSLPrefix, "Linking TessellationEvaluation Shader");
				if (!LinkGLSLang(glslShaders[2].get(), program))
					return{};

				TP_DEBUG(Log::ShaderSPIRVPrefix, "Converting GLSL -> SPIR-V");
				const std::vector<uint32_t> SPIRV = ConvertToSPIRV(glslShaders[2].get(), RendererAPI::ShaderStage::TessellationEvaluation, program);

				desc.TessellationEvaluation = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
				break;
			}

			case 3:
			{
				TP_DEBUG(Log::ShaderGLSLPrefix, "Pre-Processing Geometry Shader");
				glslShaders[3] = PreProcessGLSLForConversion(shaders[3].data(), RendererAPI::ShaderStage::Geometry, preProcessedSource);
				if (preProcessedSource.empty())
					return{};

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[3]->setStrings(&preProcessedCStr, 1);

				TP_DEBUG(Log::ShaderGLSLPrefix, "Parsing Geometry Shader");
				if (!ParseGLSLang(glslShaders[3].get()))
					return{};

				TP_DEBUG(Log::ShaderGLSLPrefix, "Linking Geometry Shader");
				if (!LinkGLSLang(glslShaders[3].get(), program))
					return{};

				TP_DEBUG(Log::ShaderSPIRVPrefix, "Converting GLSL -> SPIR-V");
				const std::vector<uint32_t> SPIRV = ConvertToSPIRV(glslShaders[3].get(), RendererAPI::ShaderStage::Geometry, program);

				desc.Geometry = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
				break;
			}

			case 4:
			{
				TP_DEBUG(Log::ShaderGLSLPrefix, "Pre-Processing Fragment Shader");
				glslShaders[4] = PreProcessGLSLForConversion(shaders[4].data(), RendererAPI::ShaderStage::Fragment, preProcessedSource);
				if (preProcessedSource.empty())
					return{};

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[4]->setStrings(&preProcessedCStr, 1);

				TP_DEBUG(Log::ShaderGLSLPrefix, "Parsing Fragment Shader");
				if (!ParseGLSLang(glslShaders[4].get()))
					return{};

				TP_DEBUG(Log::ShaderGLSLPrefix, "Linking Fragment Shader");
				if (!LinkGLSLang(glslShaders[4].get(), program))
					return{};

				TP_DEBUG(Log::ShaderSPIRVPrefix, "Converting GLSL -> SPIR-V");
				const std::vector<uint32_t> SPIRV = ConvertToSPIRV(glslShaders[4].get(), RendererAPI::ShaderStage::Fragment, program);

				desc.Fragment = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
				break;
			}

			case 5:
			{
				TP_DEBUG(Log::ShaderGLSLPrefix, "Pre-Processing Compute Shader");
				glslShaders[5] = PreProcessGLSLForConversion(shaders[5].data(), RendererAPI::ShaderStage::Compute, preProcessedSource);
				if (preProcessedSource.empty())
					return{};

				const char* preProcessedCStr = preProcessedSource.c_str();
				glslShaders[5]->setStrings(&preProcessedCStr, 1);

				TP_DEBUG(Log::ShaderGLSLPrefix, "Parsing Compute Shader");
				if (!ParseGLSLang(glslShaders[5].get()))
					return{};

				TP_DEBUG(Log::ShaderGLSLPrefix, "Linking Compute Shader");
				if (!LinkGLSLang(glslShaders[5].get(), program))
					return{};

				TP_DEBUG(Log::ShaderSPIRVPrefix, "Converting GLSL -> SPIR-V");
				const std::vector<uint32_t> SPIRV = ConvertToSPIRV(glslShaders[5].get(), RendererAPI::ShaderStage::Compute, program);

				desc.Compute = RendererAPI::BinaryShaderStageDesc{ SPIRV, "main" };
				break;
			}

				//TODO RayTracing Shaders
				
			default:
				break;
			}
			
		}
	}

	return desc;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint32_t> TRAP::Graphics::Shader::ConvertToSPIRV(glslang::TShader* shader, const RendererAPI::ShaderStage stage, glslang::TProgram& program)
{
	std::vector<uint32_t> SPIRV{};
	
	if(shader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
#ifdef TRAP_DEBUG
		spvOptions.validate = true;
		spvOptions.disableOptimizer = true;
#else
		spvOptions.disableOptimizer = false;
		spvOptions.optimizeSize = true;
#endif
		switch(stage)
		{
		case RendererAPI::ShaderStage::Vertex:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangVertex), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				TP_ERROR(Log::ShaderSPIRVPrefix, "Vertex Shader: ", logger.getAllMessages());
			break;

		case RendererAPI::ShaderStage::TessellationControl:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangTessControl), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				TP_ERROR(Log::ShaderSPIRVPrefix, "TessellationControl Shader: ", logger.getAllMessages());
			break;

		case RendererAPI::ShaderStage::TessellationEvaluation:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangTessEvaluation), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				TP_ERROR(Log::ShaderSPIRVPrefix, "TessellationEvaluation Shader: ", logger.getAllMessages());
			break;

		case RendererAPI::ShaderStage::Geometry:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangGeometry), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				TP_ERROR(Log::ShaderSPIRVPrefix, "Geometry Shader: ", logger.getAllMessages());
			break;

		case RendererAPI::ShaderStage::Fragment:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangFragment), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				TP_ERROR(Log::ShaderSPIRVPrefix, "Fragment Shader: ", logger.getAllMessages());
			break;

		case RendererAPI::ShaderStage::Compute:
			glslang::GlslangToSpv(*program.getIntermediate(EShLangCompute), SPIRV, &logger, &spvOptions);
			if (logger.getAllMessages().length() > 0)
				TP_ERROR(Log::ShaderSPIRVPrefix, "Compute Shader: ", logger.getAllMessages());
			break;

			//TODO RayTracing shaders
			
		default:
			break;
		}
	}

	return SPIRV;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::BinaryShaderDesc TRAP::Graphics::Shader::LoadSPIRV(const std::vector<uint32_t>& SPIRV)
{
	TP_DEBUG(Log::ShaderSPIRVPrefix, "Loading SPIRV");
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
			desc.TessellationControl.ByteCode = std::vector<uint32_t>(SPIRV.begin() + index, SPIRV.begin() + index + SPIRVSize);
			index += SPIRVSize;
			break;

		case RendererAPI::ShaderStage::TessellationEvaluation:
			desc.TessellationEvaluation.ByteCode = std::vector<uint32_t>(SPIRV.begin() + index, SPIRV.begin() + index + SPIRVSize);
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