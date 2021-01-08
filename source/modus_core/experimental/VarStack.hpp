#ifndef _ML_VAR_STACK_HPP_
#define _ML_VAR_STACK_HPP_

// WIP

#include <modus_core/detail/List.hpp>

namespace ml
{
	enum vtype_
	{
		vtype_none,
		vtype_int,
		vtype_uint,
		vtype_float,
		vtype_double,
		vtype_MAX
	};

	struct vinfo final
	{
		vtype_ type; // 
		uint32 count; // 
		uint32 offset; // 

		void * get(void * value) const noexcept { return (void *)((byte *)value + offset); }
	};

	struct vstack final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		vstack() noexcept
		{
		}

		vstack(vstack const & other)
		{
		}

		vstack(vstack && other) noexcept : vstack{}
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		vstack & operator=(vstack const & other)
		{
			vstack temp{ other };
			this->swap(temp);
			return (*this);
		}

		vstack & operator=(vstack && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(vstack & other) noexcept
		{
			if (this != std::addressof(other))
			{
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		vinfo * get_info(size_t index) const
		{
			return {};
		}

		void * get_var(size_t index) const
		{
			return {};
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void push(void * ctx, size_t index, int32 value)
		{
			vinfo * const info{ get_info(index) };
			if (info->type == vtype_int && info->count == 1)
			{
				int32 * pvar{ (int32 *)info->get(ctx) };
				*pvar = value;
			}
		}
		
		void push(size_t index, uint32 value)
		{
		}
		
		void push(size_t index, float32 value)
		{
		}
		
		void push(size_t index, float64 value)
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void pop(size_t count = 1)
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_VAR_STACK_HPP_