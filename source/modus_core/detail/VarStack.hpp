#ifndef _ML_VAR_STACK_HPP_
#define _ML_VAR_STACK_HPP_

// WIP

#include <modus_core/detail/Memory.hpp>

namespace ml
{
	enum var_type_
	{
		var_type_none,
		var_type_int,
		var_type_float,
		var_type_MAX
	};

	struct var_info final
	{
		var_type_	type;
		uint32		count;
		uint32		offset;

		void * get(void * value) const noexcept {
			return (void *)((byte *)value + offset);
		}
	};

	struct var_stack final : trackable
	{
		var_stack() noexcept
		{
		}

		var_stack(var_stack const & other)
		{
		}

		var_stack(var_stack && other) noexcept
		{
			this->swap(std::move(other));
		}

		var_stack & operator=(var_stack const & other)
		{
			var_stack temp{ other };
			this->swap(temp);
			return (*this);
		}

		var_stack & operator=(var_stack && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(var_stack & other) noexcept
		{
			if (this != std::addressof(other))
			{
			}
		}

		auto push(var_type_ type) {}

		auto pop(size_t count = 1) {}
	};
}

#endif // !_ML_VAR_STACK_HPP_