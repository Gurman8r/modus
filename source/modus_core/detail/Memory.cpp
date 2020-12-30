#include <modus_core/detail/Memory.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	memory_manager::memory_manager(passthrough_resource * mres)
		: m_resource{ ML_check(mres) }
		, m_alloc	{ m_resource }
		, m_records	{ m_alloc }
		, m_counter	{}
	{
		ML_verify(ML_begin_global(memory_manager, this));
	}

	memory_manager::~memory_manager() noexcept
	{
#if ML_IMPL_CLEANUP
		while (!m_records.empty()) { this->deallocate(m_records.back<id_addr>()); }
#endif
		ML_verify("MEMORY LEAKS DETECTED" && m_records.empty());
		
		ML_verify(ML_end_global(memory_manager, this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global memory manager
namespace ml::globals
{
	static memory_manager * g_memory_manager{};

	ML_impl_global(memory_manager) get_global() noexcept
	{
		return g_memory_manager;
	}

	ML_impl_global(memory_manager) set_global(memory_manager * value) noexcept
	{
		return g_memory_manager = value;
	}
}