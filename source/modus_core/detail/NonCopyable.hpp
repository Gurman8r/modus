#ifndef _ML_NON_COPYABLE_HPP_
#define _ML_NON_COPYABLE_HPP_

#include <modus_core/Export.hpp>
#include <modus_core/detail/Utility.hpp>

namespace ml
{
	struct ML_CORE_API non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		non_copyable() noexcept = default;
		~non_copyable() noexcept = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		non_copyable(non_copyable const &) = delete;
		non_copyable & operator=(non_copyable const &) = delete;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NON_COPYABLE_HPP_