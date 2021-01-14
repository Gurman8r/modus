#ifndef _ML_VARIABLE_HPP_
#define _ML_VARIABLE_HPP_

#include <modus_core/system/Memory.hpp>

namespace ml
{
	// variable
	struct variable final : trackable, std::enable_shared_from_this<variable>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using null_t = typename decltype(nullptr);

		using boolean_t = typename bool;
		
		using integer_t = typename int64;
		
		using float_t = typename float64;

		using string_t = typename string;

		using char_t = typename string_t::value_type;

		using object_t = typename std::any;

		using variant_t = typename std::variant<null_t, boolean_t, integer_t, float_t, string_t, object_t>;

		enum : size_t { ID_null, ID_boolean, ID_integer, ID_float, ID_string, ID_object, ID_invalid = std::variant_npos };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T> static constexpr bool is_null_v{ std::is_same_v<T, null_t> };

		template <class T> static constexpr bool is_boolean_v{ std::is_same_v<T, boolean_t> };

		template <class T> static constexpr bool is_integer_v{ util::is_any_of_v<T, int8, int16, int32, int64, uint8, uint16, uint32, uint64> };

		template <class T> static constexpr bool is_float_v{ util::is_any_of_v<T, float32, float64, float80> };

		template <class T> static constexpr bool is_number_v{ is_integer_v<T> || is_float_v<T> };

		template <class T> static constexpr bool is_string_v{ util::is_any_of_v<T, char_t, char_t const *, string_t> || std::is_convertible_v<T, string_t> };

		template <class T> static constexpr bool is_primitive_v{ is_null_v<T> || is_boolean_v<T> || is_number_v<T> || is_string_v<T> };

		template <class T> static constexpr bool is_object_v{ !is_primitive_v<T> };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		variable() noexcept : m_data{ nullptr } {}

		variable(variable const & other) : m_data{ other.m_data } {}

		variable(variable && other) noexcept : m_data{} { this->swap(std::move(other)); }

		variable(null_t) noexcept : m_data{ nullptr } {}

		variable(boolean_t value) noexcept : m_data{ static_cast<boolean_t>(value) } {}

		variable(int8 value) noexcept : m_data{ static_cast<integer_t>(value) } {}
		
		variable(int16 value) noexcept : m_data{ static_cast<integer_t>(value) } {}
		
		variable(int32 value) noexcept : m_data{ static_cast<integer_t>(value) } {}
		
		variable(int64 value) noexcept : m_data{ static_cast<integer_t>(value) } {}

		variable(uint8 value) noexcept : m_data{ static_cast<integer_t>(value) } {}
		
		variable(uint16 value) noexcept : m_data{ static_cast<integer_t>(value) } {}
		
		variable(uint32 value) noexcept : m_data{ static_cast<integer_t>(value) } {}
		
		variable(uint64 value) noexcept : m_data{ static_cast<integer_t>(value) } {}
		
		variable(float32 value) noexcept : m_data{ static_cast<float_t>(value) } {}
		
		variable(float64 value) noexcept : m_data{ static_cast<float_t>(value) } {}
		
		variable(float80 value) noexcept : m_data{ static_cast<float_t>(value) } {}

		variable(size_t count, char_t value, string_t::allocator_type alloc = {}) : m_data{ string_t{ count, value, alloc } } {}

		variable(char_t const * value, size_t count, string_t::allocator_type alloc = {}) : m_data{ string_t{ value, count, alloc } } {}

		variable(char_t const * value, string_t::allocator_type alloc = {}) : m_data{ string_t{ value, alloc } } {}

		variable(string_t const & value, string_t::allocator_type alloc = {}) : m_data{ string_t{ value, alloc } } {}

		variable(string_t && value, string_t::allocator_type alloc = {}) noexcept : m_data{ string_t{ std::move(value), alloc } } {}

