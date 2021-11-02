#ifndef TRAP_ROOTSIGNATURE_H
#define TRAP_ROOTSIGNATURE_H
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class RootSignature
	{
	public:
		static TRAP::Ref<RootSignature> Create(const RendererAPI::RootSignatureDesc& desc);

		virtual ~RootSignature();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		RootSignature(const RootSignature&) = delete;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		RootSignature& operator=(const RootSignature&) = delete;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		RootSignature(RootSignature&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		RootSignature& operator=(RootSignature&&) = default;

		virtual RendererAPI::PipelineType GetPipelineType() const = 0;
		virtual uint32_t GetDescriptorCount() const = 0;
		virtual const std::vector<RendererAPI::DescriptorInfo>& GetDescriptors() const = 0;
		virtual const TRAP::Scope<RendererAPI::DescriptorIndexMap>& GetDescriptorNameToIndexMap() const = 0;

	protected:
		RootSignature();

		//Graphics or Compute
		RendererAPI::PipelineType m_pipelineType;
		//Array of all descriptors declared in the root signature layout
		std::vector<RendererAPI::DescriptorInfo> m_descriptors;
		//Translates hash of descriptor name to descriptor index in m_descriptors array
		TRAP::Scope<RendererAPI::DescriptorIndexMap> m_descriptorNameToIndexMap;
	};
}

#endif /*TRAP_ROOTSIGNATURE_H*/