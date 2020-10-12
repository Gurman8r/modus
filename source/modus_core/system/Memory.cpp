#include <modus_core/system/Memory.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	memory_manager * memory_manager::g_mem{};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	memory_manager::memory_manager(passthrough_resource & res)
		: m_resource{ std::addressof(res) }
		, m_alloc	{ m_resource }
		, m_records	{ m_alloc }
		, m_counter	{}
	{
		// singleton
		ML_assert(!g_mem && (g_mem = this));

		// check has default resource
		ML_assert(m_resource == pmr::get_default_resource());
	}

	memory_manager::~memory_manager() noexcept
	{
		// singleton
		ML_assert(g_mem == this && !(g_mem = nullptr));

		// check for leaks
		ML_assert("MEMORY LEAKS DETECTED" && m_records.empty<>());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}