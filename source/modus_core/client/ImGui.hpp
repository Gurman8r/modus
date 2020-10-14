#ifndef _ML_IMGUI_HPP_
#define _ML_IMGUI_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/detail/Color.hpp>
#include <modus_core/system/Memory.hpp>

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
	}
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef ML_IMPL_IMGUI_EXTRAS
#include <ImGuizmo/ImGuizmo.h>
#include <ImGuizmo/ImSequencer.h>
#include <ImGuizmo/ImCurveEdit.h>
#include <ImGuiColorTextEdit/TextEditor.h>
#include <imgui_club/imgui_memory_editor/imgui_memory_editor.h>
#include <imgui-node-editor/imgui_node_editor.h>

namespace ml
{
	template <> struct default_delete<ax::NodeEditor::EditorContext>
	{
		void operator()(ax::NodeEditor::EditorContext * ptr)
		{
			ax::NodeEditor::DestroyEditor(ptr);
		}
	};
}
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ImGui::PushID(...); ML_defer() { ImGui::PopID(); };
#define ML_ImGui_ScopeID(...) \
	auto ML_anon = _ML impl::imgui_scoped_id{ ##__VA_ARGS__ }

namespace ml::impl
{
	struct ML_NODISCARD imgui_scoped_id final
	{
		template <class ... Args
		> imgui_scoped_id(Args && ... args) noexcept {
			ImGui::PushID(ML_forward(args)...);
		}

		~imgui_scoped_id() noexcept {
			ImGui::PopID();
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct render_window;

	ML_CORE_API bool ImGui_Startup(render_window * win, bool clbk = true);

	ML_CORE_API void ImGui_Shutdown();

	ML_CORE_API void ImGui_NewFrame();

	ML_CORE_API void ImGui_RenderDrawData(render_window * win, ImDrawData * data);

	template <class Fn, class ... Args
	> void ImGui_DoFrame(render_window * win, ImDrawData * data, Fn && fn, Args && ... args) noexcept
	{
		ImGui_NewFrame();
		std::invoke(ML_forward(fn), ML_forward(args)...);
		ImGui_RenderDrawData(win, data);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_CORE_API bool ImGui_LoadStyle(fs::path const & path, ImGuiStyle & ref = ImGui::GetStyle());

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_IMGUI_HPP_