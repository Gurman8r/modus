#ifndef _ML_SANDBOX_HPP_
#define _ML_SANDBOX_HPP_

#include <modus_core/detail/FileUtility.hpp>
#include <modus_core/detail/StreamSniper.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/graphics/Material.hpp>
#include <modus_core/graphics/Mesh.hpp>
#include <modus_core/gui/Widgets.hpp>
#include <modus_core/gui/Terminal.hpp>
#include <modus_core/runtime/Application.hpp>
#include <modus_core/scene/Components.hpp>
#include <modus_core/scene/TreeNode.hpp>
#include <modus_core/editor/JsonEditor.hpp>
#include <modus_core/editor/SceneEditor.hpp>

#include <modus_core/events/RuntimeEvents.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/SceneEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>

namespace ml
{
	static void edit_tree_node(tree_node * value, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None)
	{
		if (!value) { return; }
		ImGui_Scope(value);

		ImGuiContext &	g		{ *ImGui::GetCurrentContext() };
		ImGuiIO &		io		{ g.IO };
		ImGuiStyle &	style	{ g.Style };
		ImGuiWindow *	window	{ g.CurrentWindow };
		if (window->SkipItems) { return; }

		bool const is_root{ value->is_root() }, is_leaf{ value->is_leaf() };
		cstring node_name{ value->get_name().c_str() };
		size_t const child_count{ value->get_child_count() };
		size_t const sibling_count{ value->get_sibling_count() };
		size_t const sibling_index{ value->get_sibling_index() };

		ImGuiID const node_id{ window->GetID(value) };

		char node_label[80]{}; std::sprintf(node_label, "%s", node_name);

		ImGuiTreeNodeFlags node_flags{ flags };
		if (is_root) { node_flags |= ImGuiTreeNodeFlags_DefaultOpen; }
		if (is_leaf) { node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet; }

		// tree behavior
		bool const node_open{ ImGui::TreeNodeBehavior(node_id, node_flags, node_label) };

		// tooltip
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.f);
			ImGui::TextDisabled("%s", node_name);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		// context menu
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("new child", "", false)) {
				value->new_child("New Child");
			}
			if (!is_root) {
				if (ImGui::MenuItem("detach children", "", false, !is_leaf)) {
					value->detatch_children();
				}
				if (ImGui::MenuItem("move up", "", false, sibling_index > 0)) {
					value->set_sibling_index(sibling_index - 1);
				}
				if (ImGui::MenuItem("move down", "", false, sibling_index < sibling_count - 1)) {
					value->set_sibling_index(sibling_index + 1);
				}
			}
			ImGui::EndPopup();
		}

		// drag source
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
			ImGui::SetDragDropPayload("DND_TREE_NODE", &value, sizeof(tree_node *));
			ImGui::Text("%s", node_name);
			ImGui::EndDragDropSource();
		}

		// drop onto
		if (ImGui::BeginDragDropTarget()) {
			if (ImGuiPayload const * payload{ ImGui::AcceptDragDropPayload("DND_TREE_NODE") }) {
				ML_assert(payload->DataSize == sizeof(tree_node *));
				if (tree_node * holding{ *(tree_node **)payload->Data }) {
					holding->set_parent(value);
					holding->set_as_last_sibling();
				}
			}
			ImGui::EndDragDropTarget();
		}

		// node children
		if (node_open && !is_leaf)
		{
			// drop before
			if (g.DragDropActive) {
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				if (ImGui::BeginDragDropTarget()) {
					if (ImGuiPayload const * payload{ ImGui::AcceptDragDropPayload("DND_TREE_NODE") }) {
						ML_assert(payload->DataSize == sizeof(tree_node *));
						if (tree_node * holding{ *(tree_node **)payload->Data }) {
							holding->set_parent(value);
							holding->set_as_first_sibling();
						}
					}
					ImGui::EndDragDropTarget();
				}
			}

			for (size_t i = 0; i < child_count; ++i)
			{
				tree_node * child{ value->get_child(i) };

				edit_tree_node(child, flags);

				// drop after
				if (g.DragDropActive && (i < child_count - 1)) {
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
					if (ImGui::BeginDragDropTarget()) {
						if (ImGuiPayload const * payload{ ImGui::AcceptDragDropPayload("DND_TREE_NODE") }) {
							ML_assert(payload->DataSize == sizeof(tree_node *));
							if (tree_node * holding{ *(tree_node **)payload->Data }) {
								holding->set_parent(value);
								holding->set_sibling_index(i + 1);
							}
						}
						ImGui::EndDragDropTarget();
					}
				}
			}

			ImGui::TreePop();
		}
	}
}

