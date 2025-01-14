#ifndef TRAP_QUERYPOOL_H
#define TRAP_QUERYPOOL_H

#include "Core/Base.h"
#include "Utils/SmartPtr.h"

namespace TRAP::Graphics
{
	struct QueryPoolDesc;

	class QueryPool
	{
	public:
		/// @brief Create a new query pool from the given description.
		/// @param desc Query pool description.
		/// @return Created query pool.
		[[nodiscard]] static TRAP::Ref<QueryPool> Create(const QueryPoolDesc& desc);

		/// @brief Destructor.
		constexpr virtual ~QueryPool();

		/// @brief Copy constructor.
		consteval QueryPool(const QueryPool&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval QueryPool& operator=(const QueryPool&) noexcept = delete;
		/// @brief Move constructor.
		constexpr QueryPool(QueryPool&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr QueryPool& operator=(QueryPool&&) noexcept = default;

	protected:
		/// @brief Constructor.
		constexpr QueryPool();

		//No Graphic API independent data

	private:
		static void DebugLog(std::string_view msg);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::QueryPool::~QueryPool()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if(!std::is_constant_evaluated())
	{
		DebugLog("Destroying QueryPool");
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::QueryPool::QueryPool()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if(!std::is_constant_evaluated())
	{
		DebugLog("Creating QueryPool");
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

#endif /*TRAP_QUERYPOOL_H*/
