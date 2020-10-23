#include <modus_core/imgui/ImGui.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/detail/FileUtility.hpp>
#include <modus_core/detail/ParserUtil.hpp>

// GLFW / OpenGL3
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool ImGui_Startup(render_window * win, bool callbacks)
	{
		ML_assert(win && win->is_open());

#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
		return ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)win->get_handle(), callbacks)
			&& ImGui_ImplOpenGL3_Init("#version 130");
#else
#endif
	}

	void ImGui_Shutdown(render_window * win, ImGuiContext * ctx)
	{
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
#else
#endif
		ImGui::DestroyContext(ctx);
	}

	void ImGui_NewFrame(render_window * win, ImGuiContext * ctx)
	{
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
#else
#endif
		ImGui::NewFrame();
	}

	void ImGui_RenderFrame(render_window * win, ImGuiContext * ctx)
	{
		ImGui::Render();

		win->execute(
			gfx::command::set_viewport(win->get_framebuffer_size()),
			gfx::command::set_clear_color(colors::black),
			gfx::command::clear(gfx::clear_color));

		ImDrawData * const draw_data{ &ctx->Viewports[0]->DrawDataP };
#if defined(ML_IMPL_RENDERER_OPENGL)
		ImGui_ImplOpenGL3_RenderDrawData(draw_data);
#else
#endif

		if (ctx->IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			auto backup_context{ native_window::get_context_current() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			native_window::make_context_current(backup_context);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool ImGui_LoadStyle(fs::path const & path, ImGuiStyle & ref)
	{
		using namespace util;
	
		// empty
		if (path.empty()) return false;

		// builtin
		switch (hashof(to_lower(path.string())))
		{
		case hashof("classic")	: ImGui::StyleColorsClassic()	; return true;
		case hashof("dark")		: ImGui::StyleColorsDark()		; return true;
		case hashof("light")	: ImGui::StyleColorsLight()		; return true;
		}
	
		// open file
		std::ifstream f{ path };
		ML_defer(&f) { f.close(); };
		if (!f) { return false; }
	
		// scan file
		pmr::string line;
		while (std::getline(f, line))
		{
			if ((line = trim(line)).empty() || line.front() == '#') { continue; }
	
			// scan line
			pmr::stringstream ss{}; ss << line;
			switch (hashof(parse<pmr::string>(ss)))
			{
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			case hashof("ImGuiStyle"): { switch (hashof(parse<pmr::string>(ss)))
			{
			case hashof("Alpha")					: ref.Alpha = parse<float_t>(ss); break;
			case hashof("WindowPadding")			: ref.WindowPadding = parse<vec2>(ss); break;
			case hashof("WindowRounding")			: ref.WindowRounding = parse<float_t>(ss); break;
			case hashof("WindowBorderSize")			: ref.WindowBorderSize = parse<float_t>(ss); break;
			case hashof("WindowMinSize")			: ref.WindowMinSize = parse<vec2>(ss); break;
			case hashof("WindowTitleAlign")			: ref.WindowTitleAlign = parse<vec2>(ss); break;
			case hashof("WindowMenuButtonPosition")	: ref.WindowMenuButtonPosition = parse<int32_t>(ss); break;
			case hashof("ChildRounding")			: ref.ChildRounding = parse<float_t>(ss); break;
			case hashof("ChildBorderSize")			: ref.ChildBorderSize = parse<float_t>(ss); break;
			case hashof("PopupRounding")			: ref.PopupRounding = parse<float_t>(ss); break;
			case hashof("PopupBorderSize")			: ref.PopupBorderSize = parse<float_t>(ss); break;
			case hashof("FramePadding")				: ref.FramePadding = parse<vec2>(ss); break;
			case hashof("FrameRounding")			: ref.FrameRounding = parse<float_t>(ss); break;
			case hashof("FrameBorderSize")			: ref.FrameBorderSize = parse<float_t>(ss); break;
			case hashof("ItemSpacing")				: ref.ItemSpacing = parse<vec2>(ss); break;
			case hashof("ItemInnerSpacing")			: ref.ItemInnerSpacing = parse<vec2>(ss); break;
			case hashof("TouchExtraPadding")		: ref.TouchExtraPadding = parse<vec2>(ss); break;
			case hashof("IndentSpacing")			: ref.IndentSpacing = parse<float_t>(ss); break;
			case hashof("ColumnsMinSpacing")		: ref.ColumnsMinSpacing = parse<float_t>(ss); break;
			case hashof("ScrollbarSize")			: ref.ScrollbarSize = parse<float_t>(ss); break;
			case hashof("ScrollbarRounding")		: ref.ScrollbarRounding = parse<float_t>(ss); break;
			case hashof("GrabMinSize")				: ref.GrabMinSize = parse<float_t>(ss); break;
			case hashof("GrabRounding")				: ref.GrabRounding = parse<float_t>(ss); break;
			case hashof("TabRounding")				: ref.TabRounding = parse<float_t>(ss); break;
			case hashof("TabBorderSize")			: ref.TabBorderSize = parse<float_t>(ss); break;
			case hashof("TabMinWidthForCloseButton"): ref.TabMinWidthForCloseButton = parse<float_t>(ss); break;
			case hashof("ColorButtonPosition")		: ref.ColorButtonPosition = parse<int32_t>(ss); break;
			case hashof("ButtonTextAlign")			: ref.ButtonTextAlign = parse<vec2>(ss); break;
			case hashof("SelectableTextAlign")		: ref.SelectableTextAlign = parse<vec2>(ss); break;
			case hashof("DisplayWindowPadding")		: ref.DisplayWindowPadding = parse<vec2>(ss); break;
			case hashof("DisplaySafeAreaPadding")	: ref.DisplaySafeAreaPadding = parse<vec2>(ss); break;
			case hashof("MouseCursorScale")			: ref.MouseCursorScale = parse<float_t>(ss); break;
			case hashof("AntiAliasedLines")			: ref.AntiAliasedLines = parse<bool>(ss); break;
			case hashof("AntiAliasedFill")			: ref.AntiAliasedFill = parse<bool>(ss); break;
			case hashof("CurveTessellationTol")		: ref.CurveTessellationTol = parse<float_t>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			case hashof("ImGuiCol"): { switch (hashof(parse<pmr::string>(ss)))
			{
			case hashof("Text")					: ref.Colors[ImGuiCol_Text] = parse<vec4>(ss); break;
			case hashof("TextDisabled")			: ref.Colors[ImGuiCol_TextDisabled] = parse<vec4>(ss); break;
			case hashof("WindowBg")				: ref.Colors[ImGuiCol_WindowBg] = parse<vec4>(ss); break;
			case hashof("ChildBg")				: ref.Colors[ImGuiCol_ChildBg] = parse<vec4>(ss); break;
			case hashof("PopupBg")				: ref.Colors[ImGuiCol_PopupBg] = parse<vec4>(ss); break;
			case hashof("Border")				: ref.Colors[ImGuiCol_Border] = parse<vec4>(ss); break;
			case hashof("BorderShadow")			: ref.Colors[ImGuiCol_BorderShadow] = parse<vec4>(ss); break;
			case hashof("FrameBg")				: ref.Colors[ImGuiCol_FrameBg] = parse<vec4>(ss); break;
			case hashof("FrameBgHovered")		: ref.Colors[ImGuiCol_FrameBgHovered] = parse<vec4>(ss); break;
			case hashof("FrameBgActive")		: ref.Colors[ImGuiCol_FrameBgActive] = parse<vec4>(ss); break;
			case hashof("TitleBg")				: ref.Colors[ImGuiCol_TitleBg] = parse<vec4>(ss); break;
			case hashof("TitleBgActive")		: ref.Colors[ImGuiCol_TitleBgActive] = parse<vec4>(ss); break;
			case hashof("TitleBgCollapsed")		: ref.Colors[ImGuiCol_TitleBgCollapsed] = parse<vec4>(ss); break;
			case hashof("MenuBarBg")			: ref.Colors[ImGuiCol_MenuBarBg] = parse<vec4>(ss); break;
			case hashof("ScrollbarBg")			: ref.Colors[ImGuiCol_ScrollbarBg] = parse<vec4>(ss); break;
			case hashof("ScrollbarGrab")		: ref.Colors[ImGuiCol_ScrollbarGrab] = parse<vec4>(ss); break;
			case hashof("ScrollbarGrabHovered")	: ref.Colors[ImGuiCol_ScrollbarGrabHovered] = parse<vec4>(ss); break;
			case hashof("ScrollbarGrabActive")	: ref.Colors[ImGuiCol_ScrollbarGrabActive] = parse<vec4>(ss); break;
			case hashof("CheckMark")			: ref.Colors[ImGuiCol_CheckMark] = parse<vec4>(ss); break;
			case hashof("SliderGrab")			: ref.Colors[ImGuiCol_SliderGrab] = parse<vec4>(ss); break;
			case hashof("SliderGrabActive")		: ref.Colors[ImGuiCol_SliderGrabActive] = parse<vec4>(ss); break;
			case hashof("Button")				: ref.Colors[ImGuiCol_Button] = parse<vec4>(ss); break;
			case hashof("ButtonHovered")		: ref.Colors[ImGuiCol_ButtonHovered] = parse<vec4>(ss); break;
			case hashof("ButtonActive")			: ref.Colors[ImGuiCol_ButtonActive] = parse<vec4>(ss); break;
			case hashof("Header")				: ref.Colors[ImGuiCol_Header] = parse<vec4>(ss); break;
			case hashof("HeaderHovered")		: ref.Colors[ImGuiCol_HeaderHovered] = parse<vec4>(ss); break;
			case hashof("HeaderActive")			: ref.Colors[ImGuiCol_HeaderActive] = parse<vec4>(ss); break;
			case hashof("Separator")			: ref.Colors[ImGuiCol_Separator] = parse<vec4>(ss); break;
			case hashof("SeparatorHovered")		: ref.Colors[ImGuiCol_SeparatorHovered] = parse<vec4>(ss); break;
			case hashof("SeparatorActive")		: ref.Colors[ImGuiCol_SeparatorActive] = parse<vec4>(ss); break;
			case hashof("ResizeGrip")			: ref.Colors[ImGuiCol_ResizeGrip] = parse<vec4>(ss); break;
			case hashof("ResizeGripHovered")	: ref.Colors[ImGuiCol_ResizeGripHovered] = parse<vec4>(ss); break;
			case hashof("ResizeGripActive")		: ref.Colors[ImGuiCol_ResizeGripActive] = parse<vec4>(ss); break;
			case hashof("Tab")					: ref.Colors[ImGuiCol_Tab] = parse<vec4>(ss); break;
			case hashof("TabHovered")			: ref.Colors[ImGuiCol_TabHovered] = parse<vec4>(ss); break;
			case hashof("TabActive")			: ref.Colors[ImGuiCol_TabActive] = parse<vec4>(ss); break;
			case hashof("TabUnfocused")			: ref.Colors[ImGuiCol_TabUnfocused] = parse<vec4>(ss); break;
			case hashof("TabUnfocusedActive")	: ref.Colors[ImGuiCol_TabUnfocusedActive] = parse<vec4>(ss); break;
			case hashof("DockingPreview")		: ref.Colors[ImGuiCol_DockingPreview] = parse<vec4>(ss); break;
			case hashof("DockingEmptyBg")		: ref.Colors[ImGuiCol_DockingEmptyBg] = parse<vec4>(ss); break;
			case hashof("PlotLines")			: ref.Colors[ImGuiCol_PlotLines] = parse<vec4>(ss); break;
			case hashof("PlotLinesHovered")		: ref.Colors[ImGuiCol_PlotLinesHovered] = parse<vec4>(ss); break;
			case hashof("PlotHistogram")		: ref.Colors[ImGuiCol_PlotHistogram] = parse<vec4>(ss); break;
			case hashof("PlotHistogramHovered")	: ref.Colors[ImGuiCol_PlotHistogramHovered] = parse<vec4>(ss); break;
			case hashof("TextSelectedBg")		: ref.Colors[ImGuiCol_TextSelectedBg] = parse<vec4>(ss); break;
			case hashof("DragDropTarget")		: ref.Colors[ImGuiCol_DragDropTarget] = parse<vec4>(ss); break;
			case hashof("NavHighlight")			: ref.Colors[ImGuiCol_NavHighlight] = parse<vec4>(ss); break;
			case hashof("NavWindowingHighlight"): ref.Colors[ImGuiCol_NavWindowingHighlight] = parse<vec4>(ss); break;
			case hashof("NavWindowingDimBg")	: ref.Colors[ImGuiCol_NavWindowingDimBg] = parse<vec4>(ss); break;
			case hashof("ModalWindowDimBg")		: ref.Colors[ImGuiCol_ModalWindowDimBg] = parse<vec4>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			}
		}
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}