#include <core/imgui/ImGuiContext.hpp>
#include <core/imgui/ImGuiEvents.hpp>
#include <core/system/Events.hpp>
#include <core/detail/FileUtility.hpp>
#include <core/detail/ParserUtil.hpp>
#include <core/graphics/RenderCommand.hpp>
#include <core/graphics/RenderWindow.hpp>

// GLFW / OpenGL3
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#endif

namespace ml
{
	imgui_context::imgui_context(event_bus * bus, render_window * win, allocator_type alloc)
		: m_win			{ win }
		, m_bus			{ bus }
		, m_ctx			{}
		, m_menubar		{ alloc }
		, m_dockspace	{ alloc }
	{
		ML_assert(m_bus && m_win);

		// check version
		ML_assert("failed validating imgui version" && IMGUI_CHECKVERSION());

		// set allocators
		ImGui::SetAllocatorFunctions
		(
			[](size_t s, auto) noexcept { return memory::allocate(s); },
			[](void * p, auto) noexcept { return memory::deallocate(p); },
			nullptr
		);

		// create context
		ML_assert("failed creating imgui context" && (m_ctx = ImGui::CreateContext()));

		// configure context
		auto & io{ ImGui::GetIO() };
		io.LogFilename = nullptr;
		io.IniFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}
	
	imgui_context::~imgui_context()
	{
		shutdown();

		ImGui::DestroyContext(m_ctx);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool imgui_context::startup(bool install_callbacks)
	{
		return
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
			(ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)m_win->get_handle(), install_callbacks)
				|| debug::error("failed initializing imgui platform")) &&
			
