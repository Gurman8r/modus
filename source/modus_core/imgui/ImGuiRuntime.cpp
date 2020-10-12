#include <modus_core/imgui/ImGuiRuntime.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>
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
	imgui_runtime::imgui_runtime(event_bus * bus, render_window * win, allocator_type alloc)
		: event_listener{ bus }
		, m_win			{ win }
		, m_ctx			{}
		, m_menubar		{ alloc }
		, m_dockspace	{ alloc }
	{
		ML_assert(get_bus() && m_win);

		// check version
		ML_assert("failed validating imgui version" && IMGUI_CHECKVERSION());

		// set allocators
		ImGui::SetAllocatorFunctions
		(
			[](size_t s, auto) noexcept { return memory_manager::allocate(s); },
			[](void * p, auto) noexcept { return memory_manager::deallocate(p); },
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
	
	imgui_runtime::~imgui_runtime()
	{
		shutdown();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void imgui_runtime::on_event(event && value)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool imgui_runtime::startup(json const & j)
	{
		if (m_running) { return false; }
		else { m_running = true; }

		if (!([&, inst_clbk = (j.contains("inst_clbk") && j["inst_clbk"])]() noexcept
		{
#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
			return ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)m_win->get_handle(), inst_clbk)
				&& ImGui_ImplOpenGL3_Init("#version 130");
#endif
		})()) { return debug::error("failed starting imgui"); }
		else
		{
			m_menubar.configure(j["menubar"]);
			
			m_dockspace.configure(j["dockspace"]);
			
			this->load_style(j["style_path"]);

			return true;
		}
	}

	bool imgui_runtime::shutdown()
	{
		if (!m_running) { return false; }
		else { m_running = false; }

#if defined(ML_IMPL_WINDOW_GLFW) && defined(ML_IMPL_RENDERER_OPENGL)
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
#endif

		ImGui::DestroyContext(m_ctx);

		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void imgui_runtime::new_frame()
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
					get_bus()->fire<imgui_dockspace_event>(&m_dockspace);
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
			get_bus()->fire<imgui_menubar_event>(&m_menubar);

			ImGui::EndMainMenuBar();
		}

		// RENDER
		get_bus()->fire<imgui_render_event>(this);
	}

	void imgui_runtime::render_frame()
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

	void imgui_runtime::menubar::configure(json const & j)
	{
		j["enabled"].get_to(enabled);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void imgui_runtime::dockspace::configure(json const & j)
	{
		j["enabled"	].get_to(enabled);
		j["title"	].get_to(title);
		j["border"	].get_to(border);
		j["rounding"].get_to(rounding);
		j["alpha"	].get_to(alpha);
		j["padding"	].get_to(padding);
		j["size"	].get_to(size);
		j["nodes"	].get_to(nodes);
	}

	uint32_t imgui_runtime::dockspace::begin_builder()
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

	uint32_t imgui_runtime::dockspace::end_builder(uint32_t root)
	{
		if (root) { ImGui::DockBuilderFinish(root); }
		return root;
	}

	uint32_t imgui_runtime::dockspace::dock(cstring name, uint32_t id)
	{
		if (!name || !id) { return NULL; }
		else
		{
			ImGui::DockBuilderDockWindow(name, id);
			return id;
		}
	}

	uint32_t imgui_runtime::dockspace::split(uint32_t i, uint32_t id, int32_t dir, float_t ratio, uint32_t * value)
	{
		return nodes[(size_t)i] = split(id, dir, ratio, value);
	}

	uint32_t imgui_runtime::dockspace::split(uint32_t id, int32_t dir, float_t ratio, uint32_t * value)
	{
		return this->split(id, dir, ratio, nullptr, value);
	}

	uint32_t imgui_runtime::dockspace::split(uint32_t id, int32_t dir, float_t ratio, uint32_t * out, uint32_t * value)
	{
		return ImGui::DockBuilderSplitNode(id, dir, ratio, out, value);
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
		case hashof("classic"): ImGui::StyleColorsClassic()	; return true;
		case hashof("dark")	: ImGui::StyleColorsDark()		; return true;
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
			case hashof("Border")					: style.Colors[ImGuiCol_Border] = parse<vec4>(ss); break;
			case hashof("BorderShadow")			: style.Colors[ImGuiCol_BorderShadow] = parse<vec4>(ss); break;
			case hashof("FrameBg")				: style.Colors[ImGuiCol_FrameBg] = parse<vec4>(ss); break;
			case hashof("FrameBgHovered")			: style.Colors[ImGuiCol_FrameBgHovered] = parse<vec4>(ss); break;
			case hashof("FrameBgActive")			: style.Colors[ImGuiCol_FrameBgActive] = parse<vec4>(ss); break;
			case hashof("TitleBg")				: style.Colors[ImGuiCol_TitleBg] = parse<vec4>(ss); break;
			case hashof("TitleBgActive")			: style.Colors[ImGuiCol_TitleBgActive] = parse<vec4>(ss); break;
			case hashof("TitleBgCollapsed")		: style.Colors[ImGuiCol_TitleBgCollapsed] = parse<vec4>(ss); break;
			case hashof("MenuBarBg")				: style.Colors[ImGuiCol_MenuBarBg] = parse<vec4>(ss); break;
			case hashof("ScrollbarBg")			: style.Colors[ImGuiCol_ScrollbarBg] = parse<vec4>(ss); break;
			case hashof("ScrollbarGrab")			: style.Colors[ImGuiCol_ScrollbarGrab] = parse<vec4>(ss); break;
			case hashof("ScrollbarGrabHovered")	: style.Colors[ImGuiCol_ScrollbarGrabHovered] = parse<vec4>(ss); break;
			case hashof("ScrollbarGrabActive")	: style.Colors[ImGuiCol_ScrollbarGrabActive] = parse<vec4>(ss); break;
			case hashof("CheckMark")				: style.Colors[ImGuiCol_CheckMark] = parse<vec4>(ss); break;
			case hashof("SliderGrab")				: style.Colors[ImGuiCol_SliderGrab] = parse<vec4>(ss); break;
			case hashof("SliderGrabActive")		: style.Colors[ImGuiCol_SliderGrabActive] = parse<vec4>(ss); break;
			case hashof("Button")					: style.Colors[ImGuiCol_Button] = parse<vec4>(ss); break;
			case hashof("ButtonHovered")			: style.Colors[ImGuiCol_ButtonHovered] = parse<vec4>(ss); break;
			case hashof("ButtonActive")			: style.Colors[ImGuiCol_ButtonActive] = parse<vec4>(ss); break;
			case hashof("Header")					: style.Colors[ImGuiCol_Header] = parse<vec4>(ss); break;
			case hashof("HeaderHovered")			: style.Colors[ImGuiCol_HeaderHovered] = parse<vec4>(ss); break;
			case hashof("HeaderActive")			: style.Colors[ImGuiCol_HeaderActive] = parse<vec4>(ss); break;
			case hashof("Separator")				: style.Colors[ImGuiCol_Separator] = parse<vec4>(ss); break;
			case hashof("SeparatorHovered")		: style.Colors[ImGuiCol_SeparatorHovered] = parse<vec4>(ss); break;
			case hashof("SeparatorActive")		: style.Colors[ImGuiCol_SeparatorActive] = parse<vec4>(ss); break;
			case hashof("ResizeGrip")				: style.Colors[ImGuiCol_ResizeGrip] = parse<vec4>(ss); break;
			case hashof("ResizeGripHovered")		: style.Colors[ImGuiCol_ResizeGripHovered] = parse<vec4>(ss); break;
			case hashof("ResizeGripActive")		: style.Colors[ImGuiCol_ResizeGripActive] = parse<vec4>(ss); break;
			case hashof("Tab")					: style.Colors[ImGuiCol_Tab] = parse<vec4>(ss); break;
			case hashof("TabHovered")				: style.Colors[ImGuiCol_TabHovered] = parse<vec4>(ss); break;
			case hashof("TabActive")				: style.Colors[ImGuiCol_TabActive] = parse<vec4>(ss); break;
			case hashof("TabUnfocused")			: style.Colors[ImGuiCol_TabUnfocused] = parse<vec4>(ss); break;
			case hashof("TabUnfocusedActive")		: style.Colors[ImGuiCol_TabUnfocusedActive] = parse<vec4>(ss); break;
			case hashof("DockingPreview")			: style.Colors[ImGuiCol_DockingPreview] = parse<vec4>(ss); break;
			case hashof("DockingEmptyBg")			: style.Colors[ImGuiCol_DockingEmptyBg] = parse<vec4>(ss); break;
			case hashof("PlotLines")				: style.Colors[ImGuiCol_PlotLines] = parse<vec4>(ss); break;
			case hashof("PlotLinesHovered")		: style.Colors[ImGuiCol_PlotLinesHovered] = parse<vec4>(ss); break;
			case hashof("PlotHistogram")			: style.Colors[ImGuiCol_PlotHistogram] = parse<vec4>(ss); break;
			case hashof("PlotHistogramHovered")	: style.Colors[ImGuiCol_PlotHistogramHovered] = parse<vec4>(ss); break;
			case hashof("TextSelectedBg")			: style.Colors[ImGuiCol_TextSelectedBg] = parse<vec4>(ss); break;
			case hashof("DragDropTarget")			: style.Colors[ImGuiCol_DragDropTarget] = parse<vec4>(ss); break;
			case hashof("NavHighlight")			: style.Colors[ImGuiCol_NavHighlight] = parse<vec4>(ss); break;
			case hashof("NavWindowingHighlight")	: style.Colors[ImGuiCol_NavWindowingHighlight] = parse<vec4>(ss); break;
			case hashof("NavWindowingDimBg")		: style.Colors[ImGuiCol_NavWindowingDimBg] = parse<vec4>(ss); break;
			case hashof("ModalWindowDimBg")		: style.Colors[ImGuiCol_ModalWindowDimBg] = parse<vec4>(ss); break;
			} } break;
			/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
			}
		}
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}