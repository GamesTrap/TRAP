#ifndef TRAP_ROOTSIGNATURE_H
#define TRAP_ROOTSIGNATURE_H
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class RootSignature
	{
	public:
		/// @brief Create a new root signature from the given description.
		/// @param desc Root signature description.
		/// @return Created root signature.
		[[nodiscard]] static TRAP::Ref<RootSignature> Create(const RendererAPI::RootSignatureDesc& desc);

		/// @brief Destructor.
		virtual ~RootSignature();

		/// @brief Copy constructor.
		consteval RootSignature(const RootSignature&) = delete;
		/// @brief Copy assignment operator.
		consteval RootSignature& operator=(const RootSignature&) = delete;
		/// @brief Move constructor.
		RootSignature(RootSignature&&) noexcept = default;
		/// @brief Move assignment operator.
		RootSignature& operator=(RootSignature&&) noexcept = default;

		/// @brief Retrieve the pipeline type used by the shaders of the root signature.
		/// @return Pipeline type.
		[[nodiscard]] constexpr RendererAPI::PipelineType GetPipelineType() const noexcept;
		/// @brief Retrieve the amount of descriptors contained in the root signature.
		/// @return Amount of descriptors.
		[[nodiscard]] constexpr u64 GetDescriptorCount() const noexcept;
		/// @brief Retrieve the list of descriptors contained in the root signature.
		/// @return List of descriptors.
		[[nodiscard]] constexpr const std::vector<RendererAPI::DescriptorInfo>& GetDescriptors() const noexcept;
		/// @brief Retrieve the map which converts a descriptor name to its index.
		/// @return Map which converts a descriptor name to its index.
		[[nodiscard]] const RendererAPI::DescriptorIndexMap& GetDescriptorNameToIndexMap() const noexcept;

	protected:
		/// @brief Constructor.
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

[[nodiscard]] constexpr u64 TRAP::Graphics::RootSignature::GetDescriptorCount() const noexcept
{
	return m_descriptors.size();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::RendererAPI::DescriptorInfo>& TRAP::Graphics::RootSignature::GetDescriptors() const noexcept
{
	return m_descriptors;
}

#endif /*TRAP_ROOTSIGNATURE_H*/
