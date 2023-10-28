#ifndef TRAP_PIPELINECACHE_H
#define TRAP_PIPELINECACHE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class PipelineCache
	{
	public:
		/// <summary>
		/// Create a new pipeline cache from the given description.
		/// </summary>
		/// <param name="desc">Pipeline cache description.</param>
		/// <returns>Created pipeline cache.</returns>
		[[nodiscard]] static TRAP::Ref<PipelineCache> Create(const RendererAPI::PipelineCacheDesc& desc);
		/// <summary>
		/// Create a new pipeline cache from the given description.
		/// </summary>
		/// <param name="desc">Pipeline cache description.</param>
		/// <returns>Created pipeline cache.</returns>
		[[nodiscard]] static TRAP::Ref<PipelineCache> Create(const RendererAPI::PipelineCacheLoadDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~PipelineCache();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr PipelineCache(const PipelineCache&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		PipelineCache& operator=(const PipelineCache&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr PipelineCache(PipelineCache&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		PipelineCache& operator=(PipelineCache&&) noexcept = default;

		/// <summary>
		/// Retrieve the cached pipeline data.
		///
		/// To retrieve the size of cached data call this function with data = nullptr.
		/// </summary>
		/// <param name="size">Output: Size of the data.</param>
		/// <param name="data">Output: Pointer to store the data.</param>
		virtual void GetPipelineCacheData(usize* size, void* data) const = 0;

		/// <summary>
		/// Save a pipeline to disk.
		/// </summary>
		/// <param name="path">Path to save the pipeline to.</param>
		void Save(const std::filesystem::path& path) const;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		PipelineCache();

		//No Graphic API independent data
	};
}

#endif /*TRAP_PIPELINECACHE_H*/