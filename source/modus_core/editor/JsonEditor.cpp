#include <modus_core/editor/JsonEditor.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void json_editor::draw_contents(ImGuiTreeNodeFlags flags)
	{
		ImGuiWindow * const current_window{ ImGui::GetCurrentWindow() };
		if (!current_window || current_window->SkipItems) { return; }
		
		ImGui::Columns(1 + m_show_values);
		if (m_root) { draw_elements("", *m_root, flags); }
		ImGui::Columns(1);
	}

	void json_editor::draw_elements(cstring key, json & value, ImGuiTreeNodeFlags flags)
	{
		ImGuiWindow * const current_window{ ImGui::GetCurrentWindow() };
		if (!current_window || current_window->SkipItems) { return; }
		
		ImGui_Scope(&value);
		ImGui::BeginGroup();
		for (iterator it = value.begin(); it != value.end(); ++it) {
			int32 const index{ (int32)std::distance(value.begin(), it) };
			ImGui_Scope(index);
			char item_label[32]{};
			if (value.is_object()) { std::sprintf(item_label, "%s", it.key().c_str()); }
			else if (value.is_array()) { std::sprintf(item_label, "%s[%i]", key, index); }
			draw_field(item_label, it.value(), flags);
		}
		ImGui::EndGroup();
	}

	void json_editor::draw_field(cstring key, json & value, ImGuiTreeNodeFlags flags)
	{
		ImGuiWindow * const current_window{ ImGui::GetCurrentWindow() };
		if (!current_window || current_window->SkipItems) { return; }

		json * const	item_addr{ std::addressof(value) };
		value_t const	item_type{ value.type() };
		cstring const	type_name{ json_editor::get_type_name(item_type) };
		bool const
			is_object			{ item_type == value_t::object },
			is_array			{ item_type == value_t::array },
			is_leaf				{ !is_object && !is_array },
			is_null				{ item_type == value_t::null },
			is_string			{ item_type == value_t::string },
			is_boolean			{ item_type == value_t::boolean },
			is_number_integer	{ item_type == value_t::number_integer },
			is_number_unsigned	{ item_type == value_t::number_unsigned },
			is_number_float		{ item_type == value_t::number_float },
			is_number			{ is_number_integer || is_number_unsigned || is_number_float },
			is_binary			{ item_type == value_t::binary },
			is_discarded		{ item_type == value_t::discarded },
			is_root				{ m_root == item_addr },
			is_selected			{ m_selected == item_addr };
		ImGuiTreeNodeFlags const node_flags{
			(is_root ? ImGuiTreeNodeFlags_DefaultOpen : 0) |
			(is_leaf ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet : 0) |
			(is_selected ? ImGuiTreeNodeFlags_Selected : 0) |
			flags 
		};
		ImGuiID const node_id{ current_window->GetID(type_name) };
		char node_label[32]{}; std::sprintf(node_label, "%s", key);
		
		// KEY
		bool const node_open{ ImGui::TreeNodeBehavior(node_id, node_flags, node_label) };
		if (ImGui::IsItemHovered()) {
			if (on_item_hovered) { on_item_hovered(this, key, item_addr); }
			else {
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.f);
				ImGui::TextDisabled("%s: %s", key, type_name);
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			for (int32 i = 0; i < ImGuiMouseButton_COUNT; ++i) {
				if (ImGui::IsMouseClicked(i)) {
					if (on_item_clicked) { on_item_clicked(this, key, item_addr, i); }
					if (!is_selected) {
						m_selected = item_addr;
						if (on_item_selected) { on_item_selected(this, key, item_addr); }
					}
				}
			}
		}
		if (ImGui::BeginPopupContextItem("popup_context")) {
			if (on_item_popup_context) { on_item_popup_context(this, key, item_addr); }
			else {
				ImGui::TextDisabled("%s: %s", key, type_name); ImGui::Separator();
				if (ImGui::MenuItem("null", NULL, is_null, !is_null)) {}
				if (ImGui::MenuItem("object", NULL, is_object, !is_object)) {}
				if (ImGui::MenuItem("array", NULL, is_array, !is_array)) {}
				if (ImGui::MenuItem("string", NULL, is_string, !is_string)) {}
				if (ImGui::MenuItem("boolean", NULL, is_boolean, !is_boolean)) {}
				if (ImGui::MenuItem("integer", NULL, is_number_integer, !is_number_integer)) {}
				if (ImGui::MenuItem("unsigned", NULL, is_number_unsigned, !is_number_unsigned)) {}
				if (ImGui::MenuItem("float", NULL, is_number_float, !is_number_float)) {}
				if (ImGui::MenuItem("binary", NULL, is_binary, !is_binary)) {}
				if (ImGui::MenuItem("discarded", NULL, is_discarded, !is_discarded)) {}
			}
			ImGui::EndPopup();
		}
		
		// VALUE
		if (m_show_values) {
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-1);
			if (flags & ImGuiTreeNodeFlags_FramePadding) { ImGui::AlignTextToFramePadding(); }
			if (on_item_repr) { on_item_repr(this, key, item_addr); }
			else { repr_auto(key, value); }
			ImGui::NextColumn();
		}
		if (!is_leaf && node_open) {
			draw_elements(key, value, flags);
			ImGui::TreePop();
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool json_editor::repr_auto(cstring key, json & value)
	{
		switch (value.type())
		{
		default							: return false;
		case value_t::null				: return repr_null(key, value);
		case value_t::object			: return repr_object(key, value);
		case value_t::array				: return repr_array(key, value);
		case value_t::string			: return repr_string(key, value);
		case value_t::boolean			: return repr_boolean(key, value);
		case value_t::number_unsigned	:
		case value_t::number_integer	: return repr_number_integer(key, value);
		case value_t::number_float		: return repr_number_float(key, value);
		case value_t::binary			: return repr_binary(key, value);
		case value_t::discarded			: return repr_binary(key, value);
		}
	}
	
	bool json_editor::repr_null(cstring key, json & value)
	{
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", json_editor::get_type_name(value), key);
		ImGui::TextDisabled("null");
		return false;
	}

	bool json_editor::repr_object(cstring key, json & value)
	{
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", json_editor::get_type_name(value), key);
		ImGui::TextDisabled("{}");
		return false;
	}

	bool json_editor::repr_array(cstring key, json & value)
	{
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", json_editor::get_type_name(value), key);
		ImGui::TextDisabled("[]");
		return false;
	}

	bool json_editor::repr_string(cstring key, json & value)
	{
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", json_editor::get_type_name(value), key);
		auto temp{ value.get<string_t>() };
		bool changed{};
		ImGui::Text("%.*s", temp.size(), temp.data());
		if (changed) { value = temp; }
		return changed;
	}

	bool json_editor::repr_boolean(cstring key, json & value)
	{
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", json_editor::get_type_name(value), key);
		auto temp{ value.get<boolean_t>() };
		bool changed{};
		changed |= ImGui::Checkbox(input_label, &temp);
		if (changed) { value = temp; }
		return changed;
	}

	bool json_editor::repr_number_integer(cstring key, json & value)
	{
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", json_editor::get_type_name(value), key);
		auto temp{ value.get<number_integer_t>() };
		bool changed{};
		changed |= ImGui::DragScalar(input_label, ImGuiDataType_S64, &temp, 1.f, 0, 0, "%d", ImGuiSliderFlags_None);
		if (changed) { value = temp; }
		return changed;
	}

	bool json_editor::repr_number_unsigned(cstring key, json & value)
	{
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", json_editor::get_type_name(value), key);
		auto temp{ value.get<number_unsigned_t>() };
		bool changed{};
		changed |= ImGui::DragScalar(input_label, ImGuiDataType_U64, &temp, 1.f, 0, 0, "%d", ImGuiSliderFlags_None);
		if (changed) { value = temp; }
		return changed;
	}

	bool json_editor::repr_number_float(cstring key, json & value)
	{
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", json_editor::get_type_name(value), key);
		auto temp{ value.get<number_float_t>() };
		bool changed{};
		changed |= ImGui::DragScalar(input_label, ImGuiDataType_Double, &temp, 1.f, 0, 0, "%d", ImGuiSliderFlags_None);
		if (changed) { value = temp; }
		return changed;
	}

	bool json_editor::repr_binary(cstring key, json & value)
	{
		char input_label[32]{}; std::sprintf(input_label, "##%s_%s", json_editor::get_type_name(value), key);
		auto temp{ value.get<binary_t>() };
		ImGui::Text("binary");
		return false;
	}

	bool json_editor::repr_discarded(cstring key, json & value)
	{
		ImGui::TextDisabled("discarded");
		return false;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}