#ifndef _ML_JSON_EDITOR_HPP_
#define _ML_JSON_EDITOR_HPP_

#include <modus_core/gui/ImGui.hpp>
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

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		json *		m_root{};
		json *		m_selected{};
		bool		m_show_values{ true };
		list<char>	m_temp_buffer{};

		method<void(json_editor *, cstring, json *)>		on_item_selected		{};
		method<void(json_editor *, cstring, json *)>		on_item_hovered			{};
		method<void(json_editor *, cstring, json *, int32)>	on_item_clicked			{};
		method<void(json_editor *, cstring, json *)>		on_item_popup_context	{};
		method<void(json_editor *, cstring, json *)>		on_item_repr			{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		json_editor(json * root = nullptr) noexcept : m_root{ root } {}

		void draw(ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

		void draw_elements(cstring key, json & value, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

		void draw_field(cstring key, json & value, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static bool repr_auto(cstring key, json & value);

		static bool repr_null(cstring key, json & value);
		
		static bool repr_object(cstring key, json & value);
		
		static bool repr_array(cstring key, json & value);

		static bool repr_string(cstring key, json & value);
		
		static bool repr_boolean(cstring key, json & value);
		
		static bool repr_number_integer(cstring key, json & value);
		
		static bool repr_number_unsigned(cstring key, json & value);
		
		static bool repr_number_float(cstring key, json & value);
		
		static bool repr_binary(cstring key, json & value);
		
		static bool repr_discarded(cstring key, json & value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static bool node_behavior(ImGuiID id, ImGuiTreeNodeFlags flags, cstring label, cstring label_end = NULL);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_JSON_EDITOR_HPP_