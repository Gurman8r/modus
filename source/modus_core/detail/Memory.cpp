#include <modus_core/detail/Memory.hpp>

#ifndef ML_IMPL_LEAK_CLEANUP
#define ML_IMPL_LEAK_CLEANUP 0
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	memory_manager::memory_manager(passthrough_resource * mres)
		: m_resource{ ML_check(mres) }
		, m_alloc	{ m_resource }
		, m_records	{ m_alloc }
		, m_counter	{}
	{
		ML_assert(begin_singleton<memory_manager>(this));
	}

	memory_manager::~memory_manager() noexcept
	{
		ML_assert(end_singleton<memory_manager>(this));

#if ML_IMPL_LEAK_CLEANUP
		while (!m_records.empty()) { this->deallocate(m_records.back<id_addr>()); }
#else
		ML_assert("MEMORY LEAKS DETECTED" && m_records.empty());
#endif
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global memory manager
namespace ml::globals
{
	static memory_manager * g_memory_manager{};

	ML_impl_global(memory_manager) get() noexcept
	{
		return g_memory_manager;
	}

	ML_impl_global(memory_manager) set(memory_manager * value) noexcept
	{
		return g_memory_manager = value;
	}
}