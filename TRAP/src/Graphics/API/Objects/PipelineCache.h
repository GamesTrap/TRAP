#ifndef TRAP_PIPELINECACHE_H
#define TRAP_PIPELINECACHE_H

#include <vector>
#include <filesystem>

#include "Core/Base.h"
#include "Core/Types.h"
#include "Utils/SmartPtr.h"

namespace TRAP::Graphics
{
	struct PipelineCacheDesc;
	struct PipelineCacheLoadDesc;

	class PipelineCache
	{
	public:
		/// @brief Create a new pipeline cache from the given description.
		/// @param desc Pipeline cache description.
		/// @return Created pipeline cache.
		[[nodiscard]] static TRAP::Ref<PipelineCache> Create(const PipelineCacheDesc& desc);
		/// @brief Create a new pipeline cache from the given description.
		/// @param desc Pipeline cache description.
		/// @return Created pipeline cache.
		[[nodiscard]] static TRAP::Ref<PipelineCache> Create(const PipelineCacheLoadDesc& desc);

		/// @brief Destructor.
		constexpr virtual ~PipelineCache();

		/// @brief Copy constructor.
		consteval PipelineCache(const PipelineCache&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval PipelineCache& operator=(const PipelineCache&) noexcept = delete;
		/// @brief Move constructor.
		constexpr PipelineCache(PipelineCache&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr PipelineCache& operator=(PipelineCache&&) noexcept = default;

		/// @brief Retrieve the cached pipeline data in bytes.
		/// @return Pipeline cache data as bytes.
		[[nodiscard]] virtual std::vector<u8> GetPipelineCacheData() const = 0;

		/// @brief Save a pipeline to disk.
		/// @param path Path to save the pipeline to.
		void Save(const std::filesystem::path& path) const;

	protected:
		/// @brief Constructor.
		constexpr PipelineCache();

		//No Graphic API independent data

	private:
		static void DebugLog(std::string_view msg);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::PipelineCache::~PipelineCache()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if(!std::is_constant_evaluated())
	{
		DebugLog("Destroying PipelineCache");
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::PipelineCache::PipelineCache()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if(!std::is_constant_evaluated())
	{
		DebugLog("Creating PipelineCache");
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

#endif /*TRAP_PIPELINECACHE_H*/
