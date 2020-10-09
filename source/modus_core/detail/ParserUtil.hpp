#ifndef _ML_PARSER_UTIL_HPP_
#define _ML_PARSER_UTIL_HPP_

#include <detail/StringUtility.hpp>

namespace ml::impl
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class ... T> struct parse;

	template <> struct parse<> final { parse() = delete; };

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<pmr::string> final
	{
		ML_NODISCARD auto operator()(std::istream & in) const noexcept
		{
			pmr::string temp{};
			if (in.good()) { in >> temp; }
			return temp;
		}

		ML_NODISCARD auto operator()(std::istream & in, char delim) const noexcept
		{
			pmr::string temp{};
			std::getline(in, temp, delim);
			return temp;
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<bool> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, bool dv = {}) const noexcept
		{
			if (auto const o{ util::to_bool(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, bool dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<int8_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, int8_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_i8(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, int8_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<int16_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, int16_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_i16(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, int16_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<int32_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, int32_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_i32(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, int32_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<int64_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, int64_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_i64(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, int64_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<uint8_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, uint8_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_u8(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, uint8_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<uint16_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, uint16_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_u16(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, uint16_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<uint32_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, uint32_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_u32(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, uint32_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<uint64_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, uint64_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_u64(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, uint64_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<float32_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, float32_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_f32(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, float32_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<float64_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, float64_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_f64(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, float64_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct parse<float80_t> final
	{
		ML_NODISCARD auto operator()(pmr::string && str, float80_t dv = {}) const noexcept
		{
			if (auto const o{ util::to_f80(ML_forward(str)) }) { return (*o); }
			else { return dv; }
		}

		ML_NODISCARD auto operator()(std::istream & in, float80_t dv = {}) const noexcept
		{
			return (*this)(parse<pmr::string>{}(in), dv);
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
			pmr::stringstream ss{ str };
			return (*this)(ss);
		}

		ML_NODISCARD auto operator()(pmr::string const & str) const noexcept
		{
			pmr::stringstream ss{ str };
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