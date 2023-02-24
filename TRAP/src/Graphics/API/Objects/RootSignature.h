#ifndef TRAP_ROOTSIGNATURE_H
#define TRAP_ROOTSIGNATURE_H
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class RootSignature
	{
	public:
		/// <summary>
		/// Create a new root signature from the given description.
		/// </summary>
		/// <param name="desc">Root signature description.</param>
		/// <returns>Created root signature.</returns>
		[[nodiscard]] static TRAP::Ref<RootSignature> Create(const RendererAPI::RootSignatureDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~RootSignature();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		RootSignature(const RootSignature&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		RootSignature& operator=(const RootSignature&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		RootSignature(RootSignature&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		RootSignature& operator=(RootSignature&&) noexcept = default;

		/// <summary>
		/// Retrieve the pipeline type used by the shaders of the root signature.
		/// </summary>
		/// <returns>Pipeline type.</returns>
		[[nodiscard]] RendererAPI::PipelineType GetPipelineType() const noexcept;
		/// <summary>
		/// Retrieve the amount of descriptors contained in the root signature.
		/// </summary>
		/// <returns>Amount of descriptors.</returns>
		[[nodiscard]] uint32_t GetDescriptorCount() const noexcept;
		/// <summary>
		/// Retrieve the list of descriptors contained in the root signature.
		/// </summary>
		/// <returns>List of descriptors.</returns>
		[[nodiscard]] const std::vector<RendererAPI::DescriptorInfo>& GetDescriptors() const noexcept;
		/// <summary>
		/// Retrieve the map which converts a descriptor name to its index.
		/// </summary>
		/// <returns>Map which converts a descriptor name to its index.</returns>
		[[nodiscard]] const RendererAPI::DescriptorIndexMap& GetDescriptorNameToIndexMap() const noexcept;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		RootSignature();

		//Graphics or Compute
		RendererAPI::PipelineType m_pipelineType;
		//Array of all descriptors declared in the root signature layout
		std::vector<RendererAPI::DescriptorInfo> m_descriptors;
		//Translates hash of descriptor name to descriptor index in m_descriptors array
		RendererAPI::DescriptorIndexMap m_descriptorNameToIndexMap;
	};
}

#endif /*TRAP_ROOTSIGNATURE_H*/