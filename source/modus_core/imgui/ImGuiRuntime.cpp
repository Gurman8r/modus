#include <modus_core/imgui/ImGuiRuntime.hpp>
#include <modus_core/system/Events.hpp>
#include <modus_core/detail/FileUtility.hpp>
#include <modus_core/detail/ParserUtil.hpp>
#include <modus_core/graphics/RenderCommand.hpp>
#include <modus_core/graphics/RenderWindow.hpp>

// GLFW / OpenGL3
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#endif

namespace ml
{
	imgui_runtime::imgui_runtime(memory_manager * mem, render_window * win)
		: m_ctx	{}
		, m_win	{ win }
	{
		// check version
		ML_assert("failed validating imgui version" && IMGUI_CHECKVERSION());

		// set allocators
		ImGui::SetAllocatorFunctions(
			[](size_t s, void * x) { return ((memory_manager *)x)->allocate(s); },
			[](void * p, void * x) { return ((memory_manager *)x)->deallocate(p); },
			mem);

		// create context
		m_ctx.reset(ImGui::CreateContext());
		ML_assert(m_ctx && "failed creating imgui context");

		// configure context
		auto & io{ ImGui::GetIO() };
		io.LogFilename = nullptr;
		io.IniFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}
	
	imgui_runtime::~imgui_runtime()
	{
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
#else
#endif

		ImGui::DestroyContext(m_ctx.release());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool imgui_runtime::configure(json const & j)
	{
		if (!std::invoke([&, install = (j.contains("callbacks") && j["callbacks"])]()
		{
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
			return ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)m_win->get_handle(), install)
				&& ImGui_ImplOpenGL3_Init("#version 130");
#else
#endif
		}))
		{
			return debug::error("failed initializing imgui");
		}
		else
		{
			if (j.contains("style_path")) { load_style(j["style_path"]); }

			return true;
		}
	}

	void imgui_runtime::new_frame()
	{
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
#else
#endif

		ImGui::NewFrame();
	}

	void imgui_runtime::render_frame()
	{
		ImGui::Render();

		m_win->execute(
			gfx::command::set_viewport(m_win->get_framebuffer_size()),
			gfx::command::set_clear_color(colors::black),
			gfx::command::clear(gfx::clear_color));

#if defined(ML_IMPL_RENDERER_OPENGL)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
#endif

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			auto backup_context{ native_window::get_context_current() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			native_window::make_context_current(backup_context);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool imgui_runtime::load_style(fs::path const & path)
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
		std::ifstream f{ path }; ML_defer(&f) { f.close(); };
		if (!f) return false;
	
		// style ref
		auto & style{ ImGui::GetStyle() };
	
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
			case hashof("Alpha")					: style.Alpha = parse<float_t>(ss); break;
			case hashof("WindowPadding")			: style.WindowPadding = parse<vec2>(ss); break;
			case hashof("WindowRounding")			: style.WindowRounding = parse<float_t>(ss); break;
			case hashof("WindowBorderSize")			: style.WindowBorderSize = parse<float_t>(ss); break;
			case hashof("WindowMinSize")			: style.WindowMinSize = parse<vec2>(ss); break;
			case hashof("WindowTitleAlign")			: style.WindowTitleAlign = parse<vec2>(ss); break;
			case hashof("ChildRounding")			: style.ChildRounding = parse<float_t>(ss); break;
			case hashof("ChildBorderSize")			: style.ChildBorderSize = parse<float_t>(ss); break;
			case hashof("PopupRounding")			: style.PopupRounding = parse<float_t>(ss); break;
			case hashof("PopupBorderSize")			: style.PopupBorderSize = parse<float_t>(ss); break;
			case hashof("FramePadding")				: style.FramePadding = parse<vec2>(ss); break;
			case hashof("FrameRounding")			: style.FrameRounding = parse<float_t>(ss); break;
			case hashof("FrameBorderSize")			: style.FrameBorderSize = parse<float_t>(ss); break;
			case hashof("ItemSpacing")				: style.ItemInnerSpacing = parse<vec2>(ss); break;
			case hashof("ItemInnerSpacing")			: style.ItemInnerSpacing = parse<vec2>(ss); break;
			case hashof("TouchExtraPadding")		: style.TouchExtraPadding = parse<vec2>(ss); break;
			case hashof("IndentSpacing")			: style.IndentSpacing = parse<float_t>(ss); break;
			case hashof("ColumnsMinSpacing")		: style.ColumnsMinSpacing = parse<float_t>(ss); break;
			case hashof("ScrollbarSize")			: style.ScrollbarSize = parse<float_t>(ss); break;
			case hashof("ScrollbarRounding")		: style.ScrollbarRounding = parse<float_t>(ss); break;
			case hashof("GrabMinSize")				: style.GrabMinSize = parse<float_t>(ss); break;
			case hashof("GrabRounding")				: style.GrabRounding = parse<float_t>(ss); break;
			case hashof("TabRounding")				: style.TabRounding = parse<float_t>(ss); break;
			case hashof("TabBorderSize")			: style.TabBorderSize = parse<float_t>(ss); break;
			case hashof("ButtonTextAlign")			: style.ButtonTextAlign = parse<vec2>(ss); break;
			case hashof("SelectableTextAlign")		: style.SelectableTextAlign = parse<vec2>(ss); break;
			case hashof("DisplayWindowPadding")		: style.DisplayWindowPadding = parse<vec2>(ss); break;
			case hashof("DisplaySafeAreaPadding")	: style.DisplaySafeAreaPadding = parse<vec2>(ss); break;
			case hashof("MouseCursorScale")			: style.MouseCursorScale = parse<float_t>(ss); break;
			case hashof("AntiAliasedLines")			: style.AntiAliasedLines = parse<bool>(ss); break;
			case hashof("AntiAliasedFill")			: style.AntiAliasedFill = parse<bool>(ss); break;
			case hashof("CurveTessellationTol")		: style.CurveTessellationTol = parse<float_t>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			case hashof("ImGuiCol"): { switch (hashof(parse<pmr::string>(ss)))
			{
			case hashof("Text")					: style.Colors[ImGuiCol_Text] = parse<vec4>(ss); break;
			case hashof("TextDisabled")			: style.Colors[ImGuiCol_TextDisabled] = parse<vec4>(ss); break;
			case hashof("WindowBg")				: style.Colors[ImGuiCol_WindowBg] = parse<vec4>(ss); break;
			case hashof("ChildBg")				: style.Colors[ImGuiCol_ChildBg] = parse<vec4>(ss); break;
			case hashof("PopupBg")				: style.Colors[ImGuiCol_PopupBg] = parse<vec4>(ss); break;
			case hashof("Border")				: style.Colors[ImGuiCol_Border] = parse<vec4>(ss); break;
			case hashof("BorderShadow")			: style.Colors[ImGuiCol_BorderShadow] = parse<vec4>(ss); break;
			case hashof("FrameBg")				: style.Colors[ImGuiCol_FrameBg] = parse<vec4>(ss); break;
			case hashof("FrameBgHovered")		: style.Colors[ImGuiCol_FrameBgHovered] = parse<vec4>(ss); break;
			case hashof("FrameBgActive")		: style.Colors[ImGuiCol_FrameBgActive] = parse<vec4>(ss); break;
			case hashof("TitleBg")				: style.Colors[ImGuiCol_TitleBg] = parse<vec4>(ss); break;
			case hashof("TitleBgActive")		: style.Colors[ImGuiCol_TitleBgActive] = parse<vec4>(ss); break;
			case hashof("TitleBgCollapsed")		: style.Colors[ImGuiCol_TitleBgCollapsed] = parse<vec4>(ss); break;
			case hashof("MenuBarBg")			: style.Colors[ImGuiCol_MenuBarBg] = parse<vec4>(ss); break;
			case hashof("ScrollbarBg")			: style.Colors[ImGuiCol_ScrollbarBg] = parse<vec4>(ss); break;
			case hashof("ScrollbarGrab")		: style.Colors[ImGuiCol_ScrollbarGrab] = parse<vec4>(ss); break;
			case hashof("ScrollbarGrabHovered")	: style.Colors[ImGuiCol_ScrollbarGrabHovered] = parse<vec4>(ss); break;
			case hashof("ScrollbarGrabActive")	: style.Colors[ImGuiCol_ScrollbarGrabActive] = parse<vec4>(ss); break;
			case hashof("CheckMark")			: style.Colors[ImGuiCol_CheckMark] = parse<vec4>(ss); break;
			case hashof("SliderGrab")			: style.Colors[ImGuiCol_SliderGrab] = parse<vec4>(ss); break;
			case hashof("SliderGrabActive")		: style.Colors[ImGuiCol_SliderGrabActive] = parse<vec4>(ss); break;
			case hashof("Button")				: style.Colors[ImGuiCol_Button] = parse<vec4>(ss); break;
			case hashof("ButtonHovered")		: style.Colors[ImGuiCol_ButtonHovered] = parse<vec4>(ss); break;
			case hashof("ButtonActive")			: style.Colors[ImGuiCol_ButtonActive] = parse<vec4>(ss); break;
			case hashof("Header")				: style.Colors[ImGuiCol_Header] = parse<vec4>(ss); break;
			case hashof("HeaderHovered")		: style.Colors[ImGuiCol_HeaderHovered] = parse<vec4>(ss); break;
			case hashof("HeaderActive")			: style.Colors[ImGuiCol_HeaderActive] = parse<vec4>(ss); break;
			case hashof("Separator")			: style.Colors[ImGuiCol_Separator] = parse<vec4>(ss); break;
			case hashof("SeparatorHovered")		: style.Colors[ImGuiCol_SeparatorHovered] = parse<vec4>(ss); break;
			case hashof("SeparatorActive")		: style.Colors[ImGuiCol_SeparatorActive] = parse<vec4>(ss); break;
			case hashof("ResizeGrip")			: style.Colors[ImGuiCol_ResizeGrip] = parse<vec4>(ss); break;
			case hashof("ResizeGripHovered")	: style.Colors[ImGuiCol_ResizeGripHovered] = parse<vec4>(ss); break;
			case hashof("ResizeGripActive")		: style.Colors[ImGuiCol_ResizeGripActive] = parse<vec4>(ss); break;
			case hashof("Tab")					: style.Colors[ImGuiCol_Tab] = parse<vec4>(ss); break;
			case hashof("TabHovered")			: style.Colors[ImGuiCol_TabHovered] = parse<vec4>(ss); break;
			case hashof("TabActive")			: style.Colors[ImGuiCol_TabActive] = parse<vec4>(ss); break;
			case hashof("TabUnfocused")			: style.Colors[ImGuiCol_TabUnfocused] = parse<vec4>(ss); break;
			case hashof("TabUnfocusedActive")	: style.Colors[ImGuiCol_TabUnfocusedActive] = parse<vec4>(ss); break;
			case hashof("DockingPreview")		: style.Colors[ImGuiCol_DockingPreview] = parse<vec4>(ss); break;
			case hashof("DockingEmptyBg")		: style.Colors[ImGuiCol_DockingEmptyBg] = parse<vec4>(ss); break;
			case hashof("PlotLines")			: style.Colors[ImGuiCol_PlotLines] = parse<vec4>(ss); break;
			case hashof("PlotLinesHovered")		: style.Colors[ImGuiCol_PlotLinesHovered] = parse<vec4>(ss); break;
			case hashof("PlotHistogram")		: style.Colors[ImGuiCol_PlotHistogram] = parse<vec4>(ss); break;
			case hashof("PlotHistogramHovered")	: style.Colors[ImGuiCol_PlotHistogramHovered] = parse<vec4>(ss); break;
			case hashof("TextSelectedBg")		: style.Colors[ImGuiCol_TextSelectedBg] = parse<vec4>(ss); break;
			case hashof("DragDropTarget")		: style.Colors[ImGuiCol_DragDropTarget] = parse<vec4>(ss); break;
			case hashof("NavHighlight")			: style.Colors[ImGuiCol_NavHighlight] = parse<vec4>(ss); break;
			case hashof("NavWindowingHighlight"): style.Colors[ImGuiCol_NavWindowingHighlight] = parse<vec4>(ss); break;
			case hashof("NavWindowingDimBg")	: style.Colors[ImGuiCol_NavWindowingDimBg] = parse<vec4>(ss); break;
			case hashof("ModalWindowDimBg")		: style.Colors[ImGuiCol_ModalWindowDimBg] = parse<vec4>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			}
		}
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}