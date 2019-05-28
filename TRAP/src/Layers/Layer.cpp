#include "TRAPPCH.h"
#include "Layer.h"
#include <utility>

namespace TRAP {

	Layer::Layer(std::string debugName)
		: m_DebugName(std::move(debugName))
	{
	}
}
