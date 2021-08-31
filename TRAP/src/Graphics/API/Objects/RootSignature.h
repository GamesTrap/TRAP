#ifndef _TRAP_ROOTSIGNATURE_H_
#define _TRAP_ROOTSIGNATURE_H_
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class RootSignature
	{
	public:
		static TRAP::Ref<RootSignature> Create(const RendererAPI::RootSignatureDesc& desc);

		virtual ~RootSignature();

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

#endif /*_TRAP_ROOTSIGNATURE_H_*/