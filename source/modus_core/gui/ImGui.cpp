#include <modus_core/gui/ImGui.hpp>
#include <modus_core/detail/FileUtility.hpp>
#include <modus_core/detail/ParserUtility.hpp>

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
		switch (hashof(util::to_lower(path.string())))
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
		string line;
		while (std::getline(f, line))
		{
			if ((line = util::trim(line)).empty() || line.front() == '#') { continue; }
	
			// scan line
			std::stringstream ss{}; ss << line;
			switch (hashof(util::parse<string>(ss)))
			{
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			case hashof("ImGuiStyle"): { switch (hashof(util::parse<string>(ss)))
			{
			case hashof("Alpha")					: ref.Alpha = util::parse<float32>(ss); break;
			case hashof("WindowPadding")			: ref.WindowPadding = util::parse<vec2>(ss); break;
			case hashof("WindowRounding")			: ref.WindowRounding = util::parse<float32>(ss); break;
			case hashof("WindowBorderSize")			: ref.WindowBorderSize = util::parse<float32>(ss); break;
			case hashof("WindowMinSize")			: ref.WindowMinSize = util::parse<vec2>(ss); break;
			case hashof("WindowTitleAlign")			: ref.WindowTitleAlign = util::parse<vec2>(ss); break;
			case hashof("WindowMenuButtonPosition")	: ref.WindowMenuButtonPosition = util::parse<int32>(ss); break;
			case hashof("ChildRounding")			: ref.ChildRounding = util::parse<float32>(ss); break;
			case hashof("ChildBorderSize")			: ref.ChildBorderSize = util::parse<float32>(ss); break;
			case hashof("PopupRounding")			: ref.PopupRounding = util::parse<float32>(ss); break;
			case hashof("PopupBorderSize")			: ref.PopupBorderSize = util::parse<float32>(ss); break;
			case hashof("FramePadding")				: ref.FramePadding = util::parse<vec2>(ss); break;
			case hashof("FrameRounding")			: ref.FrameRounding = util::parse<float32>(ss); break;
			case hashof("FrameBorderSize")			: ref.FrameBorderSize = util::parse<float32>(ss); break;
			case hashof("ItemSpacing")				: ref.ItemSpacing = util::parse<vec2>(ss); break;
			case hashof("ItemInnerSpacing")			: ref.ItemInnerSpacing = util::parse<vec2>(ss); break;
			case hashof("TouchExtraPadding")		: ref.TouchExtraPadding = util::parse<vec2>(ss); break;
			case hashof("IndentSpacing")			: ref.IndentSpacing = util::parse<float32>(ss); break;
			case hashof("ColumnsMinSpacing")		: ref.ColumnsMinSpacing = util::parse<float32>(ss); break;
			case hashof("ScrollbarSize")			: ref.ScrollbarSize = util::parse<float32>(ss); break;
			case hashof("ScrollbarRounding")		: ref.ScrollbarRounding = util::parse<float32>(ss); break;
			case hashof("GrabMinSize")				: ref.GrabMinSize = util::parse<float32>(ss); break;
			case hashof("GrabRounding")				: ref.GrabRounding = util::parse<float32>(ss); break;
			case hashof("TabRounding")				: ref.TabRounding = util::parse<float32>(ss); break;
			case hashof("TabBorderSize")			: ref.TabBorderSize = util::parse<float32>(ss); break;
			case hashof("TabMinWidthForCloseButton"): ref.TabMinWidthForCloseButton = util::parse<float32>(ss); break;
			case hashof("ColorButtonPosition")		: ref.ColorButtonPosition = util::parse<int32>(ss); break;
			case hashof("ButtonTextAlign")			: ref.ButtonTextAlign = util::parse<vec2>(ss); break;
			case hashof("SelectableTextAlign")		: ref.SelectableTextAlign = util::parse<vec2>(ss); break;
			case hashof("DisplayWindowPadding")		: ref.DisplayWindowPadding = util::parse<vec2>(ss); break;
			case hashof("DisplaySafeAreaPadding")	: ref.DisplaySafeAreaPadding = util::parse<vec2>(ss); break;
			case hashof("MouseCursorScale")			: ref.MouseCursorScale = util::parse<float32>(ss); break;
			case hashof("AntiAliasedLines")			: ref.AntiAliasedLines = util::parse<bool>(ss); break;
			case hashof("AntiAliasedFill")			: ref.AntiAliasedFill = util::parse<bool>(ss); break;
			case hashof("CurveTessellationTol")		: ref.CurveTessellationTol = util::parse<float32>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			case hashof("ImGuiCol"): { switch (hashof(util::parse<string>(ss)))
			{
			case hashof("Text")					: ref.Colors[ImGuiCol_Text] = util::parse<vec4>(ss); break;
			case hashof("TextDisabled")			: ref.Colors[ImGuiCol_TextDisabled] = util::parse<vec4>(ss); break;
			case hashof("WindowBg")				: ref.Colors[ImGuiCol_WindowBg] = util::parse<vec4>(ss); break;
			case hashof("ChildBg")				: ref.Colors[ImGuiCol_ChildBg] = util::parse<vec4>(ss); break;
			case hashof("PopupBg")				: ref.Colors[ImGuiCol_PopupBg] = util::parse<vec4>(ss); break;
			case hashof("Border")				: ref.Colors[ImGuiCol_Border] = util::parse<vec4>(ss); break;
			case hashof("BorderShadow")			: ref.Colors[ImGuiCol_BorderShadow] = util::parse<vec4>(ss); break;
			case hashof("FrameBg")				: ref.Colors[ImGuiCol_FrameBg] = util::parse<vec4>(ss); break;
			case hashof("FrameBgHovered")		: ref.Colors[ImGuiCol_FrameBgHovered] = util::parse<vec4>(ss); break;
			case hashof("FrameBgActive")		: ref.Colors[ImGuiCol_FrameBgActive] = util::parse<vec4>(ss); break;
			case hashof("TitleBg")				: ref.Colors[ImGuiCol_TitleBg] = util::parse<vec4>(ss); break;
			case hashof("TitleBgActive")		: ref.Colors[ImGuiCol_TitleBgActive] = util::parse<vec4>(ss); break;
			case hashof("TitleBgCollapsed")		: ref.Colors[ImGuiCol_TitleBgCollapsed] = util::parse<vec4>(ss); break;
			case hashof("MenuBarBg")			: ref.Colors[ImGuiCol_MenuBarBg] = util::parse<vec4>(ss); break;
			case hashof("ScrollbarBg")			: ref.Colors[ImGuiCol_ScrollbarBg] = util::parse<vec4>(ss); break;
			case hashof("ScrollbarGrab")		: ref.Colors[ImGuiCol_ScrollbarGrab] = util::parse<vec4>(ss); break;
			case hashof("ScrollbarGrabHovered")	: ref.Colors[ImGuiCol_ScrollbarGrabHovered] = util::parse<vec4>(ss); break;
			case hashof("ScrollbarGrabActive")	: ref.Colors[ImGuiCol_ScrollbarGrabActive] = util::parse<vec4>(ss); break;
			case hashof("CheckMark")			: ref.Colors[ImGuiCol_CheckMark] = util::parse<vec4>(ss); break;
			case hashof("SliderGrab")			: ref.Colors[ImGuiCol_SliderGrab] = util::parse<vec4>(ss); break;
			case hashof("SliderGrabActive")		: ref.Colors[ImGuiCol_SliderGrabActive] = util::parse<vec4>(ss); break;
			case hashof("Button")				: ref.Colors[ImGuiCol_Button] = util::parse<vec4>(ss); break;
			case hashof("ButtonHovered")		: ref.Colors[ImGuiCol_ButtonHovered] = util::parse<vec4>(ss); break;
			case hashof("ButtonActive")			: ref.Colors[ImGuiCol_ButtonActive] = util::parse<vec4>(ss); break;
			case hashof("Header")				: ref.Colors[ImGuiCol_Header] = util::parse<vec4>(ss); break;
			case hashof("HeaderHovered")		: ref.Colors[ImGuiCol_HeaderHovered] = util::parse<vec4>(ss); break;
			case hashof("HeaderActive")			: ref.Colors[ImGuiCol_HeaderActive] = util::parse<vec4>(ss); break;
			case hashof("Separator")			: ref.Colors[ImGuiCol_Separator] = util::parse<vec4>(ss); break;
			case hashof("SeparatorHovered")		: ref.Colors[ImGuiCol_SeparatorHovered] = util::parse<vec4>(ss); break;
			case hashof("SeparatorActive")		: ref.Colors[ImGuiCol_SeparatorActive] = util::parse<vec4>(ss); break;
			case hashof("ResizeGrip")			: ref.Colors[ImGuiCol_ResizeGrip] = util::parse<vec4>(ss); break;
			case hashof("ResizeGripHovered")	: ref.Colors[ImGuiCol_ResizeGripHovered] = util::parse<vec4>(ss); break;
			case hashof("ResizeGripActive")		: ref.Colors[ImGuiCol_ResizeGripActive] = util::parse<vec4>(ss); break;
			case hashof("Tab")					: ref.Colors[ImGuiCol_Tab] = util::parse<vec4>(ss); break;
			case hashof("TabHovered")			: ref.Colors[ImGuiCol_TabHovered] = util::parse<vec4>(ss); break;
			case hashof("TabActive")			: ref.Colors[ImGuiCol_TabActive] = util::parse<vec4>(ss); break;
			case hashof("TabUnfocused")			: ref.Colors[ImGuiCol_TabUnfocused] = util::parse<vec4>(ss); break;
			case hashof("TabUnfocusedActive")	: ref.Colors[ImGuiCol_TabUnfocusedActive] = util::parse<vec4>(ss); break;
			case hashof("DockingPreview")		: ref.Colors[ImGuiCol_DockingPreview] = util::parse<vec4>(ss); break;
			case hashof("DockingEmptyBg")		: ref.Colors[ImGuiCol_DockingEmptyBg] = util::parse<vec4>(ss); break;
			case hashof("PlotLines")			: ref.Colors[ImGuiCol_PlotLines] = util::parse<vec4>(ss); break;
			case hashof("PlotLinesHovered")		: ref.Colors[ImGuiCol_PlotLinesHovered] = util::parse<vec4>(ss); break;
			case hashof("PlotHistogram")		: ref.Colors[ImGuiCol_PlotHistogram] = util::parse<vec4>(ss); break;
			case hashof("PlotHistogramHovered")	: ref.Colors[ImGuiCol_PlotHistogramHovered] = util::parse<vec4>(ss); break;
			case hashof("TextSelectedBg")		: ref.Colors[ImGuiCol_TextSelectedBg] = util::parse<vec4>(ss); break;
			case hashof("DragDropTarget")		: ref.Colors[ImGuiCol_DragDropTarget] = util::parse<vec4>(ss); break;
			case hashof("NavHighlight")			: ref.Colors[ImGuiCol_NavHighlight] = util::parse<vec4>(ss); break;
			case hashof("NavWindowingHighlight"): ref.Colors[ImGuiCol_NavWindowingHighlight] = util::parse<vec4>(ss); break;
			case hashof("NavWindowingDimBg")	: ref.Colors[ImGuiCol_NavWindowingDimBg] = util::parse<vec4>(ss); break;
			case hashof("ModalWindowDimBg")		: ref.Colors[ImGuiCol_ModalWindowDimBg] = util::parse<vec4>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			}
		}
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}