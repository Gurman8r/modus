#include <modus_core/editor/JsonEditor.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static void show_popup_context_item(json & value)
	{
		if (ImGui::MenuItem("object", "", nullptr, !value.is_object())) {}
		if (ImGui::MenuItem("array", "", nullptr, !value.is_array())) {}
		if (ImGui::MenuItem("string", "", nullptr, !value.is_string())) {}
		if (ImGui::MenuItem("integer", "", nullptr, !value.is_number_integer())) {}
		if (ImGui::MenuItem("unsigned", "", nullptr, !value.is_number_unsigned())) {}
		if (ImGui::MenuItem("float", "", nullptr, !value.is_number_float())) {}
		if (ImGui::MenuItem("null", "", nullptr, !value.is_null())) {}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void json_editor::draw_contents(bool show_values, ImGuiTreeNodeFlags node_flags)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2, 2 });
		ImGui::Columns(1 + show_values);
		if (m_context)
		{
			ImGui_Scope(m_context);
			for (json::iterator it = m_context->begin(); it != m_context->end(); ++it)
			{
				int32 const index{ (int32)std::distance(m_context->begin(), it) };
				ImGui_Scope(index);
				char item_label[32]{};
				if (m_context->is_object()) { std::sprintf(item_label, "%s", it.key().c_str()); }
				else if (m_context->is_array()) { std::sprintf(item_label, "[%i]", index); }
				draw_field(item_label, it.value(), show_values, node_flags);
			}
		}
		ImGui::Columns(1 + show_values);
		ImGui::PopStyleVar();
	}

	bool json_editor::draw_field(cstring key, json & value, bool show_values, ImGuiTreeNodeFlags node_flags)
	{
		json::value_t const type{ value.type() };
		bool const is_object{ type == json::value_t::object };
		bool const is_array{ type == json::value_t::array };
		bool const is_leaf{ !is_object && !is_array };

		ImGuiWindow * const window{ ImGui::GetCurrentWindow() };
		if (window->SkipItems) { return false; }

		ImGuiID const node_id{ window->GetID(get_type_name(type)) };
		char node_label[32]{}; std::sprintf(node_label, "%s", key);
		if (is_leaf) { node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet; }
		if (m_selected == &value) { node_flags |= ImGuiTreeNodeFlags_Selected; }
		ImGui::AlignTextToFramePadding();
		bool const node_open{ ImGui::TreeNodeBehavior(node_id, node_flags, node_label) };
		
		if (ImGui::IsItemHovered()) {
			if (on_item_hovered) {
				on_item_hovered(this, key, &value);
			}
			for (int32 i = 0; i < ImGuiMouseButton_COUNT; ++i) {
				if (ImGui::IsMouseClicked(i)) {
					if (on_item_clicked) {
						on_item_clicked(this, key, &value, i);
					}
					if (m_selected != &value) {
						m_selected = &value;
						if (on_item_selected) {
							on_item_selected(this, key, &value);
						}
					}
				}
			}
		}
		
		if (on_item_context_menu) {
			if (ImGui::BeginPopupContextItem("popup_context_item")) {
				on_item_context_menu(this, key, &value);
				ImGui::EndPopup();
			}
		}

		if (show_values) {
			if (is_leaf) {
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				if (on_item_value) {
					on_item_value(this, key, &value);
				}
				ImGui::NextColumn();
			}
			else {
				ImGui::NextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::TextDisabled(is_object ? "{}" : "[]");
				ImGui::NextColumn();
			}
		}

		if (!is_leaf && node_open) {
			for (json::iterator it = value.begin(); it != value.end(); ++it) {
				int32 const index{ (int32)std::distance(value.begin(), it) };
				ImGui_Scope(index);
				char item_label[32]{};
				if (is_object) { std::sprintf(item_label, "%s", it.key().c_str()); }
				else { std::sprintf(item_label, "[%i]", index); }
				draw_field(item_label, it.value(), show_values, node_flags);
			}
			ImGui::TreePop();
		}

		return node_open;
	}

	bool json_editor::draw_value(cstring key, json & value)
	{
		switch (value.type())
		{
		default								: return false;
		case json::value_t::null			: return draw_null(key, value);
		case json::value_t::string			: return draw_string(key, value);
		case json::value_t::boolean			: return draw_boolean(key, value);
		case json::value_t::number_integer	: return draw_number_integer(key, value);
		case json::value_t::number_unsigned	: return draw_number_integer(key, value);
		case json::value_t::number_float	: return draw_number_float(key, value);
		case json::value_t::binary			: return draw_binary(key, value);
		}
	}

	bool json_editor::draw_null(cstring key, json & value)
	{
		ImGui::Text("null");
		return false;
	}

	bool json_editor::draw_string(cstring key, json & value)
	{
		using T = typename json::string_t;
		constexpr json::value_t type_index{ get_type_index<T>() };
		constexpr cstring type_name{ get_type_name<T>() };

		bool changed{};
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", type_name, key);
		auto temp{ value.get<T>() };
		ImGui::Text("%.*s", temp.size(), temp.data());
		if (changed) { value = temp; }
		return changed;
	}

	bool json_editor::draw_boolean(cstring key, json & value)
	{
		using T = typename json::boolean_t;
		constexpr json::value_t type_index{ get_type_index<T>() };
		constexpr cstring type_name{ get_type_name<T>() };

		bool changed{};
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", type_name, key);
		auto temp{ value.get<T>() };
		changed |= ImGui::Checkbox(input_label, &temp);
		if (changed) { value = temp; }
		return changed;
	}

	bool json_editor::draw_number_integer(cstring key, json & value)
	{
		using T = typename json::number_integer_t;
		constexpr json::value_t type_index{ get_type_index<T>() };
		constexpr cstring type_name{ get_type_name<T>() };

		bool changed{};
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", type_name, key);
		auto temp{ value.get<T>() };
		changed |= ImGui::DragScalar(input_label, ImGuiDataType_S32, &temp, 1.f, 0, 0, "%i", ImGuiSliderFlags_None);
		if (changed) { value = temp; }
		return changed;
	}

	bool json_editor::draw_number_unsigned(cstring key, json & value)
	{
		using T = typename json::number_unsigned_t;
		constexpr json::value_t type_index{ get_type_index<T>() };
		constexpr cstring type_name{ get_type_name<T>() };

		bool changed{};
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", type_name, key);
		auto temp{ value.get<T>() };
		changed |= ImGui::DragScalar(input_label, ImGuiDataType_U32, &temp, 1.f, 0, 0, "%u", ImGuiSliderFlags_None);
		if (changed) { value = temp; }
		return changed;
	}

	bool json_editor::draw_number_float(cstring key, json & value)
	{
		using T = typename json::number_float_t;
		constexpr json::value_t type_index{ get_type_index<T>() };
		constexpr cstring type_name{ get_type_name<T>() };

		bool changed{};
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", type_name, key);
		auto temp{ value.get<T>() };
		changed |= ImGui::DragScalar(input_label, ImGuiDataType_Float, &temp, 1.f, 0, 0, "%f", ImGuiSliderFlags_None);
		if (changed) { value = temp; }
		return changed;
	}

	bool json_editor::draw_binary(cstring key, json & value)
	{
		ImGui::Text("binary");
		return false;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}