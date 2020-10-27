#ifndef _TRAP_SCRIPTABLEENTITY_H_
#define _TRAP_SCRIPTABLEENTITY_H_

#include "Entity.h"

namespace TRAP
{
	class ScriptableEntity
	{
	public:
		template<typename T>
		T& GetComponent()
		{
			return m_entity.GetComponent<T>();
		}
		
	private:
		Entity m_entity;
		friend class Scene;
	};
}

#endif /*_TRAP_SCRIPTABLEENTITY_H_*/