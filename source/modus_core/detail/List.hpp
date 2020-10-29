#ifndef _ML_LIST_HPP_
#define _ML_LIST_HPP_

// WIP

#include <modus_core/detail/Utility.hpp>

namespace ml::ds
{
	// list ( WIP / PLACEHOLDER )
	template <
		class Ty,
		class Al = pmr::polymorphic_allocator<Ty>
	> ML_alias list = typename pmr::vector<Ty>;
}

#endif // !_ML_LIST_HPP_