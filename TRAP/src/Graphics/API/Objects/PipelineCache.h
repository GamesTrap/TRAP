#ifndef TRAP_PIPELINECACHE_H
#define TRAP_PIPELINECACHE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class PipelineCache
	{
	public:
		static TRAP::Ref<PipelineCache> Create(const RendererAPI::PipelineCacheDesc& desc);
		static TRAP::Ref<PipelineCache> Create(const RendererAPI::PipelineCacheLoadDesc& desc);

		virtual ~PipelineCache();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		PipelineCache(const PipelineCache&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		PipelineCache& operator=(const PipelineCache&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		PipelineCache(PipelineCache&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		PipelineCache& operator=(PipelineCache&&) = default;

		virtual void GetPipelineCacheData(std::size_t* size, void* data) const = 0;

		virtual void Save(const std::filesystem::path& path) = 0;

	protected:
		PipelineCache();

		//No Graphic API independent data
	};
}

#endif /*TRAP_PIPELINECACHE_H*/