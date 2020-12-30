#include <modus_core/editor/JsonEditor.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool show_json(json_editor *, std::string const &, json &);
	bool show_object(json_editor *, std::string const &, json &);
	bool show_array(json_editor *, std::string const &, json &);
	bool show_string(json_editor *, std::string const &, json &);
	bool show_number(json_editor *, std::string const &, json &);
	bool show_number_float(json_editor *, std::string const &, json &);
	bool show_number_integer(json_editor *, std::string const &, json &);
	bool show_number_unsigned(json_editor *, std::string const &, json &);
	bool show_null(json_editor *, std::string const &, json &);

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool show_json_editor(json_editor * p_editor)
	{
		if (!p_editor) { return false; }
		ImGui_Scope(p_editor);
		bool changed{};
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2, 2 });
		ImGui::Columns(2);
		ImGui::Separator();
		changed |= show_json(p_editor, p_editor->label, *p_editor->context);
		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
		return true;
	}

	bool show_json_editor(cstring title, json_editor * p_editor, bool * p_open, ImGuiWindowFlags window_flags)
	{
		ML_defer(&) { ImGui::End(); };

		bool const is_open{ ImGui::Begin(title, p_open, window_flags) };

		if (is_open) { show_json_editor(p_editor); }

		return is_open;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool show_json(json_editor * p_editor, std::string const & key, json & value)
	{
		if (value.is_object()) { return show_object(p_editor, key, value); }
		else if (value.is_array()) { return show_array(p_editor, key, value); }
		else if (value.is_string()) { return show_string(p_editor, key, value); }
		else if (value.is_number()) { return show_number(p_editor, key, value); }
		else if (value.is_null()) { return show_null(p_editor, key, value); }
		else { return false; }
	}

	bool show_object(json_editor * p_editor, std::string const & key, json & value)
	{
		bool changed{};
		ImGui::AlignTextToFramePadding();
		bool node_open{ ImGui::TreeNode("object_field", "%.*s", key.size(), key.data()) };
		ImGui::NextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("object");
		ImGui::NextColumn();
		if (node_open)
		{
			for (json::iterator it = value.begin(); it != value.end(); ++it)
			{
				ptrdiff_t const i{ std::distance(value.begin(), it) };
				ImGui_Scope(i);
				char label[80]{}; std::sprintf(label, "%.*s", it.key().size(), it.key().data());
				changed |= show_json(p_editor, label, it.value());
			}
			ImGui::TreePop();
		}
		return changed;
	}

	bool show_array(json_editor * p_editor, std::string const & key, json & value)
	{
		bool changed{};
		ImGui::AlignTextToFramePadding();
		bool node_open{ ImGui::TreeNode("array_field", "%.*s", key.size(), key.data()) };
		ImGui::NextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("array");
		ImGui::NextColumn();
		if (node_open)
		{
			for (json::iterator it = value.begin(); it != value.end(); ++it)
			{
				ptrdiff_t const i{ std::distance(value.begin(), it) };
				ImGui_Scope(i);
				char label[8]{}; std::sprintf(label, "[%i]", i);
				changed |= show_json(p_editor, label, it.value());
			}
			ImGui::TreePop();
		}
		return changed;
	}

	bool show_string(json_editor * p_editor, std::string const & key, json & value)
	{
		bool changed{};
		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet };
		ImGui::TreeNodeEx("string_field", flags, "%.*s", key.size(), key.data());
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);
		{
			ds::string temp{ value };
			ImGui::Text("\"%.*s\"", temp.size(), temp.data());
			if (changed) {
				value = temp;
			}
		}
		ImGui::NextColumn();
		return changed;
	}

	bool show_number(json_editor * p_editor, std::string const & key, json & value)
	{
		return (value.is_number_float()
			? show_number_float(p_editor, key, value)
			: (value.is_number_integer()
				? show_number_integer(p_editor, key, value)
				: show_number_unsigned(p_editor, key, value)));
	}

	bool show_number_float(json_editor * p_editor, std::string const & key, json & value)
	{
		bool changed{};
		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet };
		ImGui::TreeNodeEx("float_field", flags, "%.*s", key.size(), key.data());
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);
		{
			float32 temp{ value };
			char label[32]; std::sprintf(label, "##%.*s", key.size(), key.data());
			changed |= ImGui::DragScalar(
				label,
				ImGuiDataType_Float,
				&temp,
				1.f, 0, 0, "%f",
				ImGuiSliderFlags_None);
			if (changed) {
				value = temp;
			}
		}
		ImGui::NextColumn();
		return changed;
	}

	bool show_number_integer(json_editor * p_editor, std::string const & key, json & value)
	{
		bool changed{};
		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet };
		ImGui::TreeNodeEx("integer_field", flags, "%.*s", key.size(), key.data());
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);
		{
			int32 temp{ value };
			char label[32]; std::sprintf(label, "##%.*s", key.size(), key.data());
			changed |= ImGui::DragScalar(
				label,
				ImGuiDataType_S32,
				&temp,
				1.f, 0, 0, "%i",
				ImGuiSliderFlags_None);
			if (changed) {
				value = temp;
			}
		}
		ImGui::NextColumn();
		return changed;
	}

	bool show_number_unsigned(json_editor * p_editor, std::string const & key, json & value)
	{
		bool changed{};
		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet };
		ImGui::TreeNodeEx("unsigned_field", flags, "%.*s", key.size(), key.data());
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);
		{
			uint32 temp{ value };
			char label[32]; std::sprintf(label, "##%.*s", key.size(), key.data());
			changed |= ImGui::DragScalar(
				label,
				ImGuiDataType_U32,
				&temp,
				1.f, 0, 0, "%u",
				ImGuiSliderFlags_None);
			if (changed) {
				value = temp;
			}
		}
		ImGui::NextColumn();
		return changed;
	}

	bool show_null(json_editor * p_editor, std::string const & key, json & value)
	{
		bool changed{};
		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet };
		ImGui::TreeNodeEx("null_field", flags, "%.*s", key.size(), key.data());
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);
		{
			char label[32]; std::sprintf(label, "##%.*s", key.size(), key.data());
			ImGui::TextDisabled("null");
		}
		ImGui::NextColumn();
		return changed;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}