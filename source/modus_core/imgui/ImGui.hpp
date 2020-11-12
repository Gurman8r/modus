#ifndef _ML_IMGUI_HPP_
#define _ML_IMGUI_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/detail/Color.hpp>
#include <modus_core/window/WindowContext.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef IM_VEC2_CLASS_EXTRA
#define IM_VEC2_CLASS_EXTRA								\
	constexpr ImVec2(_ML vec2 const & v) noexcept		\
		: x{ v[0] }, y{ v[1] }							\
	{}													\
	constexpr operator _ML vec2() const noexcept {		\
		return { x, y };								\
	}
#endif

#ifndef IM_VEC4_CLASS_EXTRA
#define IM_VEC4_CLASS_EXTRA								\
	constexpr ImVec4(_ML vec4 const & v) noexcept		\
		: x{ v[0] }, y{ v[1] }, z{ v[2] }, w{ v[3] }	\
	{}													\
	constexpr ImVec4(_ML color const & c) noexcept		\
		: x{ c[0] }, y{ c[1] }, z{ c[2] }, w{ c[3] }	\
	{}													\
	constexpr operator _ML vec4() const noexcept {		\
		return { x, y, z, w };							\
	}													\
	constexpr operator _ML color() const noexcept {		\
		return { x, y, z, w };							\
	}
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui_club/imgui_memory_editor/imgui_memory_editor.h>

#ifdef ML_IMPL_IMGUI_EXTRAS
#include <ImGuizmo/ImGuizmo.h>
#include <ImGuizmo/ImSequencer.h>
#include <ImGuizmo/ImCurveEdit.h>
#include <ImGuiColorTextEdit/TextEditor.h>
#include <imgui-node-editor/imgui_node_editor.h>
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_CORE_API bool ImGui_Init(window_handle window, bool callbacks = true);

	ML_CORE_API void ImGui_Shutdown();

	ML_CORE_API void ImGui_NewFrame();

	ML_CORE_API void ImGui_RenderDrawData(ImDrawData * draw_data);

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_CORE_API bool ImGui_LoadStyle(fs::path const & path, ImGuiStyle & ref = ImGui::GetStyle());

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_IMGUI_HPP_