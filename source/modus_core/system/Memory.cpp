#include <modus_core/system/Memory.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	memory_manager::memory_manager(passthrough_resource * mres)
		: m_resource{ ML_check(mres) }
		, m_alloc	{ m_resource }
		, m_records	{ m_alloc }
		, m_counter	{}
	{
		ML_ctor_global(memory_manager);
	}

	memory_manager::~memory_manager() noexcept
	{
		ML_dtor_global(memory_manager);
#if 0
		while (!m_records.empty()) { this->deallocate(m_records.back<id_addr>()); }
#endif
		ML_verify("MEMORY LEAKS DETECTED" && m_records.empty());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global memory manager
namespace ml::globals
{
	static memory_manager * g_memory_manager{};

	ML_impl_global(memory_manager) get_global() { return g_memory_manager; }

	ML_impl_global(memory_manager) set_global(memory_manager * value) { return g_memory_manager = value; }
}