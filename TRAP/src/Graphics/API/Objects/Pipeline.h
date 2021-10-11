#ifndef _TRAP_PIPELINE_H_
#define _TRAP_PIPELINE_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Pipeline
	{
	public:
		static TRAP::Ref<Pipeline> Create(const RendererAPI::PipelineDesc& desc);

		virtual ~Pipeline();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Pipeline(const Pipeline&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		Pipeline& operator=(const Pipeline&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Pipeline(Pipeline&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Pipeline& operator=(Pipeline&&) = default;

	protected:
		Pipeline();

		//No Graphic API independent data
	};
}

#endif /*_TRAP_PIPELINE_H_*/