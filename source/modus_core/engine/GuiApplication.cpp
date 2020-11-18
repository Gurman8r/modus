#include <modus_core/engine/GuiApplication.hpp>
#include <modus_core/engine/EngineEvents.hpp>
#include <modus_core/embed/Python.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	gui_application::gui_application(int32 argc, char * argv[], allocator_type alloc)
		: core_application	{ argc, argv, alloc }
		, m_window			{ get_bus(), alloc }
		, m_fps				{}
	{
		ML_assert(begin_singleton<gui_application>(this));

		main_loop()->set_loop_condition(&main_window::is_open, &m_window);
		main_loop()->set_enter_callback([&]() { get_bus()->fire<app_enter_event>(); });
		main_loop()->set_exit_callback([&]() { get_bus()->fire<app_exit_event>(); });
		main_loop()->set_idle_callback([&](auto) { get_bus()->fire<app_idle_event>(); });
	}

	gui_application::~gui_application() noexcept
	{
		ML_assert(end_singleton<gui_application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void gui_application::on_event(event const & value)
	{
		core_application::on_event(value);
		switch (value)
		{
		case app_enter_event::ID: {
			auto && ev{ (app_enter_event &&)value };

			// open window
			ML_assert(attr().contains("window"));
			json & window_prefs{ attr("window") };
			ML_assert(m_window.open(
				window_prefs["title"],
				window_prefs["video"],
				window_prefs["context"],
				window_prefs["hints"]
			));

			// configure imgui
			if (attr().contains("imgui")) {
				json & gui_prefs{ attr("imgui") };
				if (gui_prefs.contains("dockspace")) {
					m_window.get_dockspace()->Configure(gui_prefs["dockspace"]);
				}
				if (gui_prefs.contains("style")) {
					json & style_prefs{ gui_prefs["style"] };
					if (style_prefs.is_string()) {
						m_window.load_imgui_style(path_to(style_prefs));
					}
				}
			}

		} break;

		case app_exit_event::ID: {
			auto && ev{ (app_exit_event &&)value };
		} break;

		case app_idle_event::ID: {
			auto && ev{ (app_idle_event &&)value };

			// fps
			auto const dt{ main_loop()->delta_time().count() };
			m_fps.accum += dt - m_fps.times[m_fps.index];
			m_fps.times[m_fps.index] = dt;
			m_fps.index = (m_fps.index + 1) % m_fps.times.size();
			m_fps.value = (0.f < m_fps.accum)
				? 1.f / (m_fps.accum / (float32)m_fps.times.size())
				: FLT_MAX;

			// imgui
			m_window.do_imgui_frame([&
				, context	= m_window.get_imgui().get()
				, dockspace	= m_window.get_dockspace()
			](auto) noexcept
			{
				dockspace->SetWindowFlag(
					ImGuiWindowFlags_MenuBar,
					ImGui::FindWindowByName("##MainMenuBar"));

				(*dockspace)(context->Viewports[0], [&](auto) noexcept
				{
					get_bus()->fire<imgui_dockspace_event>(dockspace);
				});

				get_bus()->fire<imgui_render_event>(context);
			});

		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global gui_application
namespace ml::globals
{
	static gui_application * g_gui_application{};

	ML_impl_global(gui_application) get() noexcept
	{
		return g_gui_application;
	}

	ML_impl_global(gui_application) set(gui_application * value) noexcept
	{
		return g_gui_application = value;
	}
}