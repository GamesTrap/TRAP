#ifndef _TRAP_VULKANSHADER_H_
#define _TRAP_VULKANSHADER_H_

#include "Graphics/Shaders/Shader.h"

namespace TRAP::Graphics::API
{	
	class VulkanShader final : public Shader
	{
	public:
		VulkanShader(std::string name, std::string_view source);
		VulkanShader(std::string name, const std::vector<uint32_t>& source);
		VulkanShader(std::string name, std::string_view VSSource, std::string_view FSSource, std::string_view GSSource, std::string_view TCSSource, std::string_view TESSource, std::string_view CSSource);
		VulkanShader(const VulkanShader&) = default;
		VulkanShader& operator=(const VulkanShader&) = default;
		VulkanShader(VulkanShader&&) = default;
		VulkanShader& operator=(VulkanShader&&) = default;
		~VulkanShader();
		
		void Shutdown() const;

		void Bind() const override;
		void Unbind() const override;

	private:
	};
}

#endif /*_TRAP_VULKANSHADER_H_*/