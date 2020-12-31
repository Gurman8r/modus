#ifndef _ML_JSON_EDITOR_HPP_
#define _ML_JSON_EDITOR_HPP_

#include <modus_core/imgui/ImGui.hpp>
#include <modus_core/detail/Method.hpp>

namespace ml
{
	struct ML_NODISCARD ML_CORE_API json_editor final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		json * m_context{}, * m_selected{};
		fs::path m_path{};
		ds::list<char> m_buffer{};

		ds::method<void(json_editor *, cstring, json *)> on_item_selected{};
		ds::method<void(json_editor *, cstring, json *)> on_item_hovered{};
		ds::method<void(json_editor *, cstring, json *, int32)> on_item_clicked{};
		ds::method<void(json_editor *, cstring, json *)> on_item_context_menu{};
		ds::method<void(json_editor *, cstring, json *)> on_item_value{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class ... T
		> static constexpr json::value_t get_type_index() { static_assert(0); return json::value_t{}; }
		template <> static constexpr json::value_t get_type_index<void>() { return json::value_t::null; }
		template <> static constexpr json::value_t get_type_index<std::nullptr_t>() { return json::value_t::null; }
		template <> static constexpr json::value_t get_type_index<json::object_t>() { return json::value_t::object; }
		template <> static constexpr json::value_t get_type_index<json::array_t>() { return json::value_t::array; }
		template <> static constexpr json::value_t get_type_index<json::string_t>() { return json::value_t::string; }
		template <> static constexpr json::value_t get_type_index<json::boolean_t>() { return json::value_t::boolean; }
		template <> static constexpr json::value_t get_type_index<json::number_integer_t>() { return json::value_t::number_integer; }
		template <> static constexpr json::value_t get_type_index<json::number_unsigned_t>() { return json::value_t::number_unsigned; }
		template <> static constexpr json::value_t get_type_index<json::number_float_t>() { return json::value_t::number_float; }
		template <> static constexpr json::value_t get_type_index<json::binary_t>() { return json::value_t::binary; }

		template <class T = json::value_t> static constexpr cstring get_type_name(T type) noexcept
		{
			constexpr cstring names[] =
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

		template <class T = json::value_t> static constexpr cstring get_type_name() noexcept
		{
			return get_type_name(get_type_index<T>());
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_contents(bool show_values = true, ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None);

		bool draw_field(cstring key, json & value, bool show_values = true, ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None);

		bool draw_value(cstring key, json & value);

		bool draw_null(cstring key, json & value);
		
		bool draw_string(cstring key, json & value);
		
		bool draw_boolean(cstring key, json & value);
		
		bool draw_number_integer(cstring key, json & value);
		
		bool draw_number_unsigned(cstring key, json & value);
		
		bool draw_number_float(cstring key, json & value);
		
		bool draw_binary(cstring key, json & value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_JSON_EDITOR_HPP_