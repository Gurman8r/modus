#ifndef _ML_PARSER_UTIL_HPP_
#define _ML_PARSER_UTIL_HPP_

#include <modus_core/detail/StringUtility.hpp>

namespace ml::impl
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class ... T> struct parse;

	template <> struct parse<> final { parse() = delete; };

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<ds::string> final
	{
		ML_NODISCARD auto operator()(std::istream & in) const noexcept
		{
			ds::string temp{};
			if (in.good()) { in >> temp; }
			return temp;
		}

		ML_NODISCARD auto operator()(std::istream & in, char delim) const noexcept
		{
			ds::string temp{};
			std::getline(in, temp, delim);
			return temp;
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<bool> final
	{
		ML_NODISCARD auto operator()(ds::string && str, bool dv = {}) const noexcept
		{
			if (auto const o{ util::to_bool(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, bool dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<int8> final
	{
		ML_NODISCARD auto operator()(ds::string && str, int8 dv = {}) const noexcept
		{
			if (auto const o{ util::to_i8(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, int8 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<int16> final
	{
		ML_NODISCARD auto operator()(ds::string && str, int16 dv = {}) const noexcept
		{
			if (auto const o{ util::to_i16(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, int16 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<int32> final
	{
		ML_NODISCARD auto operator()(ds::string && str, int32 dv = {}) const noexcept
		{
			if (auto const o{ util::to_i32(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, int32 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<int64> final
	{
		ML_NODISCARD auto operator()(ds::string && str, int64 dv = {}) const noexcept
		{
			if (auto const o{ util::to_i64(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, int64 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<uint8> final
	{
		ML_NODISCARD auto operator()(ds::string && str, uint8 dv = {}) const noexcept
		{
			if (auto const o{ util::to_u8(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, uint8 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<uint16> final
	{
		ML_NODISCARD auto operator()(ds::string && str, uint16 dv = {}) const noexcept
		{
			if (auto const o{ util::to_u16(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, uint16 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<uint32> final
	{
		ML_NODISCARD auto operator()(ds::string && str, uint32 dv = {}) const noexcept
		{
			if (auto const o{ util::to_u32(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, uint32 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<uint64> final
	{
		ML_NODISCARD auto operator()(ds::string && str, uint64 dv = {}) const noexcept
		{
			if (auto const o{ util::to_u64(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, uint64 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<float32> final
	{
		ML_NODISCARD auto operator()(ds::string && str, float32 dv = {}) const noexcept
		{
			if (auto const o{ util::to_f32(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, float32 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<float64> final
	{
		ML_NODISCARD auto operator()(ds::string && str, float64 dv = {}) const noexcept
		{
			if (auto const o{ util::to_f64(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, float64 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<float80> final
	{
		ML_NODISCARD auto operator()(ds::string && str, float80 dv = {}) const noexcept
		{
			if (auto const o{ util::to_f80(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, float80 dv = {}) const noexcept
		{
			return (*this)(parse<ds::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T> struct parse<T> final
	{
		ML_NODISCARD auto operator()(std::istream & in) const noexcept
		{
			T temp{};
			if (in.good()) { in >> temp; }
			return temp;
		}

		ML_NODISCARD auto operator()(cstring str) const noexcept
		{
			ds::stringstream ss{ str };
			return (*this)(ss);
		}

		ML_NODISCARD auto operator()(ds::string const & str) const noexcept
		{
			ds::stringstream ss{ str };
			return (*this)(ss);
		}

		ML_NODISCARD auto operator()() const noexcept
		{
			return (*this)(std::cin);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

namespace ml::util
{
	template <class T, class ... Args
	> ML_NODISCARD static T parse(Args && ... args) noexcept
	{
		return std::invoke(_ML impl::parse<T>{}, ML_forward(args)...);
	}
}

#endif // !_ML_PARSER_UTIL_HPP_