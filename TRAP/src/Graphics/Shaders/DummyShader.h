#ifndef _TRAP_DUMMYSHADER_H_
#define _TRAP_DUMMYSHADER_H_

#include "Shader.h"

namespace TRAP::Graphics
{
	class DummyShader final : public Shader
	{
	public:
		DummyShader(const std::string& name, const std::string& filepath);
		~DummyShader() override = default;

		void Use(Window* window = nullptr) override;
		void UseTexture(uint32_t set, uint32_t binding, TRAP::Graphics::Texture* const texture,
		                Window* window = nullptr) override;
		void UseTextures(uint32_t set, uint32_t binding,
		                 const std::vector<TRAP::Graphics::Texture*>& textures,
						 Window* window = nullptr) override;
		void UseSampler(uint32_t set, uint32_t binding, TRAP::Graphics::Sampler* const sampler,
		                Window* window = nullptr) override;
		void UseSamplers(uint32_t set, uint32_t binding,
		                 const std::vector<TRAP::Graphics::Sampler*>& samplers,
						 Window* window = nullptr) override;
		void UseUBO(uint32_t set, uint32_t binding, TRAP::Graphics::UniformBuffer* const uniformBuffer,
		            uint64_t size, uint64_t offset, Window* window = nullptr) override;
		void UseSSBO(uint32_t set, uint32_t binding, TRAP::Graphics::StorageBuffer* const storageBuffer,
		             uint64_t size, Window* window = nullptr) override;
	};
}

#endif /*_TRAP_DUMMYSHADER_H_*/