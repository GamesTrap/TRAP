#ifndef TRAP_PIPELINE_H
#define TRAP_PIPELINE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Pipeline
	{
	public:
		/// @brief Create a new pipeline from the given description.
		/// @param desc Pipeline description.
		/// @return Created pipeline.
		[[nodiscard]] static TRAP::Ref<Pipeline> Create(const RendererAPI::PipelineDesc& desc);

		/// @brief Destructor.
		virtual ~Pipeline();

		/// @brief Copy constructor.
		constexpr Pipeline(const Pipeline&) noexcept = default;
		/// @brief Copy assignment operator.
		Pipeline& operator=(const Pipeline&) noexcept = default;
		/// @brief Move constructor.
		constexpr Pipeline(Pipeline&&) noexcept = default;
		/// @brief Move assignment operator.
		Pipeline& operator=(Pipeline&&) noexcept = default;

	protected:
		/// @brief Constructor.
		Pipeline();

		//No Graphic API independent data
	};
}

#endif /*TRAP_PIPELINE_H*/