		template <class T> variable(T && value) noexcept : m_data{ std::make_any<T>(ML_forward(value)) } {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		void swap(variable & other) noexcept { m_data.swap(other.m_data); }

		variable & operator=(variable const & other) { variable temp{ other }; this->swap(temp); return (*this); }

		variable & operator=(variable && other) noexcept { this->swap(std::move(other)); return (*this); }

		template <class T
		> variable & operator=(T && value) noexcept { this->emplace(ML_forward(value)); return (*this); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto index() const noexcept -> size_t { return m_data ? m_data->index() : ID_invalid; }

		ML_NODISCARD bool is_valid() const noexcept { return index() != ID_invalid; }

		ML_NODISCARD bool is_null() const noexcept { return index() == ID_null; }

		ML_NODISCARD bool is_boolean() const noexcept { return index() == ID_boolean; }

		ML_NODISCARD bool is_integer() const noexcept { return index() == ID_integer; }

		ML_NODISCARD bool is_float() const noexcept { return index() == ID_float; }

		ML_NODISCARD bool is_number() const noexcept { return is_integer() || is_float(); }

		ML_NODISCARD bool is_string() const noexcept { return index() == ID_string; }

		ML_NODISCARD bool is_primitive() const noexcept { return index() <= ID_string; }

		ML_NODISCARD bool is_object() const noexcept { return index() == ID_object; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
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
			case ID_object	: return std::get<object_t>(*m_data).type();
			}
		}

		template <class T> ML_NODISCARD bool holds() const noexcept
		{
			if constexpr (is_primitive_v<T>)
			{
				return is_primitive() && std::holds_alternative<T>(*m_data);
			}
			else
			{
				return is_object() && (std::get<object_t>(*m_data).type().hash_code() == typeid(T).hash_code());
			}
		}

		template <class T> ML_NODISCARD auto get() -> T &
		{
			if constexpr (is_primitive_v<T>) { return std::get<T>(*m_data); }
			else { return *std::any_cast<T>(&std::get<object_t>(*m_data)); }
		}

		template <class T> ML_NODISCARD auto get() const -> T const &
		{
			if constexpr (is_primitive_v<T>) { return std::get<T>(*m_data); }
			else { return *std::any_cast<T>(&std::get<object_t>(*m_data)); }
		}

		template <class T> ML_NODISCARD auto get_to(T & value) const -> T & { return value = this->get<T>(); }

		template <class T> ML_NODISCARD operator T & () & noexcept { return this->get<T>(); }

		template <class T> ML_NODISCARD operator T const & () const & noexcept { return this->get<T>(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		auto emplace(null_t) noexcept -> null_t & { return std::get<null_t>(*(m_data = nullptr)); }

		auto emplace(boolean_t value) noexcept -> boolean_t & { return std::get<boolean_t>(*(m_data = static_cast<boolean_t>(value))); }

		auto emplace(int8 value) noexcept -> integer_t & { return std::get<integer_t>(*(m_data = static_cast<integer_t>(value))); }

		auto emplace(int16 value) noexcept -> integer_t & { return std::get<integer_t>(*(m_data = static_cast<integer_t>(value))); }

		auto emplace(int32 value) noexcept -> integer_t & { return std::get<integer_t>(*(m_data = static_cast<integer_t>(value))); }

		auto emplace(int64 value) noexcept -> integer_t & { return std::get<integer_t>(*(m_data = static_cast<integer_t>(value))); }

		auto emplace(uint8 value) noexcept -> integer_t & { return std::get<integer_t>(*(m_data = static_cast<integer_t>(value))); }

		auto emplace(uint16 value) noexcept -> integer_t & { return std::get<integer_t>(*(m_data = static_cast<integer_t>(value))); }

		auto emplace(uint32 value) noexcept -> integer_t & { return std::get<integer_t>(*(m_data = static_cast<integer_t>(value))); }

		auto emplace(uint64 value) noexcept -> integer_t & { return std::get<integer_t>(*(m_data = static_cast<integer_t>(value))); }

		auto emplace(float32 value) noexcept -> float_t & { return std::get<float_t>(*(m_data = static_cast<float_t>(value))); }

		auto emplace(float64 value) noexcept -> float_t & { return std::get<float_t>(*(m_data = static_cast<float_t>(value))); }

		auto emplace(float80 value) noexcept -> float_t & { return std::get<float_t>(*(m_data = static_cast<float_t>(value))); }

		auto emplace(size_t count, char_t value, string_t::allocator_type alloc = {}) noexcept -> string_t & { return std::get<string_t>(*(m_data = string_t{ count, value, alloc })); }
		
		auto emplace(char_t const * value, size_t count, string_t::allocator_type alloc = {}) noexcept -> string_t & { return std::get<string_t>(*(m_data = string_t{ value, count, alloc })); }

		auto emplace(char_t const * value, string_t::allocator_type alloc = {}) noexcept -> string_t & { return std::get<string_t>(*(m_data = string_t{ value, alloc })); }

		auto emplace(string_t const & value, string_t::allocator_type alloc = {}) noexcept -> string_t & { return std::get<string_t>(*(m_data = string_t{ value, alloc })); }

		auto emplace(string_t && value, string_t::allocator_type alloc = {}) noexcept -> string_t & { return std::get<string_t>(*(m_data = string_t{ std::move(value), alloc })); }

		template <class T, class ... Args
		> auto emplace(Args && ... args) noexcept -> T & { return *std::any_cast<T>(&std::get<object_t>(*(m_data = std::make_any<T>(ML_forward(args)...)))); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		std::optional<variant_t> m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_VARIABLE_HPP_