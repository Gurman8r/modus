#ifndef _ML_TRANSFORM_EDITOR_HPP_
#define _ML_TRANSFORM_EDITOR_HPP_

#include <modus_core/gui/ImGui.hpp>

namespace ml::ImGuiExt
{
	inline bool EditVec3(cstring label, float32 * value, float32 spd = 0.001f, float32 min = 0.f, float32 max = 0.f, cstring fmt = "%.3f", float32 reset_value = 0.f, float32 column_width = 100.f)
	{
		ImGui_Scope(label);
		bool dirty{};
		ImGuiIO & io{ ImGui::GetIO() };

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		auto line_height{ ImGui::GetFontSize() + ImGui::GetStyle().FramePadding[1] * 2.0f };
		vec2 button_size{ line_height + 3.0f, line_height };

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, .1f, 0.15f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, .1f, 0.15f, 1.f });
		if (ImGui::Button("X", button_size)) { value[0] = reset_value; }
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##X", &value[0], spd, min, max, fmt);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.f });
		if (ImGui::Button("Y", button_size)) { value[1] = reset_value; }
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##Y", &value[1], spd, min, max, fmt);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { .1f, 0.25f, 0.8f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { .1f, 0.25f, 0.8f, 1.f });
		if (ImGui::Button("Z", button_size)) { value[2] = reset_value; }
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		dirty |= ImGui::DragFloat("##Z", &value[2], spd, min, max, fmt);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		return dirty;
	}

	inline bool EditTransformMatrix(float32 * value, cstring labels = "tr\0rt\0sc", float32 spd = 0.001f, float32 min = 0.f, float32 max = 0.f, cstring fmt = "%.3f", float32 reset_value = 0.f, float32 column_width = 100.f)
	{
		if (!labels || !*labels) { labels = "tr\0rt\0sc"; } // default labels

		bool dirty{};
		vec3 t, r, s;
		ImGuizmo::DecomposeMatrixToComponents(value, t, r, s);
		dirty |= ImGuiExt::EditVec3(util::single_str(labels, 0), t, spd, min, max, fmt, reset_value, column_width);
		dirty |= ImGuiExt::EditVec3(util::single_str(labels, 1), r, spd, min, max, fmt, reset_value, column_width);
		dirty |= ImGuiExt::EditVec3(util::single_str(labels, 2), s, spd, min, max, fmt, reset_value, column_width);
		ImGuizmo::RecomposeMatrixFromComponents(t, r, s, value);
		return dirty;
	}

	struct ML_NODISCARD TransformEditor final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ImGuizmo::OPERATION	Operation		{ ImGuizmo::TRANSLATE };
		ImGuizmo::MODE		Mode			{ ImGuizmo::LOCAL };
		bool				UseSnap			{ false };
		vec3				Snap			{ 1.f, 1.f, 1.f };
		vec3				Bounds[2]		{ { -.5f, -.5f, -.5f }, { .5f, .5f, .5f } };
		vec3				BoundsSnap		{ .1f, .1f, .1f };
		bool				BoundSizing		{ false };
		bool				BoundSizingSnap	{ false };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool Manipulate(float32 const * view, float32 const * proj, float32 * value, float32 * delta = {})
		{
			ImGui_Scope(this);
			return ImGuizmo::Manipulate
			(
				view,
				proj,
				Operation,
				Mode,
				value,
				delta,
				UseSnap ? (float32 *)Snap : nullptr,
				BoundSizing ? (float32 *)Bounds : nullptr,
				BoundSizingSnap ? (float32 *)BoundsSnap : nullptr
			);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void ShowOperationControls()
		{
			ImGui_Scope(this);
			ImGui::BeginGroup();
			if (ImGui::RadioButton("translate", Operation == ImGuizmo::TRANSLATE)) { Operation = ImGuizmo::TRANSLATE; }
			ImGui::SameLine();
			if (ImGui::RadioButton("rotate", Operation == ImGuizmo::ROTATE)) { Operation = ImGuizmo::ROTATE; }
			ImGui::SameLine();
			if (ImGui::RadioButton("scale", Operation == ImGuizmo::SCALE)) { Operation = ImGuizmo::SCALE; }
			ImGui::EndGroup();
		}

		void ShowModeControls()
		{
			ImGui_Scope(this);
			bool const is_scale{ Operation == ImGuizmo::SCALE };
			ImGui::BeginGroup();
			if (ImGui::RadioButton("local", !is_scale && Mode == ImGuizmo::LOCAL)) { Mode = ImGuizmo::LOCAL; }
			ImGui::SameLine();
			if (ImGui::RadioButton("world", !is_scale && Mode == ImGuizmo::WORLD)) { Mode = ImGuizmo::WORLD; }
			ImGui::EndGroup();
		}

		void ShowSnapControls(float32 speed = 0.01f)
		{
			ImGui_Scope(this);
			ImGui::BeginGroup();
			ImGui::Checkbox("##usesnap", &UseSnap); ImGui::SameLine();
			switch (Operation)
			{
			case ImGuizmo::TRANSLATE: ImGui::DragFloat3("snap##translate", &Snap[0], speed); break;
			case ImGuizmo::ROTATE: ImGui::DragFloat("snap##rotate", &Snap[0], speed); break;
			case ImGuizmo::SCALE: ImGui::DragFloat("snap##scale", &Snap[0], speed); break;
			}
			ImGui::EndGroup();
		}

		void ShowBoundsControls(float32 speed = 0.01f)
		{
			ImGui_Scope(this);
			ImGui::BeginGroup();
			if (ImGui::Checkbox("bound sizing", &BoundSizing); BoundSizing)
			{
				ImGui::Checkbox("##boundsizingsnap", &BoundSizingSnap); ImGui::SameLine();
				ImGui::DragFloat3("snap##boundsizing", BoundsSnap, speed, 0.f, 0.f, "%.2f");
			}
			ImGui::EndGroup();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_TRANSFORM_EDITOR_HPP_