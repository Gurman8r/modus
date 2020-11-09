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
	template <class Clbk, class Fn, class ... Args
	> constexpr auto swap_callback(Clbk & clbk, Fn && fn, Args && ... args) noexcept
	{
		auto const temp{ std::move(clbk) };
		clbk = std::bind(ML_forward(fn), ML_forward(args)...);
		return temp;
	}
}

#endif // !_ML_METHOD_HPP_