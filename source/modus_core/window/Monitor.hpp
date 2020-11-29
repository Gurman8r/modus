#ifndef _ML_MONITOR_HPP_
#define _ML_MONITOR_HPP_

#include <modus_core/detail/Memory.hpp>

namespace ml
{
	ML_decl_handle(monitor_handle); // monitor handle

	// monitor (WIP)
	struct ML_CORE_API monitor final : non_copyable, trackable
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		~monitor() noexcept override {}

		monitor(monitor_handle value) noexcept : m_handle{ value } {}

		monitor() noexcept : monitor{ nullptr } {}

		monitor(monitor && other) noexcept : monitor{} { this->swap(std::move(other)); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		monitor & operator=(monitor && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(monitor & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_handle, other.m_handle);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_handle() const noexcept -> monitor_handle { return m_handle; }

		ML_NODISCARD static ds::list<monitor_handle> const & get_monitors() noexcept;

		ML_NODISCARD static monitor_handle get_primary() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		monitor_handle m_handle;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MONITOR_HPP_