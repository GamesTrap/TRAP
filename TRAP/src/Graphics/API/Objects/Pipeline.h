#ifndef TRAP_PIPELINE_H
#define TRAP_PIPELINE_H

#include "Core/Base.h"
#include "Utils/SmartPtr.h"

namespace TRAP::Graphics
{
	struct PipelineDesc;

	class Pipeline
	{
	public:
		/// @brief Create a new pipeline from the given description.
		/// @param desc Pipeline description.
		/// @return Created pipeline.
		[[nodiscard]] static TRAP::Ref<Pipeline> Create(const PipelineDesc& desc);

		/// @brief Destructor.
		constexpr virtual ~Pipeline();

		/// @brief Copy constructor.
		consteval Pipeline(const Pipeline&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Pipeline& operator=(const Pipeline&) noexcept = delete;
		/// @brief Move constructor.
		constexpr Pipeline(Pipeline&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr Pipeline& operator=(Pipeline&&) noexcept = default;

	protected:
		/// @brief Constructor.
		constexpr Pipeline();

		//No Graphic API independent data

	private:
		static void DebugLog(std::string_view msg);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::Pipeline::~Pipeline()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if(!std::is_constant_evaluated())
	{
		DebugLog("Destroying Pipeline");
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::Pipeline::Pipeline()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if(!std::is_constant_evaluated())
	{
		DebugLog("Creating Pipeline");
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

#endif /*TRAP_PIPELINE_H*/
