#ifndef TRAP_PIPELINE_H
#define TRAP_PIPELINE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Pipeline
	{
	public:
		static TRAP::Ref<Pipeline> Create(const RendererAPI::PipelineDesc& desc);

		virtual ~Pipeline();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Pipeline(const Pipeline&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Pipeline& operator=(const Pipeline&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Pipeline(Pipeline&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Pipeline& operator=(Pipeline&&) = default;

	protected:
		Pipeline();

		//No Graphic API independent data
	};
}

#endif /*TRAP_PIPELINE_H*/