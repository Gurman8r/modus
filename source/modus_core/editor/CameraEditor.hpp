#ifndef _ML_CAMERA_EDITOR_HPP_
#define _ML_CAMERA_EDITOR_HPP_

#include <modus_core/graphics/Camera.hpp>
#include <modus_core/graphics/RenderUtility.hpp>
#include <modus_core/editor/TransformEditor.hpp>

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

#endif // !_ML_CAMERA_EDITOR_HPP_