#ifndef _ML_OBJECT_HPP_
#define _ML_OBJECT_HPP_

#include <modus_core/system/Memory.hpp>

namespace ml
{
	struct ML_CORE_API object : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~object() noexcept override = default;

		object(allocator_type alloc = {}) noexcept
			: m_name{ "New Object", alloc }
		{
		}

		object(string const & name, allocator_type alloc = {}) noexcept
			: m_name{ name, alloc }
		{
		}

		object(object && other, allocator_type alloc = {}) noexcept
			: m_name{ alloc }
		{
			object::swap(*this, std::move(other));
		}

		object & operator=(object && other) noexcept
		{
			object::swap(*this, std::move(other));
			return (*this);
		}

		static void swap(object & a, object & b) noexcept
		{
			if (std::addressof(a) != std::addressof(b))
			{
				std::swap(a.m_name, b.m_name);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return {}; }

		ML_NODISCARD auto get_name() const noexcept -> string const & { return m_name; }

		void set_name(string const & value) noexcept { if (m_name != value) { m_name = value; } }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		string m_name;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_OBJECT_HPP_