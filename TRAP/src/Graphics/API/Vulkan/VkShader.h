#ifndef _TRAP_VULKANSHADER_H__
#define _TRAP_VULKANSHADER_H__

#include "Graphics/Shaders/Shader.h"

namespace TRAP::Graphics::API
{	
	class VkShader final : public Shader
	{
	public:
		VkShader(std::string name, std::string_view source);
		VkShader(std::string name, const std::vector<uint32_t>& source);
		VkShader(std::string name, std::string_view VSSource, std::string_view FSSource, std::string_view GSSource, std::string_view TCSSource, std::string_view TESSource, std::string_view CSSource);
		VkShader(const VkShader&) = default;
		VkShader& operator=(const VkShader&) = default;
		VkShader(VkShader&&) = default;
		VkShader& operator=(VkShader&&) = default;
		~VkShader();
		
		void Shutdown() const;

		void Bind() const override;
		void Unbind() const override;

	private:
	};
}

#endif /*_TRAP_VULKANSHADER_H__*/