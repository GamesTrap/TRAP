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

	protected:
		QueryPool();

		//No Graphic API independent data
	};
}

#endif /*_TRAP_QUERYPOOL_H_*/