			(ImGui_ImplOpenGL3_Init("#version 130")
				|| debug::error("failed initializing imgui renderer"))
#endif
			;
	}

	void imgui_context::shutdown()
	{
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
#endif
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void imgui_context::new_frame()
	{
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
#endif
		ImGui::NewFrame();

		ML_ImGui_ScopeID(this);

		// DOCKSPACE
		if (m_dockspace.enabled && (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable))
		{
			// viewport
			ImGuiViewport const * v{ ImGui::GetMainViewport() };
			ImGui::SetNextWindowPos(v->Pos);
			ImGui::SetNextWindowSize(v->Size);
			ImGui::SetNextWindowViewport(v->ID);

			// style
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, m_dockspace.rounding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, m_dockspace.border);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_dockspace.padding);
			ImGui::SetNextWindowBgAlpha(m_dockspace.alpha);

			// begin
			if (ImGui::Begin(m_dockspace.title.c_str(), &m_dockspace.enabled,
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoNavFocus |
				ImGuiWindowFlags_NoDocking |
				ImGuiWindowFlags_NoBackground |
				(m_menubar.enabled ? ImGuiWindowFlags_MenuBar : 0)
			))
			{
				ImGui::PopStyleVar(3);

				// fire docking event if nodes are empty
				if (m_dockspace.nodes.empty())
				{
					m_bus->fire<imgui_dockspace_event>(&m_dockspace);
				}

				ImGui::DockSpace(
					ImGui::GetID(m_dockspace.title.c_str()),
					m_dockspace.size,
					ImGuiDockNodeFlags_PassthruCentralNode |
					m_dockspace.flags);

				ImGui::End();
			}
		}

		// MENUBAR
		if (m_menubar.enabled && ImGui::BeginMainMenuBar())
		{
			m_bus->fire<imgui_menubar_event>(&m_menubar);

			ImGui::EndMainMenuBar();
		}

		// RENDER
		m_bus->fire<imgui_render_event>(this);
	}

	void imgui_context::render_frame()
	{
		ImGui::Render();

		for (auto const & cmd : {
			gfx::command::set_viewport(m_win->get_framebuffer_size()),
			gfx::command::set_clear_color(colors::black),
			gfx::command::clear(gfx::clear_color),
		})	gfx::execute(cmd, m_win->get_render_context());

#if defined(ML_IMPL_RENDERER_OPENGL)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			auto backup_context{ window::get_context_current() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			window::make_context_current(backup_context);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void imgui_context::imgui_demo(bool * p_open)
	{
		ImGui::ShowDemoWindow(p_open);
	}

	void imgui_context::imgui_metrics(bool * p_open)
	{
		ImGui::ShowMetricsWindow(p_open);
	}

	void imgui_context::imgui_about(bool * p_open)
	{
		ImGui::ShowAboutWindow(p_open);
	}

	void imgui_context::imgui_style_editor(ImGuiStyle * ref)
	{
		ImGui::ShowStyleEditor(ref);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	uint32_t imgui_context::dockspace::begin_builder()
	{
		if (uint32_t root{ ImGui::GetID(title.c_str()) }
		; ImGui::DockBuilderGetNode(root)) { return NULL; }
		else
		{
			ImGui::DockBuilderRemoveNode(root);
			ImGui::DockBuilderAddNode(root, flags);
			return root;
		}
	}

	uint32_t imgui_context::dockspace::end_builder(uint32_t root)
	{
		if (root) { ImGui::DockBuilderFinish(root); }
		return root;
	}

	uint32_t imgui_context::dockspace::dock(cstring name, uint32_t id)
	{
		if (!name || !id) { return NULL; }
		else
		{
			ImGui::DockBuilderDockWindow(name, id);
			return id;
		}
	}

	uint32_t imgui_context::dockspace::dock(gui_form const & w, uint32_t id)
	{
		return this->dock(w.title, id);
	}

	uint32_t imgui_context::dockspace::split(uint32_t i, uint32_t id, int32_t dir, float_t ratio, uint32_t * value)
	{
		return nodes[(size_t)i] = split(id, dir, ratio, value);
	}

	uint32_t imgui_context::dockspace::split(uint32_t id, int32_t dir, float_t ratio, uint32_t * value)
	{
		return this->split(id, dir, ratio, nullptr, value);
	}

	uint32_t imgui_context::dockspace::split(uint32_t id, int32_t dir, float_t ratio, uint32_t * out, uint32_t * value)
	{
		return ImGui::DockBuilderSplitNode(id, dir, ratio, out, value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool imgui_context::load_style(fs::path const & path)
	{
		using namespace util;
	
		// empty
		if (path.empty()) return false;

		// builtin
		switch (hash(to_lower(path.string())))
		{
		case hash("classic"): ImGui::StyleColorsClassic()	; return true;
		case hash("dark")	: ImGui::StyleColorsDark()		; return true;
		case hash("light")	: ImGui::StyleColorsLight()		; return true;
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
			switch (hash(parse<pmr::string>(ss)))
			{
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			case hash("ImGuiStyle"): { switch (hash(parse<pmr::string>(ss)))
			{
			case hash("Alpha")					: style.Alpha = parse<float_t>(ss); break;
			case hash("WindowPadding")			: style.WindowPadding = parse<vec2>(ss); break;
			case hash("WindowRounding")			: style.WindowRounding = parse<float_t>(ss); break;
			case hash("WindowBorderSize")		: style.WindowBorderSize = parse<float_t>(ss); break;
			case hash("WindowMinSize")			: style.WindowMinSize = parse<vec2>(ss); break;
			case hash("WindowTitleAlign")		: style.WindowTitleAlign = parse<vec2>(ss); break;
			case hash("ChildRounding")			: style.ChildRounding = parse<float_t>(ss); break;
			case hash("ChildBorderSize")		: style.ChildBorderSize = parse<float_t>(ss); break;
			case hash("PopupRounding")			: style.PopupRounding = parse<float_t>(ss); break;
			case hash("PopupBorderSize")		: style.PopupBorderSize = parse<float_t>(ss); break;
			case hash("FramePadding")			: style.FramePadding = parse<vec2>(ss); break;
			case hash("FrameRounding")			: style.FrameRounding = parse<float_t>(ss); break;
			case hash("FrameBorderSize")		: style.FrameBorderSize = parse<float_t>(ss); break;
			case hash("ItemSpacing")			: style.ItemInnerSpacing = parse<vec2>(ss); break;
			case hash("ItemInnerSpacing")		: style.ItemInnerSpacing = parse<vec2>(ss); break;
			case hash("TouchExtraPadding")		: style.TouchExtraPadding = parse<vec2>(ss); break;
			case hash("IndentSpacing")			: style.IndentSpacing = parse<float_t>(ss); break;
			case hash("ColumnsMinSpacing")		: style.ColumnsMinSpacing = parse<float_t>(ss); break;
			case hash("ScrollbarSize")			: style.ScrollbarSize = parse<float_t>(ss); break;
			case hash("ScrollbarRounding")		: style.ScrollbarRounding = parse<float_t>(ss); break;
			case hash("GrabMinSize")			: style.GrabMinSize = parse<float_t>(ss); break;
			case hash("GrabRounding")			: style.GrabRounding = parse<float_t>(ss); break;
			case hash("TabRounding")			: style.TabRounding = parse<float_t>(ss); break;
			case hash("TabBorderSize")			: style.TabBorderSize = parse<float_t>(ss); break;
			case hash("ButtonTextAlign")		: style.ButtonTextAlign = parse<vec2>(ss); break;
			case hash("SelectableTextAlign")	: style.SelectableTextAlign = parse<vec2>(ss); break;
			case hash("DisplayWindowPadding")	: style.DisplayWindowPadding = parse<vec2>(ss); break;
			case hash("DisplaySafeAreaPadding")	: style.DisplaySafeAreaPadding = parse<vec2>(ss); break;
			case hash("MouseCursorScale")		: style.MouseCursorScale = parse<float_t>(ss); break;
			case hash("AntiAliasedLines")		: style.AntiAliasedLines = parse<bool>(ss); break;
			case hash("AntiAliasedFill")		: style.AntiAliasedFill = parse<bool>(ss); break;
			case hash("CurveTessellationTol")	: style.CurveTessellationTol = parse<float_t>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			case hash("ImGuiCol"): { switch (hash(parse<pmr::string>(ss)))
			{
			case hash("Text")					: style.Colors[ImGuiCol_Text] = parse<vec4>(ss); break;
			case hash("TextDisabled")			: style.Colors[ImGuiCol_TextDisabled] = parse<vec4>(ss); break;
			case hash("WindowBg")				: style.Colors[ImGuiCol_WindowBg] = parse<vec4>(ss); break;
			case hash("ChildBg")				: style.Colors[ImGuiCol_ChildBg] = parse<vec4>(ss); break;
			case hash("PopupBg")				: style.Colors[ImGuiCol_PopupBg] = parse<vec4>(ss); break;
			case hash("Border")					: style.Colors[ImGuiCol_Border] = parse<vec4>(ss); break;
			case hash("BorderShadow")			: style.Colors[ImGuiCol_BorderShadow] = parse<vec4>(ss); break;
			case hash("FrameBg")				: style.Colors[ImGuiCol_FrameBg] = parse<vec4>(ss); break;
			case hash("FrameBgHovered")			: style.Colors[ImGuiCol_FrameBgHovered] = parse<vec4>(ss); break;
			case hash("FrameBgActive")			: style.Colors[ImGuiCol_FrameBgActive] = parse<vec4>(ss); break;
			case hash("TitleBg")				: style.Colors[ImGuiCol_TitleBg] = parse<vec4>(ss); break;
			case hash("TitleBgActive")			: style.Colors[ImGuiCol_TitleBgActive] = parse<vec4>(ss); break;
			case hash("TitleBgCollapsed")		: style.Colors[ImGuiCol_TitleBgCollapsed] = parse<vec4>(ss); break;
			case hash("MenuBarBg")				: style.Colors[ImGuiCol_MenuBarBg] = parse<vec4>(ss); break;
			case hash("ScrollbarBg")			: style.Colors[ImGuiCol_ScrollbarBg] = parse<vec4>(ss); break;
			case hash("ScrollbarGrab")			: style.Colors[ImGuiCol_ScrollbarGrab] = parse<vec4>(ss); break;
			case hash("ScrollbarGrabHovered")	: style.Colors[ImGuiCol_ScrollbarGrabHovered] = parse<vec4>(ss); break;
			case hash("ScrollbarGrabActive")	: style.Colors[ImGuiCol_ScrollbarGrabActive] = parse<vec4>(ss); break;
			case hash("CheckMark")				: style.Colors[ImGuiCol_CheckMark] = parse<vec4>(ss); break;
			case hash("SliderGrab")				: style.Colors[ImGuiCol_SliderGrab] = parse<vec4>(ss); break;
			case hash("SliderGrabActive")		: style.Colors[ImGuiCol_SliderGrabActive] = parse<vec4>(ss); break;
			case hash("Button")					: style.Colors[ImGuiCol_Button] = parse<vec4>(ss); break;
			case hash("ButtonHovered")			: style.Colors[ImGuiCol_ButtonHovered] = parse<vec4>(ss); break;
			case hash("ButtonActive")			: style.Colors[ImGuiCol_ButtonActive] = parse<vec4>(ss); break;
			case hash("Header")					: style.Colors[ImGuiCol_Header] = parse<vec4>(ss); break;
			case hash("HeaderHovered")			: style.Colors[ImGuiCol_HeaderHovered] = parse<vec4>(ss); break;
			case hash("HeaderActive")			: style.Colors[ImGuiCol_HeaderActive] = parse<vec4>(ss); break;
			case hash("Separator")				: style.Colors[ImGuiCol_Separator] = parse<vec4>(ss); break;
			case hash("SeparatorHovered")		: style.Colors[ImGuiCol_SeparatorHovered] = parse<vec4>(ss); break;
			case hash("SeparatorActive")		: style.Colors[ImGuiCol_SeparatorActive] = parse<vec4>(ss); break;
			case hash("ResizeGrip")				: style.Colors[ImGuiCol_ResizeGrip] = parse<vec4>(ss); break;
			case hash("ResizeGripHovered")		: style.Colors[ImGuiCol_ResizeGripHovered] = parse<vec4>(ss); break;
			case hash("ResizeGripActive")		: style.Colors[ImGuiCol_ResizeGripActive] = parse<vec4>(ss); break;
			case hash("Tab")					: style.Colors[ImGuiCol_Tab] = parse<vec4>(ss); break;
			case hash("TabHovered")				: style.Colors[ImGuiCol_TabHovered] = parse<vec4>(ss); break;
			case hash("TabActive")				: style.Colors[ImGuiCol_TabActive] = parse<vec4>(ss); break;
			case hash("TabUnfocused")			: style.Colors[ImGuiCol_TabUnfocused] = parse<vec4>(ss); break;
			case hash("TabUnfocusedActive")		: style.Colors[ImGuiCol_TabUnfocusedActive] = parse<vec4>(ss); break;
			case hash("DockingPreview")			: style.Colors[ImGuiCol_DockingPreview] = parse<vec4>(ss); break;
			case hash("DockingEmptyBg")			: style.Colors[ImGuiCol_DockingEmptyBg] = parse<vec4>(ss); break;
			case hash("PlotLines")				: style.Colors[ImGuiCol_PlotLines] = parse<vec4>(ss); break;
			case hash("PlotLinesHovered")		: style.Colors[ImGuiCol_PlotLinesHovered] = parse<vec4>(ss); break;
			case hash("PlotHistogram")			: style.Colors[ImGuiCol_PlotHistogram] = parse<vec4>(ss); break;
			case hash("PlotHistogramHovered")	: style.Colors[ImGuiCol_PlotHistogramHovered] = parse<vec4>(ss); break;
			case hash("TextSelectedBg")			: style.Colors[ImGuiCol_TextSelectedBg] = parse<vec4>(ss); break;
			case hash("DragDropTarget")			: style.Colors[ImGuiCol_DragDropTarget] = parse<vec4>(ss); break;
			case hash("NavHighlight")			: style.Colors[ImGuiCol_NavHighlight] = parse<vec4>(ss); break;
			case hash("NavWindowingHighlight")	: style.Colors[ImGuiCol_NavWindowingHighlight] = parse<vec4>(ss); break;
			case hash("NavWindowingDimBg")		: style.Colors[ImGuiCol_NavWindowingDimBg] = parse<vec4>(ss); break;
			case hash("ModalWindowDimBg")		: style.Colors[ImGuiCol_ModalWindowDimBg] = parse<vec4>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			}
		}
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}