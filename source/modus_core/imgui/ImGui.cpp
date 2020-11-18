#include <modus_core/imgui/ImGui.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/detail/FileUtility.hpp>
#include <modus_core/detail/ParserUtil.hpp>

// GLFW
#ifdef ML_IMPL_WINDOW_GLFW
#include <imgui/backends/imgui_impl_glfw.h>
#else
#endif

// OpenGL3
#ifdef ML_IMPL_RENDERER_OPENGL
#include <imgui/backends/imgui_impl_opengl3.h>
#else
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool ImGui_Init(window_handle window, bool install_callbacks)
	{
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
		return ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)window, install_callbacks)
			&& ImGui_ImplOpenGL3_Init("#version 130");
#else
#endif
	}

	void ImGui_Shutdown()
	{
#ifdef ML_IMPL_RENDERER_OPENGL
		ImGui_ImplOpenGL3_Shutdown();
#else
#endif

#ifdef ML_IMPL_WINDOW_GLFW
		ImGui_ImplGlfw_Shutdown();
#else
#endif
	}

	void ImGui_NewFrame()
	{
#ifdef ML_IMPL_RENDERER_OPENGL
		ImGui_ImplOpenGL3_NewFrame();
#else
#endif

#ifdef ML_IMPL_WINDOW_GLFW
		ImGui_ImplGlfw_NewFrame();
#else
#endif
	}

	void ImGui_RenderDrawData(ImDrawData * draw_data)
	{
#ifdef ML_IMPL_RENDERER_OPENGL
		ImGui_ImplOpenGL3_RenderDrawData(draw_data);
#else
#endif
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool ImGui_LoadStyle(fs::path const & path, ImGuiStyle & ref)
	{
		// empty
		if (path.empty()) return false;

		// builtin
		switch (hashof(_ML_UTIL to_lower(path.string())))
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
		ds::string line;
		while (std::getline(f, line))
		{
			if ((line = _ML_UTIL trim(line)).empty() || line.front() == '#') { continue; }
	
			// scan line
			ds::stringstream ss{}; ss << line;
			switch (hashof(_ML_UTIL parse<ds::string>(ss)))
			{
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			case hashof("ImGuiStyle"): { switch (hashof(_ML_UTIL parse<ds::string>(ss)))
			{
			case hashof("Alpha")					: ref.Alpha = _ML_UTIL parse<float32>(ss); break;
			case hashof("WindowPadding")			: ref.WindowPadding = _ML_UTIL parse<vec2>(ss); break;
			case hashof("WindowRounding")			: ref.WindowRounding = _ML_UTIL parse<float32>(ss); break;
			case hashof("WindowBorderSize")			: ref.WindowBorderSize = _ML_UTIL parse<float32>(ss); break;
			case hashof("WindowMinSize")			: ref.WindowMinSize = _ML_UTIL parse<vec2>(ss); break;
			case hashof("WindowTitleAlign")			: ref.WindowTitleAlign = _ML_UTIL parse<vec2>(ss); break;
			case hashof("WindowMenuButtonPosition")	: ref.WindowMenuButtonPosition = _ML_UTIL parse<int32>(ss); break;
			case hashof("ChildRounding")			: ref.ChildRounding = _ML_UTIL parse<float32>(ss); break;
			case hashof("ChildBorderSize")			: ref.ChildBorderSize = _ML_UTIL parse<float32>(ss); break;
			case hashof("PopupRounding")			: ref.PopupRounding = _ML_UTIL parse<float32>(ss); break;
			case hashof("PopupBorderSize")			: ref.PopupBorderSize = _ML_UTIL parse<float32>(ss); break;
			case hashof("FramePadding")				: ref.FramePadding = _ML_UTIL parse<vec2>(ss); break;
			case hashof("FrameRounding")			: ref.FrameRounding = _ML_UTIL parse<float32>(ss); break;
			case hashof("FrameBorderSize")			: ref.FrameBorderSize = _ML_UTIL parse<float32>(ss); break;
			case hashof("ItemSpacing")				: ref.ItemSpacing = _ML_UTIL parse<vec2>(ss); break;
			case hashof("ItemInnerSpacing")			: ref.ItemInnerSpacing = _ML_UTIL parse<vec2>(ss); break;
			case hashof("TouchExtraPadding")		: ref.TouchExtraPadding = _ML_UTIL parse<vec2>(ss); break;
			case hashof("IndentSpacing")			: ref.IndentSpacing = _ML_UTIL parse<float32>(ss); break;
			case hashof("ColumnsMinSpacing")		: ref.ColumnsMinSpacing = _ML_UTIL parse<float32>(ss); break;
			case hashof("ScrollbarSize")			: ref.ScrollbarSize = _ML_UTIL parse<float32>(ss); break;
			case hashof("ScrollbarRounding")		: ref.ScrollbarRounding = _ML_UTIL parse<float32>(ss); break;
			case hashof("GrabMinSize")				: ref.GrabMinSize = _ML_UTIL parse<float32>(ss); break;
			case hashof("GrabRounding")				: ref.GrabRounding = _ML_UTIL parse<float32>(ss); break;
			case hashof("TabRounding")				: ref.TabRounding = _ML_UTIL parse<float32>(ss); break;
			case hashof("TabBorderSize")			: ref.TabBorderSize = _ML_UTIL parse<float32>(ss); break;
			case hashof("TabMinWidthForCloseButton"): ref.TabMinWidthForCloseButton = _ML_UTIL parse<float32>(ss); break;
			case hashof("ColorButtonPosition")		: ref.ColorButtonPosition = _ML_UTIL parse<int32>(ss); break;
			case hashof("ButtonTextAlign")			: ref.ButtonTextAlign = _ML_UTIL parse<vec2>(ss); break;
			case hashof("SelectableTextAlign")		: ref.SelectableTextAlign = _ML_UTIL parse<vec2>(ss); break;
			case hashof("DisplayWindowPadding")		: ref.DisplayWindowPadding = _ML_UTIL parse<vec2>(ss); break;
			case hashof("DisplaySafeAreaPadding")	: ref.DisplaySafeAreaPadding = _ML_UTIL parse<vec2>(ss); break;
			case hashof("MouseCursorScale")			: ref.MouseCursorScale = _ML_UTIL parse<float32>(ss); break;
			case hashof("AntiAliasedLines")			: ref.AntiAliasedLines = _ML_UTIL parse<bool>(ss); break;
			case hashof("AntiAliasedFill")			: ref.AntiAliasedFill = _ML_UTIL parse<bool>(ss); break;
			case hashof("CurveTessellationTol")		: ref.CurveTessellationTol = _ML_UTIL parse<float32>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			case hashof("ImGuiCol"): { switch (hashof(_ML_UTIL parse<ds::string>(ss)))
			{
			case hashof("Text")					: ref.Colors[ImGuiCol_Text] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("TextDisabled")			: ref.Colors[ImGuiCol_TextDisabled] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("WindowBg")				: ref.Colors[ImGuiCol_WindowBg] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ChildBg")				: ref.Colors[ImGuiCol_ChildBg] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("PopupBg")				: ref.Colors[ImGuiCol_PopupBg] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("Border")				: ref.Colors[ImGuiCol_Border] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("BorderShadow")			: ref.Colors[ImGuiCol_BorderShadow] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("FrameBg")				: ref.Colors[ImGuiCol_FrameBg] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("FrameBgHovered")		: ref.Colors[ImGuiCol_FrameBgHovered] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("FrameBgActive")		: ref.Colors[ImGuiCol_FrameBgActive] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("TitleBg")				: ref.Colors[ImGuiCol_TitleBg] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("TitleBgActive")		: ref.Colors[ImGuiCol_TitleBgActive] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("TitleBgCollapsed")		: ref.Colors[ImGuiCol_TitleBgCollapsed] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("MenuBarBg")			: ref.Colors[ImGuiCol_MenuBarBg] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ScrollbarBg")			: ref.Colors[ImGuiCol_ScrollbarBg] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ScrollbarGrab")		: ref.Colors[ImGuiCol_ScrollbarGrab] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ScrollbarGrabHovered")	: ref.Colors[ImGuiCol_ScrollbarGrabHovered] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ScrollbarGrabActive")	: ref.Colors[ImGuiCol_ScrollbarGrabActive] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("CheckMark")			: ref.Colors[ImGuiCol_CheckMark] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("SliderGrab")			: ref.Colors[ImGuiCol_SliderGrab] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("SliderGrabActive")		: ref.Colors[ImGuiCol_SliderGrabActive] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("Button")				: ref.Colors[ImGuiCol_Button] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ButtonHovered")		: ref.Colors[ImGuiCol_ButtonHovered] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ButtonActive")			: ref.Colors[ImGuiCol_ButtonActive] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("Header")				: ref.Colors[ImGuiCol_Header] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("HeaderHovered")		: ref.Colors[ImGuiCol_HeaderHovered] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("HeaderActive")			: ref.Colors[ImGuiCol_HeaderActive] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("Separator")			: ref.Colors[ImGuiCol_Separator] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("SeparatorHovered")		: ref.Colors[ImGuiCol_SeparatorHovered] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("SeparatorActive")		: ref.Colors[ImGuiCol_SeparatorActive] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ResizeGrip")			: ref.Colors[ImGuiCol_ResizeGrip] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ResizeGripHovered")	: ref.Colors[ImGuiCol_ResizeGripHovered] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ResizeGripActive")		: ref.Colors[ImGuiCol_ResizeGripActive] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("Tab")					: ref.Colors[ImGuiCol_Tab] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("TabHovered")			: ref.Colors[ImGuiCol_TabHovered] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("TabActive")			: ref.Colors[ImGuiCol_TabActive] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("TabUnfocused")			: ref.Colors[ImGuiCol_TabUnfocused] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("TabUnfocusedActive")	: ref.Colors[ImGuiCol_TabUnfocusedActive] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("DockingPreview")		: ref.Colors[ImGuiCol_DockingPreview] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("DockingEmptyBg")		: ref.Colors[ImGuiCol_DockingEmptyBg] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("PlotLines")			: ref.Colors[ImGuiCol_PlotLines] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("PlotLinesHovered")		: ref.Colors[ImGuiCol_PlotLinesHovered] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("PlotHistogram")		: ref.Colors[ImGuiCol_PlotHistogram] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("PlotHistogramHovered")	: ref.Colors[ImGuiCol_PlotHistogramHovered] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("TextSelectedBg")		: ref.Colors[ImGuiCol_TextSelectedBg] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("DragDropTarget")		: ref.Colors[ImGuiCol_DragDropTarget] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("NavHighlight")			: ref.Colors[ImGuiCol_NavHighlight] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("NavWindowingHighlight"): ref.Colors[ImGuiCol_NavWindowingHighlight] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("NavWindowingDimBg")	: ref.Colors[ImGuiCol_NavWindowingDimBg] = _ML_UTIL parse<vec4>(ss); break;
			case hashof("ModalWindowDimBg")		: ref.Colors[ImGuiCol_ModalWindowDimBg] = _ML_UTIL parse<vec4>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			}
		}
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}