#ifndef _ML_VARIABLE_HPP_
#define _ML_VARIABLE_HPP_

#include <modus_core/system/Memory.hpp>

namespace ml::util
{
	
}

namespace ml
{
	// variable
	struct variable final : trackable, std::enable_shared_from_this<variable>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		enum : size_t
		{
			ID_null,
			ID_boolean,
			ID_integer,
			ID_unsigned,
			ID_float,
			ID_string,
			ID_object,
			ID_invalid = std::variant_npos
		};

		using self_t = typename variable;

		using null_t = typename std::nullptr_t;
		
		using boolean_t = typename bool;
		
		using integer_t = typename int64;
		
		using unsigned_t = typename uint64;
		
		using float_t = typename float64;
		
		using string_t = typename string;
		
		using char_t = typename string_t::value_type;
		
		using object_t = typename std::any;
		
		using variant_t = typename std::variant<null_t, boolean_t, integer_t, unsigned_t, float_t, string_t, object_t>;

		template <class T
		> using builtin_t = typename

			std::conditional_t<util::is_any_of_v<T, null_t>, null_t,

			std::conditional_t<util::is_any_of_v<T, boolean_t>, boolean_t,

			std::conditional_t<util::is_any_of_v<T, char_t, int8, int16, int32, int64>, integer_t,

			std::conditional_t<util::is_any_of_v<T, uint8, uint16, uint32, uint64>, unsigned_t,

			std::conditional_t<util::is_any_of_v<T, float32, float64, float80>, float_t,

			std::conditional_t<std::is_convertible_v<T, std::basic_string<char_t>>, string_t,

			object_t>>>>>>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T> static constexpr size_t index_v{ util::variant_index<variant_t, builtin_t<T>>() };

		template <class T> static constexpr bool is_null_v{ index_v<T> == ID_null };

		template <class T> static constexpr bool is_boolean_v{ index_v<T> == ID_boolean };

		template <class T> static constexpr bool is_number_integer_v{ index_v<T> == ID_integer };
		
		template <class T> static constexpr bool is_number_unsigned_v{ index_v<T> == ID_unsigned };
		
		template <class T> static constexpr bool is_number_float_v{ index_v<T> == ID_float};
		
		template <class T> static constexpr bool is_number_v{ is_number_integer_v<T> || is_number_unsigned_v<T> || is_number_float_v<T> };
		
		template <class T> static constexpr bool is_scalar_v{ is_boolean_v<T> || is_number_v<T> };
		
		template <class T> static constexpr bool is_string_v{ index_v<T> == ID_string };

		template <class T> static constexpr bool is_primitive_v{ index_v<T> <= ID_string };

		template <class T> static constexpr bool is_object_v{ index_v<T> == ID_object };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		variable() noexcept : m_data{ nullptr } {}

		variable(self_t const & other) : m_data{ other.m_data } {}

		variable(self_t && other) noexcept : m_data{} { m_data.swap(std::move(other.m_data)); }

		variable(null_t) noexcept : m_data{ nullptr } {}

		variable(boolean_t value) noexcept : m_data{ static_cast<boolean_t>(value) } {}

		variable(char_t value) noexcept : m_data{ static_cast<integer_t>(value) } {}

		variable(int8 value) noexcept : m_data{ static_cast<integer_t>(value) } {}
		
		variable(int16 value) noexcept : m_data{ static_cast<integer_t>(value) } {}
		
		variable(int32 value) noexcept : m_data{ static_cast<integer_t>(value) } {}
		
		variable(int64 value) noexcept : m_data{ static_cast<integer_t>(value) } {}

		variable(uint8 value) noexcept : m_data{ static_cast<unsigned_t>(value) } {}
		
		variable(uint16 value) noexcept : m_data{ static_cast<unsigned_t>(value) } {}
		
		variable(uint32 value) noexcept : m_data{ static_cast<unsigned_t>(value) } {}
		
		variable(uint64 value) noexcept : m_data{ static_cast<unsigned_t>(value) } {}
		
		variable(float32 value) noexcept : m_data{ static_cast<float_t>(value) } {}
		
		variable(float64 value) noexcept : m_data{ static_cast<float_t>(value) } {}
		