namespace ml
{
	inline void edit_camera(camera & value)
	{
		ImGui_Scope(&value);

		// clear flags
		auto cf{ value.get_clear_flags() };
		ImGui::TextDisabled("clear flags");
		if (ImGui::CheckboxFlags("color##buffer bit", &cf, gfx::clear_flags_color)) {
			value.set_clear_flags(cf);
		}
		ImGuiExt::Tooltip("color buffer bit");
		ImGui::SameLine();
		if (ImGui::CheckboxFlags("depth##buffer bit", &cf, gfx::clear_flags_depth)) {
			value.set_clear_flags(cf);
		}
		ImGuiExt::Tooltip("depth buffer bit");
		ImGui::SameLine();
		if (ImGui::CheckboxFlags("stencil##buffer bit", &cf, gfx::clear_flags_stencil)) {
			value.set_clear_flags(cf);
		}
		ImGuiExt::Tooltip("stencil buffer bit");

		// background
		if (auto bg{ value.get_background() }
		; ImGui::ColorEdit4("background", bg, ImGuiColorEditFlags_NoInputs)) {
			value.set_background(bg);
		}
		ImGuiExt::Tooltip("clear color");
		ImGui::Separator();

		// projection
		ImGui::TextDisabled("projection");
		bool is_ortho{ value.is_orthographic() };
		if (ImGui::RadioButton("perspective", !is_ortho)) {
			value.set_orthographic(false);
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("orthographic", is_ortho)) {
			value.set_orthographic(true);
		}
		if (auto fov{ value.get_fov() }
		; ImGui::DragFloat("field of view", &fov, .1f, 0.f, 180.f)) {
			value.set_fov(fov);
		}
		if (auto clip{ value.get_clip() }
		; ImGui::DragFloat2("clipping planes", clip)) {
			value.set_clip(clip);
		}
		ImGui::Separator();

		// view
		ImGui::TextDisabled("view");
		if (auto eye{ value.get_eye() }
		; ImGuiExt::EditVec3("eye", eye, .1f, 0.f, 0.f, "%.1f", 0.f, 56.f)) {
			value.set_eye(eye);
		}
		if (auto target{ value.get_target() }
		; ImGuiExt::EditVec3("target", target, .1f, 0.f, 0.f, "%.1f", 0.f, 56.f)) {
			value.set_target(target);
		}
		if (auto up{ value.get_up() }
		; ImGuiExt::EditVec3("up", up, .1f, 0.f, 0.f, "%.1f", 0.f, 56.f)) {
			value.set_up(up);
		}
		ImGui::Separator();
	}

	inline void edit_camera_controller(camera_controller & value)
	{
		ImGui_Scope(&value);

		// controller
		ImGui::TextDisabled("controller");
		if (auto pos{ value.get_position() }
		; ImGui::DragFloat3("position", pos, .1f, 0.f, 0.f, "%.1f")) {
			value.set_position(pos);
		}
		if (auto yaw{ value.get_yaw() }
		; ImGui::DragFloat("yaw", &yaw, .1f, 0.f, 0.f, "%.1f")) {
			value.set_yaw(yaw);
		}
		if (auto pitch{ value.get_pitch() }
		; ImGui::DragFloat("pitch", &pitch, .1f, 0.f, 0.f, "%.1f")) {
			value.set_pitch(pitch);
		}
		if (auto roll{ value.get_roll() }
		; ImGui::DragFloat("roll", &roll, .1f, 0.f, 0.f, "%.1f")) {
			value.set_roll(roll);
		}
		if (auto zoom{ value.get_zoom() }
		; ImGui::DragFloat("zoom", &zoom, .1f, FLT_MIN, 100.f, "%.1f")) {
			value.set_zoom(zoom);
		}
		ImGui::Separator();
	}
}

#endif // !_ML_SANDBOX_HPP_