#ifndef TRAP_QUERYPOOL_H
#define TRAP_QUERYPOOL_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class QueryPool
	{
	public:
		/// @brief Create a new query pool from the given description.
		/// @param desc Query pool description.
		/// @return Created query pool.
		[[nodiscard]] static TRAP::Ref<QueryPool> Create(const RendererAPI::QueryPoolDesc& desc);

		/// @brief Destructor.
		virtual ~QueryPool();

		/// @brief Copy constructor.
		consteval QueryPool(const QueryPool&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval QueryPool& operator=(const QueryPool&) noexcept = delete;
		/// @brief Move constructor.
		constexpr QueryPool(QueryPool&&) noexcept = default;
		/// @brief Move assignment operator.
		QueryPool& operator=(QueryPool&&) noexcept = default;

	protected:
		/// @brief Constructor.
		QueryPool();

		//No Graphic API independent data
	};
}

#endif /*TRAP_QUERYPOOL_H*/