		variable(float80 value) noexcept : m_data{ static_cast<float_t>(value) } {}

		variable(size_t count, char_t value, string_t::allocator_type alloc = {}) : m_data{ string_t{ count, value, alloc } } {}

		variable(char_t const * value, size_t count, string_t::allocator_type alloc = {}) : m_data{ string_t{ value, count, alloc } } {}

		variable(char_t const * value, string_t::allocator_type alloc = {}) : m_data{ string_t{ value, alloc } } {}

		variable(string_t const & value, string_t::allocator_type alloc = {}) : m_data{ string_t{ value, alloc } } {}

		variable(string_t && value, string_t::allocator_type alloc = {}) noexcept : m_data{ string_t{ std::move(value), alloc } } {}

		template <class T> variable(T && value) noexcept : m_data{ object_t{ std::in_place_type<T>, ML_forward(value) } } {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		self_t & operator=(self_t const & other) { self_t temp{ other }; this->swap(temp); return (*this); }

		self_t & operator=(self_t && other) noexcept { this->swap(std::move(other)); return (*this); }

		template <class T> self_t & operator=(T && value) noexcept { this->emplace(ML_forward(value)); return (*this); }

		void swap(self_t & other) noexcept { m_data.swap(other.m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		decltype(auto) emplace(null_t) noexcept { return (null_t &)std::get<null_t>(m_data = nullptr); }

		decltype(auto) emplace(boolean_t value) noexcept { return (boolean_t &)std::get<boolean_t>(m_data = static_cast<boolean_t>(value)); }

		decltype(auto) emplace(char_t value) noexcept { return (char_t &)std::get<integer_t>(m_data = static_cast<integer_t>(value)); }

		decltype(auto) emplace(int8 value) noexcept { return (int8 &)std::get<integer_t>(m_data = static_cast<integer_t>(value)); }

		decltype(auto) emplace(int16 value) noexcept { return (int16 &)std::get<integer_t>(m_data = static_cast<integer_t>(value)); }

		decltype(auto) emplace(int32 value) noexcept { return (int32 &)std::get<integer_t>(m_data = static_cast<integer_t>(value)); }

		decltype(auto) emplace(int64 value) noexcept { return (int64 &)std::get<integer_t>(m_data = static_cast<integer_t>(value)); }

		decltype(auto) emplace(uint8 value) noexcept { return (uint8 &)std::get<unsigned_t>(m_data = static_cast<unsigned_t>(value)); }

		decltype(auto) emplace(uint16 value) noexcept { return (uint16 &)std::get<unsigned_t>(m_data = static_cast<unsigned_t>(value)); }

		decltype(auto) emplace(uint32 value) noexcept { return (uint32 &)std::get<unsigned_t>(m_data = static_cast<unsigned_t>(value)); }

		decltype(auto) emplace(uint64 value) noexcept { return (uint64 &)std::get<unsigned_t>(m_data = static_cast<unsigned_t>(value)); }

		decltype(auto) emplace(float32 value) noexcept { return (float32 &)std::get<float_t>(m_data = static_cast<float_t>(value)); }

		decltype(auto) emplace(float64 value) noexcept { return (float64 &)std::get<float_t>(m_data = static_cast<float_t>(value)); }

		decltype(auto) emplace(float80 value) noexcept { return (float80 &)std::get<float_t>(m_data = static_cast<float_t>(value)); }

		decltype(auto) emplace(size_t count, char_t value, string_t::allocator_type alloc = {}) noexcept { return std::get<string_t>(m_data = string_t{ count, value, alloc }); }
		
		decltype(auto) emplace(char_t const * value, size_t count, string_t::allocator_type alloc = {}) noexcept { return std::get<string_t>(m_data = string_t{ value, count, alloc }); }

		decltype(auto) emplace(char_t const * value, string_t::allocator_type alloc = {}) noexcept { return std::get<string_t>(m_data = string_t{ value, alloc }); }

		decltype(auto) emplace(string_t const & value, string_t::allocator_type alloc = {}) noexcept { return std::get<string_t>(m_data = string_t{ value, alloc }); }

		decltype(auto) emplace(string_t && value, string_t::allocator_type alloc = {}) noexcept { return std::get<string_t>(m_data = string_t{ std::move(value), alloc }); }

		decltype(auto) emplace(string_t::const_iterator first, string_t::const_iterator last, string_t::allocator_type alloc = {}) noexcept { return std::get<string_t>(m_data = string_t{ first, last, alloc }); }
		
		decltype(auto) emplace(object_t const & value) noexcept { return std::get<object_t>(m_data = object_t{ value }); }

		decltype(auto) emplace(object_t && value) noexcept { return std::get<object_t>(m_data = object_t{ std::move(value) }); }

		template <class T, class Arg0, class ... Args
		> decltype(auto) emplace(Arg0 && arg0, Args && ... args) noexcept
		{
			using B = typename builtin_t<T>;

			if constexpr (is_scalar_v<B>)
			{
				static_assert(0 == sizeof...(args));

				return (T &)std::get<B>(m_data = static_cast<B>(ML_forward(arg0)));
			}
			else if constexpr (!is_object_v<B>)
			{
				return (B &)std::get<B>(m_data = B{ ML_forward(arg0), ML_forward(args)... });
			}
			else
			{
				return (T &)(*std::any_cast<T>(std::addressof
				(
					std::get<B>(m_data = B{ std::in_place_type<T>, ML_forward(arg0), ML_forward(args)... })
				)));
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto index() const noexcept -> size_t { return m_data.index(); }

		ML_NODISCARD bool is_valid() const noexcept { return index() != ID_invalid; }

		ML_NODISCARD bool is_null() const noexcept { return index() == ID_null; }

		ML_NODISCARD bool is_boolean() const noexcept { return index() == ID_boolean; }

		ML_NODISCARD bool is_number_integer() const noexcept { return index() == ID_integer; }

		ML_NODISCARD bool is_number_unsigned() const noexcept { return index() == ID_unsigned; }
		
		ML_NODISCARD bool is_number_float() const noexcept { return index() == ID_float; }

		ML_NODISCARD bool is_number() const noexcept { return is_number_integer() || is_number_unsigned() || is_number_float(); }

		ML_NODISCARD bool is_scalar() const noexcept { return is_boolean() || is_number(); }

		ML_NODISCARD bool is_string() const noexcept { return index() == ID_string; }

		ML_NODISCARD bool is_primitive() const noexcept { return index() <= ID_string; }

		ML_NODISCARD bool is_object() const noexcept { return index() == ID_object; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto type() const noexcept -> std::type_info const &
		{
			switch (index())
			{
			default			: return typeid(void);
			case ID_null	: return typeid(null_t);
			case ID_boolean	: return typeid(boolean_t);
			case ID_integer	: return typeid(integer_t);
			case ID_float	: return typeid(float_t);
			case ID_string	: return typeid(string_t);
			case ID_object	: return std::get<object_t>(m_data).type();
			}
		}

		template <class T> ML_NODISCARD bool has() const noexcept
		{
			if constexpr (is_primitive_v<T>)
			{
				return is_primitive() && std::holds_alternative<T>(m_data);
			}
			else
			{
				return !is_primitive() && (std::get<object_t>(m_data).type().hash_code() == typeid(T).hash_code());
			}
		}

		template <class T> ML_NODISCARD auto get() -> T &
		{
			ML_assert(this->has<T>());
			if constexpr (is_primitive_v<T>) { return std::get<builtin_t<T>>(m_data); }
			else { return *std::any_cast<T>(std::addressof(std::get<builtin_t<T>>(m_data))); }
		}

		template <class T> ML_NODISCARD auto get() const -> T const &
		{
			ML_assert(this->has<T>());
			if constexpr (is_primitive_v<T>) { return std::get<builtin_t<T>>(m_data); }
			else { return *std::any_cast<T>(std::addressof(std::get<builtin_t<T>>(m_data))); }
		}

		template <class T> ML_NODISCARD auto get_to(T & value) const -> T & { return value = this->get<T>(); }

		template <class T> ML_NODISCARD operator T & () & noexcept { return this->get<T>(); }

		template <class T> ML_NODISCARD operator T const & () const & noexcept { return this->get<T>(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		variant_t m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_VARIABLE_HPP_