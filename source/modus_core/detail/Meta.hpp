#ifndef _ML_META_HPP_
#define _ML_META_HPP_

// Sources:
//
// https://www.youtube.com/watch?v=NTWSeQtHZ9M
// https://github.com/SuperV1234/cppcon2015
// https://github.com/SuperV1234/cppcon2015/tree/master/Other/ecs/Utils/MPL
// https://github.com/SuperV1234/cppcon2015/blob/master/Other/ecs/Utils/MetaFor.hpp
// https://github.com/SuperV1234/cppcon2015/blob/master/Other/ecs/Utils/MPL/TypeListOps.hpp
// 
// https://stackoverflow.com/questions/18063451/get-index-of-a-tuple-elements-type
// https://stackoverflow.com/questions/25958259/how-do-i-find-out-if-a-tuple-contains-a-type
// https://stackoverflow.com/questions/37029886/how-to-construct-a-tuple-from-an-array
// https://stackoverflow.com/questions/36580855/construct-tuple-by-passing-the-same-argument-to-each-element-with-explicit-const
// https://stackoverflow.com/questions/22560100/how-to-initialize-all-tuple-elements-by-the-same-arguments

#include <modus_core/Standard.hpp>

#define _ML_META _ML meta:: // meta

// DS
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// type list
	template <class ... Ts
	> struct list { static constexpr size_t size{ sizeof...(Ts) }; };

	// type tag
	template <class T
	> struct tag { using type = typename T; };

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// loops
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Tup, class Fn, size_t ... Is
	> constexpr void impl_tuple_expand(Tup && tp, Fn && fn, std::index_sequence<Is...>) noexcept
	{
		ML_forward(fn)(std::get<Is>(ML_forward(tp))...);
	}

	template <class Tup, class Fn
	> constexpr void impl_tuple_expand(Tup && tp, Fn && fn) noexcept
	{
		_ML_META impl_tuple_expand(
			ML_forward(tp),
			ML_forward(fn),
			std::make_index_sequence<std::tuple_size_v<std::decay_t<Tup>>>());
	}

	template <class Fn, class ... Args
	> constexpr void impl_for_args(Fn && fn, Args && ... args) noexcept
	{
		if constexpr (0 < sizeof...(args)) // sanity check
		{
			(void)std::initializer_list<int32>
			{
				(ML_forward(fn)(ML_forward(args)), 0)...
			};
		}
	}

	template <class Tup, class Fn
	> constexpr void impl_for_tuple(Tup && tp, Fn && fn) noexcept
	{
		_ML_META impl_tuple_expand(ML_forward(tp), [&fn](auto && ... rest) noexcept
		{
			_ML_META for_args(fn, ML_forward(rest)...);
		});
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// "unpacks" the contents of a tuple inside a function call
	template <class Tup, class Fn
	> constexpr void tuple_expand(Tup && tp, Fn && fn) noexcept
	{
		_ML_META impl_tuple_expand(ML_forward(tp), ML_forward(fn));
	}

	// invokes a function on every passed object
	template <class Fn, class ... Args
	> constexpr void for_args(Fn && fn, Args && ... args) noexcept
	{
		_ML_META impl_for_args(ML_forward(fn), ML_forward(args)...);
	}

	// invokes a function on every element of a tuple
	template <class Tup, class Fn
	> constexpr void for_tuple(Tup && tp, Fn && fn) noexcept
	{
		_ML_META impl_for_tuple(ML_forward(tp), ML_forward(fn));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// CONCAT - combine types
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class ...
	> struct impl_concat
	{
		using type = typename list<>;
	};

	template <class ... Ts
	> ML_alias concat = typename impl_concat<Ts...>::type;

	template <class ... Ts
	> struct impl_concat<list<Ts...>>
	{
		using type = typename list<Ts...>;
	};

	template <class ... Ts0, class ... Ts1, class ... Rest
	> struct impl_concat<list<Ts0...>, list<Ts1...>, Rest...>
	{
		using type = typename concat<list<Ts0..., Ts1...>, Rest...>;
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// REMAP - modify inner types
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <template <class> class Pr, class
	> struct impl_remap
	{
		using type = typename list<>;
	};

	template <template <class> class Pr, class Ls
	> ML_alias remap = typename impl_remap<Pr, Ls>::type;

	template <template <class> class Pr, class T, class ... Ts
	> struct impl_remap<Pr, list<T, Ts...>>
	{
		using type = typename concat<list<Pr<T>>, remap<Pr, list<Ts...>>>;
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// RENAME - modify outer type
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <template <class...> class To, class T
	> struct impl_rename;

	template <
		template <class ...> class To,
		template <class...> class From,
		class ... Ts
	> struct impl_rename<To, From<Ts...>>
	{
		using type = typename To<Ts...>;
	};

	template<
		template <class...> class To, class T
	> ML_alias rename = typename impl_rename<To, T>::type;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// CONTAINS - check list contains type
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T, class Ls
	> struct contains;

	template <class T
	> struct contains<T, list<>>
		: std::false_type {};

	template <class T, class U, class ... Ts
	> struct contains<T, list<U, Ts...>>
		: contains<T, list<Ts...>> {};

	template <class T, class ... Ts
	> struct contains<T, list<T, Ts...>>
		: std::true_type {};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// INDEX OF - get index of type in a list
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T, class Ls
	> struct index_of;

	template <class T, class ... Ts
	> struct index_of<T, list<T, Ts...>>
		: std::integral_constant<size_t, 0> {};

	template <class T, class U, class ... Ts
	> struct index_of<T, list<U, Ts...>>
		: std::integral_constant<size_t, 1 + index_of<T, list<Ts...>>::value> {};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// GENERAL
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	template <class Ls
	> ML_alias tuple = typename rename<std::tuple, Ls>;
	
	template <class Ls
	> ML_alias tag_tuple = typename tuple<remap<tag, Ls>>;

	template <class Ls, class Fn
	> constexpr void for_type_list(Fn && fn) noexcept
	{
		_ML_META for_tuple(tag_tuple<Ls>{}, fn);
	}

	template <class T0, class ... Ts, class Fn
	> constexpr void for_types(Fn && fn) noexcept
	{
		_ML_META for_type_list<_ML_META list<T0, Ts...>>(ML_forward(fn));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Ls
	> constexpr size_t size() noexcept { return Ls::size; }

	template <class Ls, class T> ML_alias push_back = typename concat<Ls, list<T>>;

	template <class Ls, class T> ML_alias push_front = typename concat<list<T>, Ls>;

	template <size_t I, class Ls> ML_alias nth = typename std::tuple_element_t<I, tuple<Ls>>;

	template <class Ls> ML_alias head = typename nth<0, Ls>;

	template <class Ls> ML_alias tail = typename nth<size<Ls>() - 1, Ls>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// REPEAT - size N list of T
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <size_t N, class T
	> struct impl_repeat
	{
		using type = typename push_back<typename impl_repeat<N - 1, T>::type, T>;
	};

	template <class T
	> struct impl_repeat<0, T>
	{
		using type = typename list<>;
	};

	template <size_t N, class T
	> ML_alias repeat = typename impl_repeat<N, T>::type;

	template <class T, size_t N
	> ML_alias array = typename tuple<repeat<N, T>>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// FILTER - remove types from list
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <template <class> class Pr, class
	> struct impl_filter
	{
		using type = typename list<>;
	};

	template <template <class> class Pr, class Ls
	> ML_alias filter = typename impl_filter<Pr, Ls>::type;

	template <template <class> class Pr, class T, class ... Ts
	> struct impl_filter<Pr, list<T, Ts...>>
	{
		using next = typename filter<Pr, list<Ts...>>;

		using type = typename std::conditional_t<
			(Pr<T>{}),
			concat<list<T>, next>,
			next
		>;
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// ALL - used to run a check against all types in a list
namespace ml::meta
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <template <class> class, class...
	> struct all
		: std::true_type {};

	template <template <class> class Pr, class T, class ... Ts
	> struct all<Pr, T, Ts...>
		: std::bool_constant<(Pr<T>{} && all<Pr, Ts...>{})> {};

	template <template <class> class TMF
	> struct bound_all
	{
		template <class ... Ts
		> using type = typename all<TMF, Ts...>;
	};

	template <template <class> class TMF, class TL
	> ML_alias all_types = typename rename<bound_all<TMF>::template type, TL>;

	// example
	static_assert(all<std::is_const, int const>{});
	static_assert(all_types<std::is_const, list<int const, float const>>{});

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_META_HPP_