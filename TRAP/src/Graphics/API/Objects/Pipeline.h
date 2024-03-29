#ifndef TRAP_PIPELINE_H
#define TRAP_PIPELINE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Pipeline
	{
	public:
		/// <summary>
		/// Create a new pipeline from the given description.
		/// </summary>
		/// <param name="desc">Pipeline description.</param>
		/// <returns>Created pipeline.</returns>
		[[nodiscard]] static TRAP::Ref<Pipeline> Create(const RendererAPI::PipelineDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Pipeline();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Pipeline(const Pipeline&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Pipeline& operator=(const Pipeline&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Pipeline(Pipeline&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Pipeline& operator=(Pipeline&&) noexcept = default;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		Pipeline();

		//No Graphic API independent data
	};
}

#endif /*TRAP_PIPELINE_H*/