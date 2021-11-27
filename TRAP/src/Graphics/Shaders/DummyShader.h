#ifndef TRAP_DUMMYSHADER_H
#define TRAP_DUMMYSHADER_H

#include "Shader.h"

namespace TRAP::Graphics
{
	class DummyShader final : public Shader
	{
	public:
		DummyShader(std::string name, std::filesystem::path filepath);
		~DummyShader() override = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		DummyShader(const DummyShader&) = default;
		/// <summary>
		/// Copy assignment pperator.
		/// </summary>
		DummyShader& operator=(const DummyShader&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		DummyShader(DummyShader&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		DummyShader& operator=(DummyShader&&) = default;

		void Use(Window* window = nullptr) override;
		void UseTexture(uint32_t set, uint32_t binding, TRAP::Graphics::Texture* texture,
		                Window* window = nullptr) override;
		void UseTextures(uint32_t set, uint32_t binding,
		                 const std::vector<TRAP::Graphics::Texture*>& textures,
						 Window* window = nullptr) override;
		void UseSampler(uint32_t set, uint32_t binding, TRAP::Graphics::Sampler* sampler,
		                Window* window = nullptr) override;
		void UseSamplers(uint32_t set, uint32_t binding,
		                 const std::vector<TRAP::Graphics::Sampler*>& samplers,
						 Window* window = nullptr) override;
		void UseUBO(uint32_t set, uint32_t binding, TRAP::Graphics::UniformBuffer* uniformBuffer,
		            uint64_t size, uint64_t offset, Window* window = nullptr) override;
		void UseSSBO(uint32_t set, uint32_t binding, TRAP::Graphics::StorageBuffer* storageBuffer,
		             uint64_t size, Window* window = nullptr) override;
	};
}

#endif /*TRAP_DUMMYSHADER_H*/