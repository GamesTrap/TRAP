#ifndef TRAP_QUERYPOOL_H
#define TRAP_QUERYPOOL_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class QueryPool
	{
	public:
		/// <summary>
		/// Create a new query pool from the given description.
		/// </summary>
		/// <param name="desc">Query pool description.</param>
		/// <returns>Created query pool.</returns>
		[[nodiscard]] static TRAP::Ref<QueryPool> Create(const RendererAPI::QueryPoolDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~QueryPool();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		QueryPool(const QueryPool&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		QueryPool& operator=(const QueryPool&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		QueryPool(QueryPool&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		QueryPool& operator=(QueryPool&&) noexcept = default;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		QueryPool();

		//No Graphic API independent data
	};
}

#endif /*TRAP_QUERYPOOL_H*/