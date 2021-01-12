#ifndef _ML_SANDBOX_HPP_
#define _ML_SANDBOX_HPP_

#include <modus_core/detail/FileUtility.hpp>
#include <modus_core/detail/StreamSniper.hpp>
#include <modus_core/embed/Python.hpp>
#include <modus_core/graphics/Material.hpp>
#include <modus_core/graphics/Mesh.hpp>
#include <modus_core/gui/Terminal.hpp>
#include <modus_core/runtime/Application.hpp>
#include <modus_core/scene/Components.hpp>

// editors
#include <modus_core/editor/CameraEditor.hpp>
#include <modus_core/editor/JsonEditor.hpp>
#include <modus_core/editor/SceneEditor.hpp>
#include <modus_core/editor/TransformEditor.hpp>

// events
#include <modus_core/events/EditorEvents.hpp>
#include <modus_core/events/InputEvents.hpp>
#include <modus_core/events/RuntimeEvents.hpp>
#include <modus_core/events/SceneEvents.hpp>
#include <modus_core/events/SystemEvents.hpp>
#include <modus_core/events/WindowEvents.hpp>

namespace ml
{
	static void edit_node(ref<node> const & value, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None)
	{
		if (!value) { return; }
		ImGui_Scope(value.get());

		ImGuiContext &	g		{ *ImGui::GetCurrentContext() };
		ImGuiIO &		io		{ g.IO };
		ImGuiStyle &	style	{ g.Style };
		ImGuiWindow *	window	{ g.CurrentWindow };
		if (window->SkipItems) { return; }

		bool const is_root{ !value->get_parent() };
		bool const is_leaf{ 0 == value->get_child_count() };
		cstring const node_name{ value->get_name().c_str() };
		size_t const child_count{ value->get_child_count() };
		size_t const sibling_count{ value->get_sibling_count() };
		size_t const sibling_index{ value->get_sibling_index() };

		ImGuiID const node_id{ window->GetID(value.get()) };
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
			if (ImGui::BeginMenu("create")) {
				if (ImGui::MenuItem("empty", "", false)) { value->new_child<node>("New Entity"); }
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("clear children", "", false, !is_leaf)) { value->clear_children(); }
			if (ImGui::MenuItem("detach children", "", false, !is_root && !is_leaf)) { value->detatch_children(); }
			if (ImGui::MenuItem("move up", "", false, !is_root && (sibling_index > 0))) {
				value->set_sibling_index(sibling_index - 1);
			}
			if (ImGui::MenuItem("move down", "", false, !is_root && (sibling_index < sibling_count - 1))) {
				value->set_sibling_index(sibling_index + 1);
			}
			ImGui::EndPopup();
		}

		// drag source
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
			ImGui::SetDragDropPayload("DND_NODE_REF", &value, sizeof(ref<node>));
			ImGui::Text("%s", node_name);
			ImGui::EndDragDropSource();
		}

		// drop onto
		if (ImGui::BeginDragDropTarget()) {
			if (ImGuiPayload const * payload{ ImGui::AcceptDragDropPayload("DND_NODE_REF") }) {
				ML_assert(payload->DataSize == sizeof(ref<node>));
				if (ref<node> holding{ *(ref<node> *)payload->Data }) {
					holding->set_parent(value);
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
					if (ImGuiPayload const * payload{ ImGui::AcceptDragDropPayload("DND_NODE_REF") }) {
						ML_assert(payload->DataSize == sizeof(ref<node>));
						if (ref<node> holding{ *(ref<node> *)payload->Data }) {
							holding->set_parent(value);
							holding->set_sibling_index(0);
						}
					}
					ImGui::EndDragDropTarget();
				}
			}

			for (size_t i = 0; i < child_count; ++i)
			{
				edit_node(value->get_child(i), flags);

				// drop after
				if (g.DragDropActive && (i < child_count - 1)) {
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
					if (ImGui::BeginDragDropTarget()) {
						if (ImGuiPayload const * payload{ ImGui::AcceptDragDropPayload("DND_NODE_REF") }) {
							ML_assert(payload->DataSize == sizeof(ref<node>));
							if (ref<node> holding{ *(ref<node> *)payload->Data }) {
								holding->set_parent(value);
								holding->set_sibling_index(i);
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

#endif // !_ML_SANDBOX_HPP_