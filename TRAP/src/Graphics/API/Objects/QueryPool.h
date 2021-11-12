#ifndef TRAP_QUERYPOOL_H
#define TRAP_QUERYPOOL_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class QueryPool
	{
	public:
		static TRAP::Ref<QueryPool> Create(const RendererAPI::QueryPoolDesc& desc);

		virtual ~QueryPool();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		QueryPool(const QueryPool&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		QueryPool& operator=(const QueryPool&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		QueryPool(QueryPool&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		QueryPool& operator=(QueryPool&&) = default;

	protected:
		QueryPool();

		//No Graphic API independent data
	};
}

#endif /*TRAP_QUERYPOOL_H*/