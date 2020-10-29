#ifndef _ML_OS_HPP_
#define _ML_OS_HPP_

#include <modus_core/detail/Debug.hpp>

namespace ml
{
	struct ML_CORE_API OS final
	{
		OS() noexcept;
		
		~OS() noexcept;
	};
}

namespace ml::globals
{
	template <> ML_NODISCARD ML_CORE_API OS * get() noexcept;
	
	template <> ML_CORE_API OS * set(OS * value) noexcept;
}

#endif // !_ML_OS_HPP_