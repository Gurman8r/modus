#ifndef _ML_METHOD_HPP_
#define _ML_METHOD_HPP_

#include <modus_core/detail/Utility.hpp>

namespace ml
{
	// method
	template <class Sig> struct method : public std::function<Sig>
	{
	public:
		using std::function<Sig>::function;

		using self_type = typename method<Sig>;

		method() noexcept = default;
		method(self_type const &) = default;
		method(self_type &&) noexcept = default;
		self_type & operator=(self_type const &) = default;
		self_type & operator=(self_type &&) noexcept = default;
	};
}

namespace ml::util
{
	// bind method and return its previous value
	template <class Clbk, class Fn, class ... Args
	> constexpr auto chain(Clbk & clbk, Fn && fn, Args && ... args) noexcept
	{
		auto const prev{ std::move(clbk) };
		
		if constexpr (0 < sizeof...(args))
		{
			clbk = std::bind(ML_forward(fn), ML_forward(args)...);
		}
		else
		{
			clbk = Clbk{ ML_forward(fn) };
		}
		
		return prev;
	}
}

#endif // !_ML_METHOD_HPP_