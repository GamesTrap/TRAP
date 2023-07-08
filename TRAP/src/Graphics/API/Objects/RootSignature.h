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
		constexpr RootSignature(const RootSignature&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr RootSignature& operator=(const RootSignature&) = delete;
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
		[[nodiscard]] constexpr RendererAPI::PipelineType GetPipelineType() const noexcept;
		/// <summary>
		/// Retrieve the amount of descriptors contained in the root signature.
		/// </summary>
		/// <returns>Amount of descriptors.</returns>
		[[nodiscard]] constexpr uint64_t GetDescriptorCount() const noexcept;
		/// <summary>
		/// Retrieve the list of descriptors contained in the root signature.
		/// </summary>
		/// <returns>List of descriptors.</returns>
		[[nodiscard]] constexpr const std::vector<RendererAPI::DescriptorInfo>& GetDescriptors() const noexcept;
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
		RendererAPI::PipelineType m_pipelineType = RendererAPI::PipelineType::Undefined;
		//Array of all descriptors declared in the root signature layout
		std::vector<RendererAPI::DescriptorInfo> m_descriptors{};
		//Translates hash of descriptor name to descriptor index in m_descriptors array
		RendererAPI::DescriptorIndexMap m_descriptorNameToIndexMap{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::PipelineType TRAP::Graphics::RootSignature::GetPipelineType() const noexcept
{
	return m_pipelineType;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t TRAP::Graphics::RootSignature::GetDescriptorCount() const noexcept
{
	return m_descriptors.size();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::RendererAPI::DescriptorInfo>& TRAP::Graphics::RootSignature::GetDescriptors() const noexcept
{
	return m_descriptors;
}

#endif /*TRAP_ROOTSIGNATURE_H*/