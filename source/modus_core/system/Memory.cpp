#include <modus_core/system/Memory.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static memory_manager * g_memory{};

	memory_manager * get_global_memory() noexcept {
		return g_memory;
	}

	memory_manager * set_global_memory(memory_manager * value) noexcept {
		return g_memory = value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}