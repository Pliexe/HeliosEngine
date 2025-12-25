#pragma once

#ifdef __cplusplus
#include <cstdint>
#endif

#ifdef uintptr_t
#undef uintptr_t
#endif

namespace Helios
{
	#ifdef __cplusplus
	typedef ::std::uintptr_t BehaviourHandle;
	#else
	typedef void* ScriptHandle;
	#endif
}
