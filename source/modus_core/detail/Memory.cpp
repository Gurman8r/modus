#include <modus_core/detail/Memory.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	memory_manager::memory_manager(pmr::memory_resource * mres)
		: m_resource{ reinterpret_cast<passthrough_resource *>(mres) }
		, m_alloc	{ m_resource }
		, m_records	{ m_alloc }
		, m_counter	{}
	{
		ML_assert(begin_global<memory_manager>(this));
	}

	memory_manager::~memory_manager() noexcept
	{
		ML_assert_msg(m_records.empty(), "MEMORY LEAKS DETECTED");

		ML_assert(end_global<memory_manager>(this));
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