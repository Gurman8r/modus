#include <modus_core/system/Memory.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// global memory manager
	static memory_manager * g_memory{};

	memory_manager * get_global_memory() noexcept {
		return g_memory;
	}

	memory_manager * set_global_memory(memory_manager * value) noexcept {
		return g_memory = value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	memory_manager::memory_manager(pmr::memory_resource * mres) noexcept
		: m_resource{ reinterpret_cast<passthrough_resource *>(mres) }
		, m_alloc{ m_resource }
		, m_records{ m_alloc }
		, m_counter{}
	{
		if (!get_global_memory() && (m_resource == pmr::get_default_resource()))
		{
			set_global_memory(this);
		}
	}

	memory_manager::~memory_manager() noexcept
	{
		ML_assert("MEMORY LEAKS DETECTED" && m_records.empty());

		if (this == get_global_memory())
		{
			set_global_memory(nullptr);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}