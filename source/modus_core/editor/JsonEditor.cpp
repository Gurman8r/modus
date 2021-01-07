#include <modus_core/editor/JsonEditor.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void json_editor::draw(ImGuiTreeNodeFlags flags)
	{
		ImGuiWindow * const current_window{ ImGui::GetCurrentWindow() };
		if (!current_window || current_window->SkipItems) { return; }
		
		ImGui::PushID(this);
		ImGui::Columns(1 + m_show_values);
		if (m_root) { draw_elements("", *m_root, flags); }
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void json_editor::draw_elements(cstring key, json & value, ImGuiTreeNodeFlags flags)
	{
		ImGuiWindow * const current_window{ ImGui::GetCurrentWindow() };
		if (!current_window || current_window->SkipItems) { return; }
		
		bool const is_object{ value.is_object() }, is_array{ value.is_array() };
		if (!is_object && !is_array) { return; }

		ImGui::PushID(&value);
		ImGui::BeginGroup();
		for (iterator it = value.begin(); it != value.end(); ++it) {
			ImGui::PushID(&it.value());
			int32 const index{ (int32)std::distance(value.begin(), it) };
			char item_label[32]{};
			if (is_object) { std::sprintf(item_label, "%s", it.key().c_str()); }
			else if (is_array) { std::sprintf(item_label, "[%i]", index); }
			else { std::sprintf(item_label, "?"); }
			draw_field(item_label, it.value(), flags);
			ImGui::PopID();
		}
		ImGui::EndGroup();
		ImGui::PopID();
	}

	void json_editor::draw_field(cstring key, json & value, ImGuiTreeNodeFlags flags)
	{
		ImGuiWindow * const current_window{ ImGui::GetCurrentWindow() };
		if (!current_window || current_window->SkipItems) { return; }

		// INFO
		json * const	item_addr{ std::addressof(value) };
		value_t const	item_type{ value.type() };
		cstring const	type_name{ json_editor::get_type_name(item_type) };
		bool const
			is_root		{ m_root == item_addr },
			is_selected	{ m_selected == item_addr },
			is_object	{ item_type == value_t::object },
			is_array	{ item_type == value_t::array },
			is_leaf		{ !is_object && !is_array };
		
		// NODE
		ImGuiTreeNodeFlags const node_flags{
			(is_root ? ImGuiTreeNodeFlags_DefaultOpen : 0) |
			(is_leaf ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet : 0) |
			(is_selected ? ImGuiTreeNodeFlags_Selected : 0) |
			flags 
		};
		ImGuiID const node_id{ current_window->GetID(type_name) };
		char node_label[32]{}; std::sprintf(node_label, "%s", key);
		bool const node_open{ node_behavior(node_id, node_flags, node_label) };
		if (ImGui::IsItemHovered()) {
			if (on_item_hovered) { on_item_hovered(this, key, item_addr); }
			else
			{
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
			else
			{
				ImGui::TextDisabled("%s: %s", key, type_name);
				ImGui::Separator();
				if (ImGui::MenuItem("null", "", item_type == value_t::null, item_type != value_t::null)) {}
				if (ImGui::MenuItem("object", "", item_type == value_t::object, item_type != value_t::object)) {}
				if (ImGui::MenuItem("array", "", item_type == value_t::array, item_type != value_t::array)) {}
				if (ImGui::MenuItem("string", "", item_type == value_t::string, item_type != value_t::string)) {}
				if (ImGui::MenuItem("boolean", "", item_type == value_t::boolean, item_type != value_t::boolean)) {}
				if (ImGui::MenuItem("integer", "", item_type == value_t::number_integer, item_type != value_t::number_integer)) {}
				if (ImGui::MenuItem("unsigned", "", item_type == value_t::number_unsigned, item_type != value_t::number_unsigned)) {}
				if (ImGui::MenuItem("float", "", item_type == value_t::number_float, item_type != value_t::number_float)) {}
				if (ImGui::MenuItem("binary", "", item_type == value_t::binary, item_type != value_t::binary)) {}
				if (ImGui::MenuItem("discarded", "", item_type == value_t::discarded, item_type != value_t::discarded)) {}
			}
			ImGui::EndPopup();
		}
		
		// REPR
		if (m_show_values) {
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-1);
			if (flags & ImGuiTreeNodeFlags_FramePadding) { ImGui::AlignTextToFramePadding(); }
			if (on_item_repr) { on_item_repr(this, key, item_addr); }
			else
			{
				repr_auto(key, value);
			}
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
		case value_t::number_integer	: return repr_number_integer(key, value);
		case value_t::number_unsigned	: return repr_number_unsigned(key, value);
		case value_t::number_float		: return repr_number_float(key, value);
		case value_t::binary			: return repr_binary(key, value);
		case value_t::discarded			: return repr_discarded(key, value);
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
		changed |= ImGui::DragScalar(input_label, ImGuiDataType_S64, &temp, 1.f, 0, 0, "%d", ImGuiSliderFlags_None);
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

	bool json_editor::node_behavior(ImGuiID id, ImGuiTreeNodeFlags flags, cstring label, cstring label_end)
	{
		ImGuiWindow * window = ImGui::GetCurrentWindow();
		if (window->SkipItems) { return false; }

		ImGuiContext & g{ *GImGui };
		const ImGuiStyle & style{ g.Style };
		const bool display_frame{ (flags & ImGuiTreeNodeFlags_Framed) != 0 };
		ImVec2 const padding{ (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y)) };

		if (!label_end) {
			label_end = ImGui::FindRenderedTextEnd(label);
		}
		ImVec2 const label_size{ ImGui::CalcTextSize(label, label_end, false) };

		// We vertically grow up to current line height up the typical widget height.
		float32 const frame_height{ ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2) };
		ImRect frame_bb;
		frame_bb.Min.x = (flags & ImGuiTreeNodeFlags_SpanFullWidth) ? window->WorkRect.Min.x : window->DC.CursorPos.x;
		frame_bb.Min.y = window->DC.CursorPos.y;
		frame_bb.Max.x = window->WorkRect.Max.x;
		frame_bb.Max.y = window->DC.CursorPos.y + frame_height;
		if (display_frame) {
			// Framed header expand a little outside the default padding, to the edge of InnerClipRect
			// (FIXME: May remove this at some point and make InnerClipRect align with WindowPadding.x instead of WindowPadding.x*0.5f)
			frame_bb.Min.x -= IM_FLOOR(window->WindowPadding.x * 0.5f - 1.0f);
			frame_bb.Max.x += IM_FLOOR(window->WindowPadding.x * 0.5f);
		}

		float32 const text_offset_x{ g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2) };           // Collapser arrow width + Spacing
		float32 const text_offset_y{ ImMax(padding.y, window->DC.CurrLineTextBaseOffset) };                    // Latch before ItemSize changes it
		float32 const text_width{ g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f) };  // Include collapser
		ImVec2 text_pos(window->DC.CursorPos.x + text_offset_x, window->DC.CursorPos.y + text_offset_y);
		ImGui::ItemSize(ImVec2(text_width, frame_height), padding.y);

		// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
		ImRect interact_bb = frame_bb;
		if (!display_frame && (flags & (ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth)) == 0) {
			interact_bb.Max.x = frame_bb.Min.x + text_width + style.ItemSpacing.x * 2.0f;
		}

		// Store a flag for the current depth to tell if we will allow closing this node when navigating one of its child.
		// For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
		// This is currently only support 32 level deep and we are fine with (1 << Depth) overflowing into a zero.
		const bool is_leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;
		bool is_open = ImGui::TreeNodeBehaviorIsOpen(id, flags);
		if (is_open && !g.NavIdIsAlive && (flags & ImGuiTreeNodeFlags_NavLeftJumpsBackHere) && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
			window->DC.TreeJumpToParentOnPopMask |= (1 << window->DC.TreeDepth);
		}

		bool item_add = ImGui::ItemAdd(interact_bb, id);
		window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
		window->DC.LastItemDisplayRect = frame_bb;

		if (!item_add) {
			if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
				ImGui::TreePushOverrideID(id);
			}
			IMGUI_TEST_ENGINE_ITEM_INFO(window->DC.LastItemId, label, window->DC.ItemFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
			return is_open;
		}

		ImGuiButtonFlags button_flags{ ImGuiTreeNodeFlags_None };
		if (flags & ImGuiTreeNodeFlags_AllowItemOverlap) {
			button_flags |= ImGuiButtonFlags_AllowItemOverlap;
		}
		if (!is_leaf) {
			button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;
		}

		// We allow clicking on the arrow section with keyboard modifiers held, in order to easily
		// allow browsing a tree while preserving selection with code implementing multi-selection patterns.
		// When clicking on the rest of the tree node we always disallow keyboard modifiers.
		float32 const arrow_hit_x1 = (text_pos.x - text_offset_x) - style.TouchExtraPadding.x;
		float32 const arrow_hit_x2 = (text_pos.x - text_offset_x) + (g.FontSize + padding.x * 2.0f) + style.TouchExtraPadding.x;
		const bool is_mouse_x_over_arrow = (g.IO.MousePos.x >= arrow_hit_x1 && g.IO.MousePos.x < arrow_hit_x2);
		if (window != g.HoveredWindow || !is_mouse_x_over_arrow) {
			button_flags |= ImGuiButtonFlags_NoKeyModifiers;
		}

		// Open behaviors can be altered with the _OpenOnArrow and _OnOnDoubleClick flags.
		// Some alteration have subtle effects (e.g. toggle on MouseUp vs MouseDown events) due to requirements for multi-selection and drag and drop support.
		// - Single-click on label = Toggle on MouseUp (default, when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=1)
		// - Double-click on label = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1)
		// - Double-click on arrow = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1 and _OpenOnArrow=0)
		// It is rather standard that arrow click react on Down rather than Up.
		// We set ImGuiButtonFlags_PressedOnClickRelease on OpenOnDoubleClick because we want the item to be active on the initial MouseDown in order for drag and drop to work.
		if (is_mouse_x_over_arrow) {
			button_flags |= ImGuiButtonFlags_PressedOnClick;
		}
		else if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick) {
			button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
		}
		else {
			button_flags |= ImGuiButtonFlags_PressedOnClickRelease;
		}

		bool selected{ (flags & ImGuiTreeNodeFlags_Selected) != 0 };
		const bool was_selected = selected;

		bool hovered, held;
		bool pressed{ ImGui::ButtonBehavior(interact_bb, id, &hovered, &held, button_flags) };
		bool toggled{};
		if (!is_leaf) {
			if (pressed && g.DragDropHoldJustPressedId != id) {
				if ((flags & (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) == 0 || (g.NavActivateId == id)) {
					toggled = true;
				}
				if (flags & ImGuiTreeNodeFlags_OpenOnArrow) {
					toggled |= is_mouse_x_over_arrow && !g.NavDisableMouseHover; // Lightweight equivalent of IsMouseHoveringRect() since ButtonBehavior() already did the job
				}
				if ((flags & ImGuiTreeNodeFlags_OpenOnDoubleClick) && g.IO.MouseDoubleClicked[0])
					toggled = true;
			}
			else if (pressed && g.DragDropHoldJustPressedId == id) {
				IM_ASSERT(button_flags & ImGuiButtonFlags_PressedOnDragDropHold);
				if (!is_open) {
					toggled = true; // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
				}
			}

			if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Left && is_open) {
				toggled = true;
				ImGui::NavMoveRequestCancel();
			}
			
			if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Right && !is_open) {
				// If there's something upcoming on the line we may want to give it the priority?
				toggled = true;
				ImGui::NavMoveRequestCancel();
			}

			if (toggled) {
				is_open = !is_open;
				window->DC.StateStorage->SetInt(id, is_open);
				window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledOpen;
			}
		}
		if (flags & ImGuiTreeNodeFlags_AllowItemOverlap) {
			ImGui::SetItemAllowOverlap();
		}

		// In this branch, TreeNodeBehavior() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) {
			window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection; //-V547
		}

		// Render
		uint32 const text_col{ ImGui::GetColorU32(ImGuiCol_Text) };
		ImGuiNavHighlightFlags nav_highlight_flags = ImGuiNavHighlightFlags_TypeThin;
		if (display_frame) {
			// Framed type
			uint32 const bg_col{ ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header) };
			ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, true, style.FrameRounding);
			ImGui::RenderNavHighlight(frame_bb, id, nav_highlight_flags);
			if (flags & ImGuiTreeNodeFlags_Bullet) {
				ImGui::RenderBullet(window->DrawList, { text_pos.x - text_offset_x * 0.60f, text_pos.y + g.FontSize * 0.5f }, text_col);
			}
			else if (!is_leaf) {
				ImGui::RenderArrow(window->DrawList, { text_pos.x - text_offset_x + padding.x, text_pos.y }, text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 1.0f);
			}
			else {
				text_pos.x -= text_offset_x; // Leaf without bullet, left-adjusted text
			}
				
			if (flags & ImGuiTreeNodeFlags_ClipLabelForTrailingButton) { frame_bb.Max.x -= g.FontSize + style.FramePadding.x; }
			if (g.LogEnabled)
			{
				// NB: '##' is normally used to hide text (as a library-wide feature), so we need to specify the text range to make sure the ## aren't stripped out here.
				const char log_prefix[] = "\n##";
				const char log_suffix[] = "##";
				ImGui::LogRenderedText(&text_pos, log_prefix, log_prefix + 3);
				ImGui::RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
				ImGui::LogRenderedText(&text_pos, log_suffix, log_suffix + 2);
			}
			else
			{
				ImGui::RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
			}
		}
		else {
			// Unframed typed for tree nodes
			if (hovered || selected)
			{
				uint32 const bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
				ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, false);
				ImGui::RenderNavHighlight(frame_bb, id, nav_highlight_flags);
			}
			if (flags & ImGuiTreeNodeFlags_Bullet) {
				ImGui::RenderBullet(window->DrawList, { text_pos.x - text_offset_x * 0.5f, text_pos.y + g.FontSize * 0.5f }, text_col);
			}
			else if (!is_leaf) {
				ImGui::RenderArrow(window->DrawList, { text_pos.x - text_offset_x + padding.x, text_pos.y + g.FontSize * 0.15f }, text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 0.70f);
			}
			if (g.LogEnabled) {
				ImGui::LogRenderedText(&text_pos, ">");
			}
			ImGui::RenderText(text_pos, label, label_end, false);
		}

		if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			ImGui::TreePushOverrideID(id);
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
		return is_open;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}