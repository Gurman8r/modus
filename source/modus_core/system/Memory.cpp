#include <modus_core/system/Memory.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static memory_manager * g_memory{};

	memory_manager * get_default_memory() noexcept {
		return g_memory;
	}

	memory_manager * set_default_memory(memory_manager * value) noexcept {
		return g_memory = value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}