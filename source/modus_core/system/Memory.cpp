#include <modus_core/system/Memory.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	memory_manager::memory_manager(pmr::memory_resource * mres)
		: m_resource{ reinterpret_cast<passthrough_resource *>(mres) }
		, m_alloc	{ m_resource }
		, m_records	{ m_alloc }
		, m_counter	{}
	{
		if (!get_global<memory_manager>())
		{
			set_global<memory_manager>(this);
		}
	}

	memory_manager::~memory_manager() noexcept
	{
		ML_assert("MEMORY LEAKS DETECTED" && m_records.empty());

		if (this == get_global<memory_manager>())
		{
			set_global<memory_manager>(nullptr);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global memory manager
namespace ml::globals
{
	static memory_manager * g_memory_manager{};

	template <> memory_manager * get() noexcept {
		return g_memory_manager;
	}

	template <> memory_manager * set(memory_manager * value) noexcept {
		return g_memory_manager = value;
	}
}