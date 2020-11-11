#ifndef _ML_METHOD_HPP_
#define _ML_METHOD_HPP_

// WIP / PLACEHOLDER

#include <modus_core/detail/Utility.hpp>

namespace ml::ds
{
	// method
	template <class Signature
	> struct method : public std::function<Signature>
	{
		using std::function<Signature>::function;
	};
}

namespace ml::util
{
	// bind callback and return its previous value
	template <class Clbk, class Fn, class ... Args
	> constexpr auto route_callback(Clbk & clbk, Fn && fn, Args && ... args) noexcept
	{
		auto const prev{ std::move(clbk) };
		
		if constexpr (0 < sizeof...(Args))
		{
			clbk = std::bind(ML_forward(fn), ML_forward(args)...);
		}
		else
		{
			clbk = ML_forward(fn);
		}
		
		return prev;
	}
}

#endif // !_ML_METHOD_HPP_