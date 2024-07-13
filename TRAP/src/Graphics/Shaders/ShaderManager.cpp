#include "TRAPPCH.h"
#include "ShaderManager.h"

#include "Embed.h"
#include "FileSystem/FileSystem.h"

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	TRAP::Utils::UnorderedStringMap<TRAP::Ref<TRAP::Graphics::Shader>> Shaders{};
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::LoadFile(const RendererAPI::ShaderType shaderType,
                                                                          const std::filesystem::path& filepath,
    																      const std::vector<Shader::Macro>& userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Ref<Shader> shader = Shader::CreateFromFile(shaderType, filepath, userMacros);

	if(!shader)
	{
		switch(shaderType)
		{
		case RendererAPI::ShaderType::Graphics:
			return Get(RendererAPI::ShaderType::Graphics, "FallbackGraphics");

		case RendererAPI::ShaderType::Compute:
			return Get(RendererAPI::ShaderType::Compute, "FallbackCompute");
		}
	}

	const std::string name = shader->GetName();

	Add(shader);

	return shader;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::LoadFile(const RendererAPI::ShaderType shaderType,
                                                                          const std::string& name,
																          const std::filesystem::path& filepath,
																          const std::vector<Shader::Macro>& userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Ref<Shader> shader = Shader::CreateFromFile(shaderType, name, filepath, userMacros);

	if(!shader)
	{
		switch(shaderType)
		{
		case RendererAPI::ShaderType::Graphics:
			return Get(RendererAPI::ShaderType::Graphics, "FallbackGraphics");

		case RendererAPI::ShaderType::Compute:
			return Get(RendererAPI::ShaderType::Compute, "FallbackCompute");
		}
	}

	Add(shader);

	return shader;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::LoadSource(const RendererAPI::ShaderType shaderType,
	                                                                        const std::string& name,
														              		const std::string& glslSource,
																      		const std::vector<Shader::Macro>& userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Ref<Shader> shader = Shader::CreateFromSource(shaderType, name, glslSource, userMacros);

	if(!shader)
	{
		switch(shaderType)
		{
		case RendererAPI::ShaderType::Graphics:
			return Get(RendererAPI::ShaderType::Graphics, "FallbackGraphics");

		case RendererAPI::ShaderType::Compute:
			return Get(RendererAPI::ShaderType::Compute, "FallbackCompute");
		}
	}

	Add(shader);

	return shader;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Add(const Ref<Shader>& shader)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(shader == nullptr)
	{
		TRAP_ASSERT(false, "ShaderManager::Add(): Provided shader is nullptr!");
		return;
	}

	if(!Contains(shader->GetName()))
		Shaders[shader->GetName()] = shader;
	else
		TP_ERROR(Log::ShaderManagerPrefix, "Shader with name: \"", shader->GetName(), "\" already exists! Ignoring new shader");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::Remove(const Ref<Shader>& shader)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(shader == nullptr)
	{
		TRAP_ASSERT(false, "ShaderManager::Remove(): Provided shader is nullptr!");
		return nullptr;
	}

	if(Shaders.erase(shader->GetName()) > 0)
		return shader;

	TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: \"", shader->GetName(), "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::Remove(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(const auto it = Shaders.find(name); it != Shaders.end())
	{
		Ref<Shader> shader = it->second;
		Shaders.erase(it);
		return shader;
	}

	TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::Get(const RendererAPI::ShaderType shaderType,
                                                                                   const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(const auto it = Shaders.find(name); it != Shaders.end())
		return it->second;

	TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with name: ", name, "!");
	TP_WARN(Log::ShaderManagerPrefix, "Using fallback shader!");

	//Should always be available as a fallback
	switch(shaderType)
	{
	case RendererAPI::ShaderType::Graphics:
		return Get(RendererAPI::ShaderType::Graphics, "FallbackGraphics");

	case RendererAPI::ShaderType::Compute:
		return Get(RendererAPI::ShaderType::Compute, "FallbackCompute");
	}

	TRAP_ASSERT(false, "ShaderManager::Get(): Failed to retrieve Fallback Shader, unknown ShaderType provided!");
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::GetGraphics(const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Get(RendererAPI::ShaderType::Graphics, name);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::GetCompute(const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Get(RendererAPI::ShaderType::Compute, name);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> TRAP::Graphics::ShaderManager::GetByPath(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	const auto it = std::ranges::find_if(Shaders, [&path](const auto& e)
	{
		const auto& [name, shader] = e;
		return !shader->GetFilePath().empty() && FileSystem::IsEquivalent(path, shader->GetFilePath());
	});

	if(it != Shaders.end())
		return it->second;

	TP_ERROR(Log::ShaderManagerPrefix, "Couldn't find shader with path: ", path, "!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Utils::UnorderedStringMap<TRAP::Ref<TRAP::Graphics::Shader>>& TRAP::Graphics::ShaderManager::GetShaders() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return Shaders;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Clean()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Shaders.clear();

	//Make sure that fallback shaders are always available
	Graphics::ShaderManager::LoadSource(TRAP::Graphics::RendererAPI::ShaderType::Graphics, "FallbackGraphics", std::string(Embed::FallbackGraphicsShader));
	Graphics::ShaderManager::LoadSource(TRAP::Graphics::RendererAPI::ShaderType::Compute,"FallbackCompute", std::string(Embed::FallbackComputeShader));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::ShaderManager::Contains(const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Shaders.contains(name);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::ShaderManager::ContainsByPath(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return std::ranges::any_of(Shaders, [&path](const auto& element)
	{
		return !element.second->GetFilePath().empty() && FileSystem::IsEquivalent(element.second->GetFilePath(), path);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::ShaderManager::Shutdown() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Shaders.clear();
}
