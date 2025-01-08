#ifndef TRAP_ROOTSIGNATURE_H
#define TRAP_ROOTSIGNATURE_H

#include <vector>

#include "Core/Types.h"
#include "Utils/SmartPtr.h"
#include "Utils/Map.h"

namespace TRAP::Graphics
{
	struct RootSignatureDesc;
    struct DescriptorInfo;
    enum class PipelineType : u32;

    using DescriptorIndexMap = TRAP::Utils::UnorderedStringMap<u32>;

	class RootSignature
	{
	public:
		/// @brief Create a new root signature from the given description.
		/// @param desc Root signature description.
		/// @return Created root signature.
		[[nodiscard]] static TRAP::Ref<RootSignature> Create(const RootSignatureDesc& desc);

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
		[[nodiscard]] constexpr PipelineType GetPipelineType() const noexcept;
		/// @brief Retrieve the list of descriptors contained in the root signature.
		/// @return List of descriptors.
		[[nodiscard]] constexpr const std::vector<DescriptorInfo>& GetDescriptors() const noexcept;
		/// @brief Retrieve the map which converts a descriptor name to its index.
		/// @return Map which converts a descriptor name to its index.
		[[nodiscard]] const DescriptorIndexMap& GetDescriptorNameToIndexMap() const noexcept;

	protected:
		/// @brief Constructor.
		RootSignature();

		//Graphics or Compute
		PipelineType m_pipelineType;
		//Array of all descriptors declared in the root signature layout
		std::vector<DescriptorInfo> m_descriptors;
		//Translates hash of descriptor name to descriptor index in m_descriptors array
		DescriptorIndexMap m_descriptorNameToIndexMap{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::PipelineType TRAP::Graphics::RootSignature::GetPipelineType() const noexcept
{
	return m_pipelineType;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::DescriptorInfo>& TRAP::Graphics::RootSignature::GetDescriptors() const noexcept
{
	return m_descriptors;
}

#endif /*TRAP_ROOTSIGNATURE_H*/
