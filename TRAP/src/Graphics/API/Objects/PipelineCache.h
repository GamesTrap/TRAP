#ifndef TRAP_PIPELINECACHE_H
#define TRAP_PIPELINECACHE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class PipelineCache
	{
	public:
		/// @brief Create a new pipeline cache from the given description.
		/// @param desc Pipeline cache description.
		/// @return Created pipeline cache.
		[[nodiscard]] static TRAP::Ref<PipelineCache> Create(const RendererAPI::PipelineCacheDesc& desc);
		/// @brief Create a new pipeline cache from the given description.
		/// @param desc Pipeline cache description.
		/// @return Created pipeline cache.
		[[nodiscard]] static TRAP::Ref<PipelineCache> Create(const RendererAPI::PipelineCacheLoadDesc& desc);

		/// @brief Destructor.
		virtual ~PipelineCache();

		/// @brief Copy constructor.
		consteval PipelineCache(const PipelineCache&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval PipelineCache& operator=(const PipelineCache&) noexcept = delete;
		/// @brief Move constructor.
		constexpr PipelineCache(PipelineCache&&) noexcept = default;
		/// @brief Move assignment operator.
		PipelineCache& operator=(PipelineCache&&) noexcept = default;

		/// @brief Retrieve the cached pipeline data.
		///        To retrieve the size of cached data call this function with data = nullptr.
		/// @param size Output: Size of the data.
		/// @param data Output: Pointer to store the data.
		virtual void GetPipelineCacheData(usize* size, void* data) const = 0;

		/// @brief Save a pipeline to disk.
		/// @param path Path to save the pipeline to.
		void Save(const std::filesystem::path& path) const;

	protected:
		/// @brief Constructor.
		PipelineCache();

		//No Graphic API independent data
	};
}

#endif /*TRAP_PIPELINECACHE_H*/