#ifndef _ML_JSON_EDITOR_HPP_
#define _ML_JSON_EDITOR_HPP_

#include <modus_core/imgui/ImGui.hpp>
#include <modus_core/detail/Method.hpp>

namespace ml
{
	struct ML_NODISCARD ML_CORE_API json_editor final
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using value_t			= typename json::value_t;
		using object_t			= typename json::object_t;
		using array_t			= typename json::array_t;
		using string_t			= typename json::string_t;
		using boolean_t			= typename json::boolean_t;
		using number_integer_t	= typename json::number_integer_t;
		using number_unsigned_t	= typename json::number_unsigned_t;
		using number_float_t	= typename json::number_float_t;
		using binary_t			= typename json::binary_t;
		using iterator			= typename json::iterator;
		using const_iterator	= typename json::const_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class ... T
		> static constexpr value_t get_type() { static_assert(0); return value_t{}; }
		template <> static constexpr value_t get_type<>() { return value_t::null; }
		template <> static constexpr value_t get_type<void>() { return value_t::null; }
		template <> static constexpr value_t get_type<std::nullptr_t>() { return value_t::null; }
		template <> static constexpr value_t get_type<object_t>() { return value_t::object; }
		template <> static constexpr value_t get_type<array_t>() { return value_t::array; }
		template <> static constexpr value_t get_type<string_t>() { return value_t::string; }
		template <> static constexpr value_t get_type<boolean_t>() { return value_t::boolean; }
		template <> static constexpr value_t get_type<number_integer_t>() { return value_t::number_integer; }
		template <> static constexpr value_t get_type<number_unsigned_t>() { return value_t::number_unsigned; }
		template <> static constexpr value_t get_type<number_float_t>() { return value_t::number_float; }
		template <> static constexpr value_t get_type<binary_t>() { return value_t::binary; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr cstring get_type_name(value_t type) noexcept
		{
			constexpr cstring names[10] =
			{
				"null",
				"object",
				"array",
				"string",
				"boolean",
				"integer",
				"unsigned",
				"float",
				"binary",
				"discarded",
			};
			return names[static_cast<size_t>(type)];
		}

		static cstring get_type_name(json const & value) noexcept
		{
			return get_type_name(value.type());
		}

		template <class T> static constexpr cstring get_type_name() noexcept
		{
			return get_type_name(get_type<T>());
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		json_editor(json * root = nullptr) noexcept : m_root{ root } {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		json *			m_root{};
		json *			m_selected{};
		bool			m_show_values{ true };
		ds::list<char>	m_temp_buffer{};

		ds::method<void(json_editor *, cstring, json *)>		on_item_selected		{};
		ds::method<void(json_editor *, cstring, json *)>		on_item_hovered			{};
		ds::method<void(json_editor *, cstring, json *, int32)>	on_item_clicked			{};
		ds::method<void(json_editor *, cstring, json *)>		on_item_popup_context	{};
		ds::method<void(json_editor *, cstring, json *)>		on_item_repr			{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_contents(ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);
		
		void draw_elements(cstring key, json & value, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

		void draw_field(cstring key, json & value, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool repr_auto(cstring key, json & value);

		bool repr_null(cstring key, json & value);
		
		bool repr_object(cstring key, json & value);
		
		bool repr_array(cstring key, json & value);

		bool repr_string(cstring key, json & value);
		
		bool repr_boolean(cstring key, json & value);
		
		bool repr_number_integer(cstring key, json & value);
		
		bool repr_number_unsigned(cstring key, json & value);
		
		bool repr_number_float(cstring key, json & value);
		
		bool repr_binary(cstring key, json & value);
		
		bool repr_discarded(cstring key, json & value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_JSON_EDITOR_HPP_