#ifndef _TRAP_QUERYPOOL_H_
#define _TRAP_QUERYPOOL_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class QueryPool
	{
	public:
		static TRAP::Ref<QueryPool> Create(const RendererAPI::QueryPoolDesc& desc);

		virtual ~QueryPool();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		QueryPool(const QueryPool&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		QueryPool& operator=(const QueryPool&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		QueryPool(QueryPool&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		QueryPool& operator=(QueryPool&&) = default;

	protected:
		QueryPool();

		//No Graphic API independent data
	};
}

#endif /*_TRAP_QUERYPOOL_H_*/