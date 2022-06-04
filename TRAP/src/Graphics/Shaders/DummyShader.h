#ifndef TRAP_DUMMYSHADER_H
#define TRAP_DUMMYSHADER_H

#include "Shader.h"

namespace TRAP::Graphics
{
	/// <summary>
	/// Dummy shader only saving the name and filepath of the shader.
	/// </summary>
	class DummyShader final : public Shader
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of the shader.</param>
		/// <param name="filepath">Filepath of the shader.</param>
		DummyShader(std::string name, std::filesystem::path filepath);
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of the shader.</param>
		/// <param name="filepath">Filepath of the shader.</param>
		/// <param name="stages">Used shader stages.</param>
		DummyShader(std::string name, std::filesystem::path filepath, RendererAPI::ShaderStage stages);
		/// <summary>
		/// Destructor.
		/// </summary>
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

		/// <summary>
		/// Use shader for rendering on the given window.
		/// Note: DummyShader always uses the Fallback shader.
		/// </summary>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		void Use(Window* window) override;
		/// <summary>
		/// Use texture with this shader on the given window.
		/// Note: DummyShader does nothing.
		/// </summary>
		/// <param name="set">Descriptor set to use the texture with.</param>
		/// <param name="binding">Binding point of the texture.</param>
		/// <param name="texture">Texture to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		void UseTexture(uint32_t set, uint32_t binding, TRAP::Graphics::Texture* texture,
		                Window* window) override;
		/// <summary>
		/// Use multiple textures with this shader on the given window.
		/// Note: DummyShader does nothing.
		/// </summary>
		/// <param name="set">Descriptor set to use the textures with.</param>
		/// <param name="binding">Binding point of the textures.</param>
		/// <param name="textures">Textures to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		void UseTextures(uint32_t set, uint32_t binding,
		                 const std::vector<TRAP::Graphics::Texture*>& textures,
						 Window* window ) override;
		/// <summary>
		/// Use sampler with this shader on the given window.
		/// Note: DummyShader does nothing.
		/// </summary>
		/// <param name="set">Descriptor set to use the sampler with.</param>
		/// <param name="binding">Binding point of the sampler.</param>
		/// <param name="sampler">Sampler to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		void UseSampler(uint32_t set, uint32_t binding, TRAP::Graphics::Sampler* sampler,
		                Window* window) override;
		/// <summary>
		/// Use multiple samplers with this shader on the given window.
		/// Note: DummyShader does nothing.
		/// </summary>
		/// <param name="set">Descriptor set to use the samplers with.</param>
		/// <param name="binding">Binding point of the samplers.</param>
		/// <param name="sampler">Samplers to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		void UseSamplers(uint32_t set, uint32_t binding,
		                 const std::vector<TRAP::Graphics::Sampler*>& samplers,
						 Window* window) override;
		/// <summary>
		/// Use uniform buffer object with this shader on the given window.
		/// Note: DummyShader does nothing.
		/// </summary>
		/// <param name="set">Descriptor set to use the UBO with.</param>
		/// <param name="binding">Binding point of the UBO.</param>
		/// <param name="uniformBuffer">Uniform buffer to use.</param>
		/// <param name="size">Size of the UBO.</param>
		/// <param name="offset">Offset of the UBO.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		void UseUBO(uint32_t set, uint32_t binding, TRAP::Graphics::UniformBuffer* uniformBuffer,
		            uint64_t size, uint64_t offset, Window* window) override;
		/// <summary>
		/// Use shader storage buffer object with this shader on the given window.
		/// Note: DummyShader does nothing.
		/// </summary>
		/// <param name="set">Descriptor set to use the SSBO with.</param>
		/// <param name="binding">Binding point of the SSBO.</param>
		/// <param name="storageBuffer">Storage buffer to use.</param>
		/// <param name="size">Size of the SSBO.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		void UseSSBO(uint32_t set, uint32_t binding, TRAP::Graphics::StorageBuffer* storageBuffer,
		             uint64_t size, Window* window) override;
	};
}

#endif /*TRAP_DUMMYSHADER_H*/